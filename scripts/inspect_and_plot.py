#!/usr/bin/env python3
"""
Inspect ROOT file for histogram prefixes (e.g. h5, h7), list histograms per prefix,
and optionally plot all histograms for a chosen prefix into a multipage PDF.

Requires: uproot, numpy, matplotlib
Install: pip install uproot numpy matplotlib

Usage. For inspection:

    python3 inspect_and_plot_prefixes.py hist_file.root

For list and plot: 

    python3 inspect_and_plot_prefixes.py hist_file.root --plot-prefix h5 --inputs hist_file.root other_sim.root -o h5_plots.pdf

    (for plot multiple)

"""
from __future__ import annotations

import argparse
import os
import re
from collections import defaultdict
from typing import Dict, List, Tuple

import numpy as np
import uproot
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

PREFIX_RE = re.compile(r"^([A-Za-z]+)(\d)")  # captures letters then first digit, e.g. 'h' + '7' -> 'h7'
COLORS = ["C0", "C1", "C2", "C3", "C4", "C5"]


def normalize_key(key) -> str:
    """Return a str name for a key (handle bytes keys and strip cycle suffix like ;1)."""
    name = key.decode() if isinstance(key, (bytes, bytearray)) else str(key)
    # strip ROOT cycle suffix if present
    if ";" in name:
        name = name.split(";", 1)[0]
    return name


def detect_prefix(name: str) -> str:
    """Return a prefix like 'h5' or 'h7' if matched, else return the leading letters."""
    m = PREFIX_RE.match(name)
    if m:
        return f"{m.group(1)}{m.group(2)}"
    # fallback: take leading letters
    m2 = re.match(r"^([A-Za-z]+)", name)
    return m2.group(1) if m2 else ""


def list_histograms_by_prefix(rootfile: str) -> Dict[str, List[str]]:
    """Open ROOT file and return a dict mapping prefix -> list of histogram names (no cycle)."""
    groups: Dict[str, List[str]] = defaultdict(list)
    with uproot.open(rootfile) as f:
        for key in f.keys():
            name = normalize_key(key)
            # only consider TH1/TH2-like objects; try to access to_numpy lazily later
            prefix = detect_prefix(name)
            groups[prefix].append(name)
    # sort lists for stable output
    for k in groups:
        groups[k].sort()
    return dict(groups)


def print_summary(groups: Dict[str, List[str]]) -> None:
    """Print a compact summary of prefixes and counts, and list histograms per prefix."""
    print("Detected prefixes and counts:")
    for prefix, names in sorted(groups.items()):
        print(f"  {prefix!r}: {len(names)} histograms")
    print()
    for prefix, names in sorted(groups.items()):
        print(f"Prefix {prefix!r}:")
        # show up to first 50 names, then indicate more
        sample = names[:50]
        for n in sample:
            print(f"  {n}")
        if len(names) > len(sample):
            print(f"  ... and {len(names) - len(sample)} more")
        print()


def read_hist_values(rootfile: str, histname: str) -> Tuple[np.ndarray, np.ndarray]:
    """Return (values, edges) for a 1D histogram. Raises KeyError if not found or not 1D."""
    with uproot.open(rootfile) as f:
        obj = f[histname]
        values, edges = obj.to_numpy(flow=False)
    return values, edges


def plot_prefix_histograms(input_files: List[str], prefix: str, output_pdf: str, x_range=(0.0, 3.0)) -> None:
    """
    For each histogram whose name starts with the given prefix, plot that histogram
    from each input file on a single page and save pages to output_pdf.
    """
    # collect unique histogram names across files that start with prefix
    histnames = set()
    for infile in input_files:
        with uproot.open(infile) as f:
            for key in f.keys():
                name = normalize_key(key)
                if name.startswith(prefix):
                    histnames.add(name)
    histnames = sorted(histnames)
    if not histnames:
        raise ValueError(f"No histograms found with prefix {prefix!r} in the provided files.")

    with PdfPages(output_pdf) as pdf:
        for histname in histnames:
            plt.figure(figsize=(6, 6))
            ax = plt.gca()
            plotted = False
            for idx, infile in enumerate(input_files):
                try:
                    vals, edges = read_hist_values(infile, histname)
                except Exception:
                    # histogram missing or not readable in this file
                    continue
                ax.step(edges, np.append(vals, vals[-1]), where="post",
                        color=COLORS[idx % len(COLORS)],
                        label=os.path.splitext(os.path.basename(infile))[0],
                        linewidth=1.6)
                plotted = True

            if not plotted:
                plt.close()
                continue

            ax.set_title(histname)
            ax.set_xlim(*x_range)
            ax.set_xlabel("Energy [GeV]")
            ax.set_ylabel("Flux")
            ax.grid(True, linestyle=":", alpha=0.5)
            ax.legend(frameon=False, loc="best")
            plt.tight_layout()
            pdf.savefig()
            plt.close()


def parse_args():
    p = argparse.ArgumentParser(description="Detect histogram prefixes and optionally plot a prefix.")
    p.add_argument("rootfile", help="ROOT file to inspect")
    p.add_argument("--plot-prefix", "-p", help="Prefix to plot (e.g. h5 or h7). If omitted, only lists histograms.")
    p.add_argument("--inputs", "-i", nargs="+", help="One or more ROOT files to plot from (required if --plot-prefix used).")
    p.add_argument("--output", "-o", default="prefix_plots.pdf", help="Output PDF when plotting (default: prefix_plots.pdf).")
    return p.parse_args()


def main():
    args = parse_args()
    if not os.path.isfile(args.rootfile):
        raise SystemExit(f"File not found: {args.rootfile}")

    groups = list_histograms_by_prefix(args.rootfile)
    print_summary(groups)

    if args.plot_prefix:
        if not args.inputs:
            raise SystemExit("When using --plot-prefix you must provide --inputs with one or more ROOT files.")
        # ensure inputs exist
        for p in args.inputs:
            if not os.path.isfile(p):
                raise SystemExit(f"Input file not found: {p}")
        print(f"Plotting prefix {args.plot_prefix!r} from {len(args.inputs)} input file(s) into {args.output}")
        plot_prefix_histograms(args.inputs, args.plot_prefix, args.output)
        print("Done. PDF written.")


if __name__ == "__main__":
    main()
