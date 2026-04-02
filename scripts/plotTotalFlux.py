#!/usr/bin/env python3
"""
Plot h5{sector}{inu} neutrino flux histograms from G4BNB simulation ROOT files.

This script mirrors the h7 plotting script but targets h5 histograms instead.
It uses uproot (pure Python) and matplotlib to generate a multipage PDF.

Usage:
    python3 plot_h5.py -i file1.root file2.root -o h5_plots.pdf
"""

import argparse
import os
import numpy as np
import uproot
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

COLORS = ["C0", "C1", "C2", "C3", "C4", "C5"]


def read_hist(root_path, hist_name):
    """Return (values, edges) for a 1D histogram."""
    with uproot.open(root_path) as f:
        h = f[hist_name]
        return h.to_numpy(flow=False)


def plot_h5(input_files, output_pdf, x_range=(0.0, 3.0)):
    neutrino_names = {1: "nue", 2: "nuebar", 3: "numu", 4: "numubar"}

    # Validate input files
    for f in input_files:
        if not os.path.isfile(f):
            raise FileNotFoundError(f"Input file not found: {f}")

    with PdfPages(output_pdf) as pdf:
        for inu in range(1, 5):       # neutrino types
            for isec in range(0, 6):  # sectors
                hname = f"h5{isec}{inu}"

                plt.figure(figsize=(6, 6))
                ax = plt.gca()
                plotted = False

                for idx, infile in enumerate(input_files):
                    try:
                        vals, edges = read_hist(infile, hname)
                    except KeyError:
                        continue

                    # Step plot
                    ax.step(
                        edges,
                        np.append(vals, vals[-1]),
                        where="post",
                        color=COLORS[idx % len(COLORS)],
                        label=os.path.splitext(os.path.basename(infile))[0],
                        linewidth=1.8,
                    )
                    plotted = True

                if not plotted:
                    plt.close()
                    continue

                ax.set_title(f"{hname} ({neutrino_names.get(inu, 'unknown')}, sector {isec})")
                ax.set_xlim(*x_range)
                ax.set_xlabel("Energy [GeV]")
                ax.set_ylabel("Flux")
                ax.grid(True, linestyle=":", alpha=0.5)
                ax.legend(frameon=False)
                plt.tight_layout()
                pdf.savefig()
                plt.close()


def parse_args():
    p = argparse.ArgumentParser(description="Plot h5 histograms from G4BNB ROOT files.")
    p.add_argument("-i", "--input", required=True, nargs="+",
                   help="Simulation ROOT files containing h5 histograms.")
    p.add_argument("-o", "--output", default="h5_plots.pdf",
                   help="Output PDF file (default: h5_plots.pdf).")
    return p.parse_args()


def main():
    args = parse_args()
    plot_h5(args.input, args.output)


if __name__ == "__main__":
    main()
