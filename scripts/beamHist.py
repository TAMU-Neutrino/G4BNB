#!/usr/bin/env python3
"""Python port of beamHist.cc: standard flux histograms from dk2nu files.

Produces the same ROOT file layout as the C++ (via uproot, no ROOT needed):
  h_xyE            TH3F of (x, y, E) fills, NOT scaled by POT
  h501..h504       flux per species (nue, nuebar, numu, numubar), /POT
  h5{p}{n}         flux by parent: p = 1 mu+-, 2 pi+-, 3 K0L, 4 K+-
  h701..h704       copies of h501..h504 (MiniBooNE file convention)
  h7{s}{n}         flux by first inelastic secondary: s = 1 pi->..->mu,
                   2 pi (not mu), 3 K0L, 4 K+-, 5 p/n
All 1D histograms: 200 bins, 0-10 GeV, with sumw2 errors.

The histograms are filled from the flat weighted ntuple built by
beam_ntuple.py; use that module directly to manipulate the ntuple instead
of (or in addition to) these histograms.  --save-ntuple dumps it to .npz.

Differences from the C++: --detector-radius is required (the C++ default 0
silently zeroes every weight); no --thread (numpy is vectorised); the disk
sampling RNG differs, so bins agree only within MC statistics.

Example (MiniBooNE position, as in submitBeam.py):
  python3 beamHist.py --input 'production_1e6/*.dk2nu.root' \
      --detector-radius 610 --detector-position 0 189.614 54134 \
      --output hist_miniboone.root
"""

import argparse
import os
import sys

import numpy as np
import uproot
from uproot.writing.identify import to_TAxis, to_TH1x, to_TH3x

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import beam_ntuple as bn

NBINS_E, EMIN, EMAX = 200, 0.0, 10.0
NBINS_XY = 100

# Titles verbatim from beamHist.cc
NULTX = ["#nu_{e}", "#bar{#nu}_{e}", "#nu_{#mu}", "#bar{#nu}_{#mu}"]
PDGCODE = [12, -12, 14, -14]
PLTX = ["#mu^{#pm}", "#pi^{#pm}", "K^{0}_{L}", "K^{#pm}"]
SECLTX = ["pBe->#pi^{#pm}->...->#mu^{#pm}",
          "pBe->#pi^{#pm}->..(not #mu^{#pm})..",
          "pBe->K^{0}_{L}->...",
          "pBe->K^{#pm}->...",
          "pBe->(p or n)->..."]


def root_bin_index(x, edges):
    """ROOT bin numbering: 0 underflow, 1..n in-range ([lo, hi)), n+1 overflow."""
    return np.searchsorted(edges, x, side="right")


def fill1d(x, w, edges):
    """Return (contents, sumw2, stats, nfills) with under/overflow, ROOT-style."""
    nb = len(edges) - 1
    idx = root_bin_index(x, edges)
    data = np.bincount(idx, weights=w, minlength=nb + 2)
    sumw2 = np.bincount(idx, weights=w * w, minlength=nb + 2)
    inr = (idx >= 1) & (idx <= nb)
    xi, wi = x[inr], w[inr]
    stats = {"fTsumw": wi.sum(), "fTsumw2": (wi * wi).sum(),
             "fTsumwx": (wi * xi).sum(), "fTsumwx2": (wi * xi * xi).sum()}
    return data, sumw2, stats, len(x)


def fill3d(x, y, z, w, ex, ey, ez):
    """3D fill in ROOT's global-bin layout (x fastest)."""
    nx, ny, nz = len(ex) - 1, len(ey) - 1, len(ez) - 1
    ix = root_bin_index(x, ex)
    iy = root_bin_index(y, ey)
    iz = root_bin_index(z, ez)
    lin = ix + (nx + 2) * (iy + (ny + 2) * iz)
    data = np.bincount(lin, weights=w, minlength=(nx + 2) * (ny + 2) * (nz + 2))
    inr = (ix >= 1) & (ix <= nx) & (iy >= 1) & (iy <= ny) & (iz >= 1) & (iz <= nz)
    xi, yi, zi, wi = x[inr], y[inr], z[inr], w[inr]
    stats = {"fTsumw": wi.sum(), "fTsumw2": (wi * wi).sum(),
             "fTsumwx": (wi * xi).sum(), "fTsumwx2": (wi * xi * xi).sum(),
             "fTsumwy": (wi * yi).sum(), "fTsumwy2": (wi * yi * yi).sum(),
             "fTsumwxy": (wi * xi * yi).sum(),
             "fTsumwz": (wi * zi).sum(), "fTsumwz2": (wi * zi * zi).sum(),
             "fTsumwxz": (wi * xi * zi).sum(), "fTsumwyz": (wi * yi * zi).sum()}
    return data, stats, len(x)


def split_title(title):
    """ROOT-style 'title;xtitle;ytitle;ztitle' -> (title, [axis titles])."""
    parts = title.split(";")
    return parts[0], parts[1:] + [""] * (3 - len(parts[1:]))


def th1f(name, title, data, sumw2, stats, nfills, edges, scale=1.0):
    base, (xt, yt, _) = split_title(title)
    s = float(scale)
    return to_TH1x(
        fName=name, fTitle=base,
        data=(data * s).astype(np.float32),
        fEntries=float(nfills),
        fTsumw=stats["fTsumw"] * s, fTsumw2=stats["fTsumw2"] * s * s,
        fTsumwx=stats["fTsumwx"] * s, fTsumwx2=stats["fTsumwx2"] * s,
        fSumw2=sumw2 * s * s,
        fXaxis=to_TAxis(fName="xaxis", fTitle=xt, fNbins=len(edges) - 1,
                        fXmin=edges[0], fXmax=edges[-1]),
        fYaxis=to_TAxis(fName="yaxis", fTitle=yt, fNbins=1, fXmin=0.0, fXmax=1.0))


def th3f(name, title, data, stats, nfills, ex, ey, ez):
    base, (xt, yt, zt) = split_title(title)
    axis = lambda nm, t, e: to_TAxis(fName=nm, fTitle=t, fNbins=len(e) - 1,
                                     fXmin=e[0], fXmax=e[-1])
    return to_TH3x(
        fName=name, fTitle=base,
        data=data.astype(np.float32),
        fEntries=float(nfills),
        fTsumw=stats["fTsumw"], fTsumw2=stats["fTsumw2"],
        fTsumwx=stats["fTsumwx"], fTsumwx2=stats["fTsumwx2"],
        fTsumwy=stats["fTsumwy"], fTsumwy2=stats["fTsumwy2"],
        fTsumwxy=stats["fTsumwxy"],
        fTsumwz=stats["fTsumwz"], fTsumwz2=stats["fTsumwz2"],
        fTsumwxz=stats["fTsumwxz"], fTsumwyz=stats["fTsumwyz"],
        fSumw2=np.array([], np.float64),  # C++ never calls Sumw2 on the TH3
        fXaxis=axis("xaxis", xt, ex), fYaxis=axis("yaxis", yt, ey),
        fZaxis=axis("zaxis", zt, ez))


def main():
    ap = argparse.ArgumentParser(
        description=__doc__.splitlines()[0],
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    ap.add_argument("--input", required=True, nargs="+",
                    help="Input file(s) / glob pattern(s); quote patterns "
                         "or let the shell expand them")
    ap.add_argument("--output", default="hist.root", help="Output file name")
    ap.add_argument("--pot", type=float, default=None,
                    help="POT for normalization (overrides the dkmetaTree "
                         "count; give the TOTAL: files x POT per file)")
    ap.add_argument("--nredecays", type=int, default=1,
                    help="Number of redecays per dk2nu entry")
    ap.add_argument("--detector-radius", type=float, required=True,
                    help="Detector radius in cm (must be > 0)")
    ap.add_argument("--detector-position", type=float, nargs=3,
                    default=[0.0, 0.0, 47000.0], metavar=("X", "Y", "Z"),
                    help="Detector position in cm (default: MicroBooNE)")
    ap.add_argument("--seed", type=int, default=0,
                    help="Random seed for the disk sampling")
    ap.add_argument("--save-ntuple", default=None, metavar="FILE.npz",
                    help="Also save the weighted ntuple as npz")
    args = ap.parse_args()

    detpos = tuple(args.detector_position)
    rdet = args.detector_radius
    print(f"Searching {' '.join(args.input)}")
    print(f"Making histograms for detector at r=({detpos[0]}, {detpos[1]}, "
          f"{detpos[2]}) cm and smearing over RDet={rdet} cm")
    print(f"Redecaying {args.nredecays} times.")

    nt = bn.load_beam_ntuple(args.input, detpos=detpos, rdet=rdet,
                             nredecay=args.nredecays, seed=args.seed)
    t = nt.table

    if args.pot is not None:
        tot_pot = args.pot
        print(f"POT set using --pot option to {tot_pot}")
    else:
        # As in the C++: counted POT includes the redecay factor
        tot_pot = nt.pot_effective
        print(f"Total POT summed using meta data= {tot_pot}")
    print(f"Ntuple rows: {len(t)}")

    e_edges = np.linspace(EMIN, EMAX, NBINS_E + 1)
    xy_edges = np.linspace(-rdet, rdet, NBINS_XY + 1)

    hists = []  # (name, model) in the C++ Write() order

    d3, s3, n3 = fill3d(t["x"], t["y"], t["enu"], t["wgt"],
                        xy_edges, xy_edges, e_edges)
    title3 = (f"Neutrino vertices at r=({detpos[0]:f},{detpos[1]:f},"
              f"{detpos[2]:f})cm;x (cm);y (cm);E (GeV)")
    hists.append(("h_xyE", th3f("h_xyE", title3, d3, s3, n3,
                                xy_edges, xy_edges, e_edges)))

    flux_parts = {}  # cache h50x pieces for the h70x copies
    for inu, (pdg, ltx) in enumerate(zip(PDGCODE, NULTX)):
        sel = t[t["ntype"] == pdg]
        parts = fill1d(sel["enu"], sel["wgt"], e_edges)
        flux_parts[inu] = parts
        title = f"{ltx} (all);Energy {ltx} (GeV);#phi({ltx})/50MeV/POT"
        hists.append((f"h50{inu + 1}",
                      th1f(f"h50{inu + 1}", title, *parts, e_edges,
                           scale=1.0 / tot_pot)))
        for ipar, pl in enumerate(PLTX):
            ps = sel[sel["parent_cat"] == ipar]
            title = (f"...->{pl}->{ltx};Energy {ltx} (GeV);"
                     f"#phi({ltx})/50MeV/POT")
            hists.append((f"h5{ipar + 1}{inu + 1}",
                          th1f(f"h5{ipar + 1}{inu + 1}", title,
                               *fill1d(ps["enu"], ps["wgt"], e_edges),
                               e_edges, scale=1.0 / tot_pot)))

    for inu, (pdg, ltx) in enumerate(zip(PDGCODE, NULTX)):
        sel = t[t["ntype"] == pdg]
        title = f"{ltx} (all);Energy {ltx} (GeV);#phi({ltx})/50MeV/POT"
        hists.append((f"h70{inu + 1}",
                      th1f(f"h70{inu + 1}", title, *flux_parts[inu], e_edges,
                           scale=1.0 / tot_pot)))
        for isec, sl in enumerate(SECLTX):
            ss = sel[sel["sec_cat"] == isec]
            title = f"{sl}->{ltx};Energy {ltx} (GeV);#phi({ltx})/50MeV/POT"
            hists.append((f"h7{isec + 1}{inu + 1}",
                          th1f(f"h7{isec + 1}{inu + 1}", title,
                               *fill1d(ss["enu"], ss["wgt"], e_edges),
                               e_edges, scale=1.0 / tot_pot)))

    with uproot.recreate(args.output) as fout:
        for name, model in hists:
            fout[name] = model
    print(f"Wrote {len(hists)} histograms to {args.output}")

    if args.save_ntuple:
        np.savez_compressed(args.save_ntuple, table=t, pot=nt.pot,
                            nredecay=nt.nredecay, detpos=np.array(nt.detpos),
                            rdet=nt.rdet, files=np.array(nt.files))
        print(f"Saved ntuple ({len(t)} rows) to {args.save_ntuple}")


if __name__ == "__main__":
    main()
