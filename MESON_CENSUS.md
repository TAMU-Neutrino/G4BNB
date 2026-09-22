# Optional meson census and positive production interpolation

The vector-portal extension is opt-in:

```
/boone/output/saveMesonNtuple true
```

`mesonTree` records pi+/pi-/pi0/K+/K- births (stage=0) before stacking
transport cuts, and charged meson decays (stage=1) independently of neutrino
mode. Each record contains PDG identity, event/track/parent identifiers,
statistical weight, four-momentum [GeV], position [cm], time [ns], process
name/type, material and quasi-elastic flag. A (track, stage) guard prevents
repeated records. Births preserve pre-transport positions and directions. Elastic ancestry track
continuations have stage=2 and must never count as produced pions. Schema-1
files can be read by excluding creator processes BooNEHadronElastic and
hadElastic from production. The process field records the creator process;
the stage distinguishes birth, decay and continuation.
`mesonMeta` schema 2 stores the completed run POT, run identifier and both
actual horn currents [A]. Existing dk2nu output remains available.

The charged-pion-average neutral estimate belongs in the downstream reader:
use one half of charged-pion hadronic births, excluding elastic continuations
and quasi-elastic recycling. The Table II reference counts forward pions with
kinetic energy >1 MeV and transverse momentum <1 GeV. Native hadronic pi0 is an alternative, not an added contribution;
native pi0 from decays is a separate source. Do not use focused pion decay
positions or directions as a neutral production distribution.

For RHC reverse BOTH `/boone/field/horncurrent` and
`/boone/field/skin/SkinDepthHornCurrent`. The geometry constructor now retains
the actual skin-field member used by the messenger and metadata writer.

The census exposed negative statistical weights in the old first-order
additive table interpolation. `BooNETableInterpolation.hh` uses positive
multilinear interpolation with clamped endpoints. Multiplicity integrates
that same interpolant over the rejection sampler's domain. Missing proposal
support raises an error. No negative event weight is clipped downstream.

The standalone numerical check requires no Geant4 runtime:

```
c++ -std=c++11 -Iinclude tests/test_table_interpolation.cc -o /tmp/test_g4_table
/tmp/test_g4_table
```

Reproducible matched run and neutrino-flux validation drivers live in the
companion BSM-beam repository as `scripts/run_bnb_mesons.py` and
`scripts/validate_bnb_mesons.py`. Their sidecars record macro and executable
hashes, repository revision, seeds, actual currents and complete-file POT.
