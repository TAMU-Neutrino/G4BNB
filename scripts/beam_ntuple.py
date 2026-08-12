#!/usr/bin/env python3
"""Load G4BNB dk2nu files into flat numpy ntuples of neutrino decays with
per-detector flux weights.

Python port of the data-loading / weighting core of scripts/beamHist.cc.
Instead of filling histograms directly, this module returns the underlying
weighted ntuple (one row per decay x redecay) so it can be manipulated with
plain numpy before histogramming.

The weight calculation reimplements bsim::calcEnuWgt
(sources/dk2nu/tree/calcLocationWeights.cxx) in vectorised numpy, and the
per-fill weight reproduces beamHist.cc exactly:

    wgt = wgt_xy * nimpwt * RDet^2 * 1e-4

(the C++ writes wgt_xy*nimpwt/pi*RDet*RDet*pi*1e-4; the pi factors cancel).
This is the expected number of neutrinos crossing the detector disk of
radius RDet (cm) per simulated decay, provided the (x, y) sample point is
drawn uniformly over the disk, which redecay() does.

Flux per POT: histogram `enu` with weights `wgt` and divide by
`pot_effective` (= summed dkmetaTree POT x nredecay), e.g.

    import numpy as np
    import beam_ntuple as bn

    nt = bn.load_beam_ntuple("production_1e6/*.dk2nu.root",
                             detpos=(0.0, 189.614, 54134.0),  # MiniBooNE
                             rdet=610.0, nredecay=1, seed=0)
    t = nt.table
    numu_from_kplus = t[(t["ntype"] == 14) & (t["ptype"] == 321)]
    flux, edges = np.histogram(numu_from_kplus["enu"], bins=200, range=(0, 10),
                               weights=numu_from_kplus["wgt"])
    flux /= nt.pot_effective   # nu / 50 MeV / POT through the disk

Lower-level access: read_dk2nu() returns the raw per-decay columns (no
position sampling), redecay() expands them into the weighted table.  The
table's "src" column indexes back into the read_dk2nu() arrays, so any
decay-level quantity can be joined onto the redecayed rows.

Note on randomness: the (x, y) disk sampling uses numpy's default_rng, not
ROOT's TRandom3, so individual rows differ from a beamHist.cc run; only the
distributions (and any histogram, within MC statistics) are comparable.
"""

import glob
import os
import sys
from dataclasses import dataclass

import awkward as ak
import numpy as np
import uproot

# Neutrino species, in beamHist.cc index order (h501..h504)
NU_SPECIES = (12, -12, 14, -14)
NU_NAMES = {12: "nue", -12: "nuebar", 14: "numu", -14: "numubar"}

# Parent categories, in beamHist.cc order (h51x..h54x)
PARENT_CATEGORIES = ("mu", "pi", "K0L", "K")
# First-inelastic-secondary categories, in beamHist.cc order (h71x..h75x)
SEC_CATEGORIES = ("pi_to_mu", "pi_not_mu", "K0L", "K", "p_or_n")

# Parent masses in GeV, hard-coded to match dk2nu calcLocationWeights.cxx
# (Geant4 10.3 values, the non-HISTORIC_MASS branch)
PARENT_MASS_GEV = {
    211: 0.1395701, -211: 0.1395701,      # pi+-
    321: 0.493677, -321: 0.493677,        # K+-
    130: 0.497614, 310: 0.497614, 311: 0.497614,  # K0L / K0S / K0
    13: 0.1056583715, -13: 0.1056583715,  # mu-+
    3334: 1.67245, -3334: 1.67245,        # omega-+
    2112: 0.93956536, -2112: 0.93956536,  # n / nbar
}
_MUMASS = 0.1056583715
_KRDET = 100.0  # cm; calcEnuWgt returns flux through a 100 cm radius disk
_NDECAY_MUP = 11  # mu+ -> nue numubar e+ (bsim::dkp_mup_nusep)
_NDECAY_MUM = 12  # mu- -> nuebar numu e- (bsim::dkp_mum_nusep)

# Decay-level branches always read from dk2nuTree (short key = part after '.')
DECAY_BRANCHES = [
    "decay.ntype", "decay.ptype", "decay.ndecay", "decay.necm",
    "decay.vx", "decay.vy", "decay.vz",
    "decay.pdpx", "decay.pdpy", "decay.pdpz",
    "decay.ppenergy", "decay.ppdxdz", "decay.ppdydz", "decay.pppz",
    "decay.muparpx", "decay.muparpy", "decay.muparpz", "decay.mupare",
    "decay.nimpwt",
]


def expand_inputs(patterns):
    """Expand a path / glob pattern (or a list of them) into a sorted file list."""
    if isinstance(patterns, (str, os.PathLike)):
        patterns = [patterns]
    files = []
    for pattern in patterns:
        pattern = os.path.expanduser(str(pattern))
        matches = sorted(glob.glob(pattern))
        if matches:
            files.extend(matches)
        elif os.path.exists(pattern):
            files.append(pattern)
        else:
            print(f"warning: no matches for '{pattern}'", file=sys.stderr)
    return files


def _parent_category(ptype):
    """beamHist.cc parent classification: 0 mu+-, 1 pi+-, 2 K0L, 3 K+-, else -1."""
    cat = np.full(ptype.shape, -1, dtype=np.int8)
    a = np.abs(ptype)
    cat[a == 13] = 0
    cat[a == 211] = 1
    cat[ptype == 130] = 2
    cat[a == 321] = 3
    return cat


def _first_inelastic(anc_pdg, anc_proc):
    """PDG of the first ancestor produced by a *HadronInelastic* process.

    Mirrors the beamHist.cc scan
        while (ancestor[i].proc.find("HadronInelastic") == npos) i++;
    Returns (sec_pdg, found): pdg is 0 where no such ancestor exists
    (the C++ would read out of bounds there).
    """
    counts = ak.to_numpy(ak.num(anc_proc))
    n = len(counts)
    if n == 0 or counts.sum() == 0:
        return np.zeros(n, np.int32), np.zeros(n, bool)
    flat_proc = ak.to_numpy(ak.flatten(anc_proc))
    pattern = b"HadronInelastic" if flat_proc.dtype.kind == "S" else "HadronInelastic"
    hit_flat = np.char.find(flat_proc, pattern) >= 0
    hit = ak.unflatten(hit_flat, counts)
    found = ak.to_numpy(ak.any(hit, axis=1))
    # argmax of booleans = index of first True (0 if none; masked by `found`)
    first = ak.to_numpy(ak.fill_none(ak.argmax(hit, axis=1), 0)).astype(np.int64)
    flat_pdg = ak.to_numpy(ak.flatten(anc_pdg)).astype(np.int64)
    starts = np.zeros(n, np.int64)
    np.cumsum(counts[:-1], out=starts[1:])
    idx = np.minimum(starts + first, len(flat_pdg) - 1)
    sec_pdg = np.where(found, flat_pdg[idx], 0).astype(np.int32)
    return sec_pdg, found


def _sec_category(sec_pdg, found, ptype):
    """beamHist.cc secondary classification of the first-inelastic ancestor.

    0: |pdg|==211 and |ptype|==13 (pBe -> pi -> ... -> mu -> nu)
    1: |pdg|==211 otherwise
    2: |pdg|==130,  3: |pdg|==321,  4: pdg==2212 or 2112 (exact, no antis)
    -1: anything else (not filled by the C++)
    """
    cat = np.full(sec_pdg.shape, -1, dtype=np.int8)
    a = np.abs(sec_pdg)
    cat[a == 211] = 1
    cat[(a == 211) & (np.abs(ptype) == 13)] = 0
    cat[a == 130] = 2
    cat[a == 321] = 3
    cat[(sec_pdg == 2212) | (sec_pdg == 2112)] = 4
    cat[~found] = -1
    return cat


def read_dk2nu(inputs, species=NU_SPECIES, extra_branches=(), quiet=False):
    """Read dk2nu files into flat per-decay numpy arrays.

    Parameters
    ----------
    inputs : str or list of str
        File path(s) and/or glob pattern(s).
    species : sequence of int
        Neutrino PDG codes to keep (default: nue, nuebar, numu, numubar,
        i.e. everything beamHist.cc processes).
    extra_branches : sequence of str
        Additional flat (one value per decay) dk2nuTree branches to carry
        along, e.g. "decay.sumnimpwt2" or "potnum".  Stored under the part
        of the name after the last '.'.
    quiet : bool
        Suppress per-file progress printout.

    Returns
    -------
    decays : dict of str -> ndarray
        One entry per selected decay.  Keys: short branch names (ntype,
        ptype, ndecay, necm, vx..vz, pdpx..pdpz, ppenergy, ppdxdz, ppdydz,
        pppz, muparpx..mupare, nimpwt), plus file_index, entry (entry number
        within its file), parent_cat, sec_pdg, sec_cat, and any extras.
    pot : float
        Sum of dkmetaTree pots over all files (NOT scaled by any redecay
        factor).
    """
    files = expand_inputs(inputs)
    if not files:
        raise RuntimeError(f"no input files found for {inputs!r}")
    if not quiet:
        print(f"Found {len(files)} files")

    branches = list(DECAY_BRANCHES)
    for b in extra_branches:
        if b not in branches:
            branches.append(b)

    chunks = []
    pot = 0.0
    for file_index, path in enumerate(files):
        with uproot.open(path) as f:
            tree = f["dk2nuTree"]
            arrays = tree.arrays(branches, library="np")
            anc = tree.arrays(["ancestor.pdg", "ancestor.proc"], library="ak")
            pot += float(np.sum(f["dkmetaTree"]["pots"].array(library="np")))

        cols = {}
        for name, arr in arrays.items():
            key = name.split(".")[-1]
            if arr.dtype == object:
                raise RuntimeError(
                    f"branch '{name}' is not flat (one value per decay); "
                    "only scalar branches can be carried in the ntuple")
            cols[key] = arr

        keep = np.isin(cols["ntype"], np.asarray(species))
        entry = np.nonzero(keep)[0].astype(np.int64)
        cols = {k: v[keep] for k, v in cols.items()}
        cols["entry"] = entry
        cols["file_index"] = np.full(len(entry), file_index, dtype=np.int32)

        sec_pdg, found = _first_inelastic(anc["ancestor.pdg"][keep],
                                          anc["ancestor.proc"][keep])
        cols["parent_cat"] = _parent_category(cols["ptype"])
        cols["sec_pdg"] = sec_pdg
        cols["sec_cat"] = _sec_category(sec_pdg, found, cols["ptype"])
        chunks.append(cols)
        if not quiet:
            print(f"  [{file_index + 1}/{len(files)}] {os.path.basename(path)}: "
                  f"{len(entry)} decays")

    decays = {k: np.concatenate([c[k] for c in chunks]) for k in chunks[0]}
    return decays, pot


def calc_enu_wgt(decays, det_x, det_y, det_z):
    """Vectorised port of bsim::calcEnuWgt (dk2nu calcLocationWeights.cxx).

    decays : dict with the DECAY_BRANCHES columns (as from read_dk2nu)
    det_x, det_y, det_z : scalars or per-row arrays, position(s) in cm
        (beam coordinates) at which to evaluate the flux.

    Returns
    -------
    enu : ndarray -- neutrino energy at the detector point (GeV)
    wgt_xy : ndarray -- nuray weight (solid angle x boost x polarisation);
        flux through a 100 cm radius disk at that point per decay is
        wgt_xy * nimpwt.

    Rows the C++ rejects with an error code (unknown parent type, invalid
    Michel weight) come back with enu = wgt_xy = 0, as the C++ sets them.
    """
    ptype = decays["ptype"]
    ntype = decays["ntype"]
    ndecay = decays["ndecay"]
    necm = decays["necm"]
    n = len(ptype)

    parent_mass = np.full(n, np.nan)
    for pdg, mass in PARENT_MASS_GEV.items():
        parent_mass[ptype == pdg] = mass
    unknown = np.isnan(parent_mass)
    if np.any(unknown):
        print(f"calc_enu_wgt: {int(unknown.sum())} decays with unknown parent "
              f"type {sorted(set(ptype[unknown].tolist()))}, weight set to 0",
              file=sys.stderr)
        parent_mass[unknown] = 1.0  # placeholder; rows zeroed below

    pdpx, pdpy, pdpz = decays["pdpx"], decays["pdpy"], decays["pdpz"]
    parentp2 = pdpx * pdpx + pdpy * pdpy + pdpz * pdpz
    parent_energy = np.sqrt(parentp2 + parent_mass * parent_mass)
    parentp = np.sqrt(parentp2)
    gamma = parent_energy / parent_mass
    beta_mag = np.sqrt((gamma * gamma - 1.0) / (gamma * gamma))

    dx = np.asarray(det_x, dtype=np.float64) - decays["vx"]
    dy = np.asarray(det_y, dtype=np.float64) - decays["vy"]
    dz = np.asarray(det_z, dtype=np.float64) - decays["vz"]
    rad = np.sqrt(dx * dx + dy * dy + dz * dz)

    # Lorentz boost factor (isotropic decay in the parent frame)
    emrat = np.ones(n)
    moving = parentp > 0.0
    costh = (pdpx * dx + pdpy * dy + pdpz * dz)[moving] \
        / (parentp[moving] * rad[moving])
    np.clip(costh, -1.0, 1.0, out=costh)
    emrat[moving] = 1.0 / (gamma[moving] * (1.0 - beta_mag[moving] * costh))

    enu = emrat * necm

    # Solid angle of a 100 cm radius disk at distance rad (exact form)
    sangdet = (1.0 - np.cos(np.arctan(_KRDET / rad))) / 2.0
    wgt = sangdet * emrat * emrat

    # Polarised muon decay correction, only for tagged mu -> nu nu e decays
    mu = np.nonzero((ndecay == _NDECAY_MUP) | (ndecay == _NDECAY_MUM))[0]
    if len(mu) > 0:
        _apply_muon_polarisation(decays, mu, enu, wgt, parent_mass,
                                 parent_energy, gamma, dx, dy, dz, rad,
                                 ntype, necm)

    enu[unknown] = 0.0
    wgt[unknown] = 0.0
    return enu, wgt


def _apply_muon_polarisation(decays, i, enu, wgt, parent_mass, parent_energy,
                             gamma, dx, dy, dz, rad, ntype, necm):
    """In-place Michel polarisation correction for rows `i` (muon decays)."""
    enu_i = enu[i]
    pe = parent_energy[i]
    gam = gamma[i]

    # Boost the neutrino into the muon decay CM
    beta = np.column_stack([decays["pdpx"][i] / pe,
                            decays["pdpy"][i] / pe,
                            decays["pdpz"][i] / pe])
    p_nu = np.column_stack([dx[i] * enu_i / rad[i],
                            dy[i] * enu_i / rad[i],
                            dz[i] * enu_i / rad[i]])
    partial = enu_i - gam * np.sum(beta * p_nu, axis=1) / (gam + 1.0)
    p_dcm = p_nu - beta * (gam * partial)[:, None]
    p_dcm_mag = np.sqrt(np.sum(p_dcm * p_dcm, axis=1))

    # Boost the muon's parent into the muon production CM
    ppenergy = decays["ppenergy"][i]
    gam2 = ppenergy / parent_mass[i]
    beta2 = np.column_stack([decays["ppdxdz"][i] * decays["pppz"][i] / ppenergy,
                             decays["ppdydz"][i] * decays["pppz"][i] / ppenergy,
                             decays["pppz"][i] / ppenergy])
    mupar = np.column_stack([decays["muparpx"][i],
                             decays["muparpy"][i],
                             decays["muparpz"][i]])
    partial2 = decays["mupare"][i] - gam2 * np.sum(beta2 * mupar, axis=1) / (gam2 + 1.0)
    p_pcm = mupar - beta2 * (gam2 * partial2)[:, None]
    p_pcm_mag = np.sqrt(np.sum(p_pcm * p_pcm, axis=1))

    # C++ returns error 3 (keeping the unpolarised weight) if either
    # magnitude vanishes; mirror by only correcting the valid rows.
    eps = 1.0e-30
    valid = (p_dcm_mag > eps) & (p_pcm_mag > eps)
    vi = np.nonzero(valid)[0]
    if len(vi) == 0:
        return

    costh = np.sum(p_dcm[vi] * p_pcm[vi], axis=1) / (p_dcm_mag[vi] * p_pcm_mag[vi])
    np.clip(costh, -1.0, 1.0, out=costh)

    nt = ntype[i[vi]]
    ratio = np.ones(len(vi))
    is_nue = np.abs(nt) == 12
    ratio[is_nue] = 1.0 - costh[is_nue]
    is_numu = np.abs(nt) == 14
    if np.any(is_numu):
        xnu = 2.0 * necm[i[vi]][is_numu] / _MUMASS
        r = ((3.0 - 2.0 * xnu) - (1.0 - 2.0 * xnu) * costh[is_numu]) / (3.0 - 2.0 * xnu)
        bad = r < 0.0
        if np.any(bad):
            print(f"calc_enu_wgt: {int(bad.sum())} muon decays with negative "
                  "Michel weight, zeroed (C++ error 4)", file=sys.stderr)
            r[bad] = 0.0
        ratio[is_numu] = r
    # C++ error 2 (muon decay to a non nue/numu species) zeroes the row
    other = ~(is_nue | is_numu)
    ratio[other] = 0.0

    gi = i[vi]
    wgt[gi] *= ratio
    zeroed = ratio == 0.0
    enu[gi[zeroed]] = 0.0
    wgt[gi[zeroed]] = 0.0


# Table columns that redecay() builds beyond the identifier columns
_TABLE_ID_FIELDS = [
    ("file_index", np.int32), ("entry", np.int64), ("src", np.int64),
    ("ntype", np.int32), ("ptype", np.int32), ("ndecay", np.int32),
    ("parent_cat", np.int8), ("sec_cat", np.int8), ("sec_pdg", np.int32),
    ("nimpwt", np.float64),
]
_TABLE_CALC_FIELDS = [
    ("x", np.float64), ("y", np.float64), ("enu", np.float64),
    ("wgt_xy", np.float64), ("wgt", np.float64),
]


def redecay(decays, detpos, rdet, nredecay=1, seed=0, extra_columns=()):
    """Expand per-decay arrays into the weighted flux ntuple of beamHist.cc.

    Each decay is used `nredecay` times; each use samples a point (x, y)
    uniformly over the detector disk of radius `rdet` (cm) centred at
    `detpos` (cm, beam coordinates) and evaluates the neutrino energy and
    weight there.

    Returns a numpy structured array with one row per (decay, redecay):
      file_index, entry  -- source file / entry within it
      src                -- row index into the `decays` arrays
      ntype, ptype, ndecay, parent_cat, sec_cat, sec_pdg, nimpwt
      x, y               -- sampled offsets from the detector centre (cm)
      enu                -- neutrino energy at that point (GeV)
      wgt_xy             -- calcEnuWgt nuray weight
      wgt                -- beamHist.cc fill weight
                            (= wgt_xy * nimpwt * rdet^2 * 1e-4)
    plus any `extra_columns` (names of keys in `decays` to carry along).

    Histogramming `enu` with weights `wgt` and dividing by
    (POT * nredecay) reproduces the beamHist.cc flux histograms.
    """
    rdet = float(rdet)
    if rdet <= 0.0:
        raise ValueError(
            "rdet must be > 0 (beamHist.cc's default of 0 makes every "
            "weight identically zero)")
    nredecay = int(nredecay)
    if nredecay < 1:
        raise ValueError("nredecay must be >= 1")

    n = len(decays["ntype"])
    src = np.repeat(np.arange(n, dtype=np.int64), nredecay)

    rng = np.random.default_rng(seed)
    # Uniform over the disk (same distribution as the C++ accept/reject)
    r = rdet * np.sqrt(rng.random(len(src)))
    phi = rng.random(len(src)) * (2.0 * np.pi)
    x = r * np.cos(phi)
    y = r * np.sin(phi)

    expanded = {k: v[src] for k, v in decays.items()}
    enu, wgt_xy = calc_enu_wgt(expanded,
                               detpos[0] + x, detpos[1] + y, detpos[2])
    # beamHist.cc: wgt_xy*nimpwt/pi*RDet*RDet*pi*1e-4 (pi cancels)
    wgt = wgt_xy * expanded["nimpwt"] * (rdet * rdet * 1.0e-4)

    fields = list(_TABLE_ID_FIELDS) + list(_TABLE_CALC_FIELDS)
    for name in extra_columns:
        if name not in decays:
            raise KeyError(f"extra column '{name}' not in decays arrays")
        if name not in [f[0] for f in fields]:
            fields.append((name, decays[name].dtype))
    table = np.empty(len(src), dtype=fields)
    table["src"] = src
    for name, _ in _TABLE_ID_FIELDS:
        if name != "src":
            table[name] = expanded[name]
    table["x"] = x
    table["y"] = y
    table["enu"] = enu
    table["wgt_xy"] = wgt_xy
    table["wgt"] = wgt
    for name in extra_columns:
        table[name] = expanded[name]
    return table


@dataclass
class BeamNtuple:
    """Weighted flux ntuple plus the metadata needed to normalise it."""
    table: np.ndarray     # structured array, one row per (decay, redecay)
    pot: float            # summed dkmetaTree POT of the input files
    nredecay: int
    detpos: tuple
    rdet: float
    files: list

    @property
    def pot_effective(self):
        """Divide histogram yields by this for per-POT normalisation."""
        return self.pot * self.nredecay

    def __getitem__(self, key):
        return self.table[key]

    def __len__(self):
        return len(self.table)


def load_beam_ntuple(inputs, detpos=(0.0, 0.0, 47000.0), rdet=None,
                     nredecay=1, seed=0, species=NU_SPECIES,
                     extra_branches=(), quiet=False):
    """One-call convenience: read_dk2nu() + redecay() -> BeamNtuple.

    detpos default is MicroBooNE (0, 0, 47000) cm, as in beamHist.cc.
    rdet (cm) is required.  extra_branches are read from the tree and
    carried into the table (e.g. "decay.pdpz" appears as column "pdpz").
    """
    if rdet is None:
        raise ValueError("rdet (detector radius in cm) is required")
    decays, pot = read_dk2nu(inputs, species=species,
                             extra_branches=extra_branches, quiet=quiet)
    extra_cols = [b.split(".")[-1] for b in extra_branches]
    table = redecay(decays, detpos, rdet, nredecay=nredecay, seed=seed,
                    extra_columns=extra_cols)
    return BeamNtuple(table=table, pot=pot, nredecay=int(nredecay),
                      detpos=tuple(float(v) for v in detpos),
                      rdet=float(rdet), files=expand_inputs(inputs))
