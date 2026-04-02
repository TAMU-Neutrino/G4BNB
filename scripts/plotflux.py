#!/usr/bin/env python3
"""
Plot neutrino flux histograms produced by G4BNB simulation ROOT files.

This modernized script uses uproot (uproot4) to read TH1 histograms and
matplotlib to produce a multipage PDF with one plot per (sector, neutrino-type).
It intentionally skips reading any old reference ROOT file and therefore
does not perform any comparisons or ratio plots.

Usage:
    python3 plot_flux.py -i sim1.root sim2.root -o flux_plots.pdf
"""
from __future__ import annotations

import argparse
import os
from typing import Dict, List, Tuple

import numpy as np
import uproot
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Colors for different simulation files
COLORS = ["C0", "C1", "C2", "C3", "C4", "C5"]


def read_histogram(root_path: str, hist_name: str) -> Tuple[np.ndarray, np.ndarray]:
    """
    Read a 1D histogram from a ROOT file using uproot and return (values, edges).

    Returns:
        values: bin contents (length N)
        edges: bin edges (length N+1)
    Raises:
        KeyError if histogram not found.
    """
    with uproot.open(root_path) as f:
        obj = f[hist_name]
        # uproot's to_numpy returns (values, edges)
        values, edges = obj.to_numpy(flow=False)
    return values, edges


def plot_flux(
    input_files: List[str],
    output_pdf: str,
    x_range: Tuple[float, float] = (0.0, 3.0),
) -> None:
    """
    For each sector (0..5) and neutrino type (inu 1..4), plot the histogram
    h7{sector}{inu} from each input file on the same axes and save to a PDF.
    """
    neutrino_names = {1: "nue", 2: "nuebar", 3: "numu", 4: "numubar"}

    # Validate input files
    for p in input_files:
        if not os.path.isfile(p):
            raise FileNotFoundError(f"Input file not found: {p}")

    with PdfPages(output_pdf) as pdf:
        for inu in range(1, 5):
            for isec in range(0, 6):
                hist_name = f"h7{isec}{inu}"
                plt.figure(figsize=(6, 6))
                ax = plt.gca()
                plotted_any = False
                title_set = False

                for idx, infile in enumerate(input_files):
                    try:
                        values, edges = read_histogram(infile, hist_name)
                    except KeyError:
                        # Histogram not present in this file; skip it
                        continue

                    # Bin centers for plotting as a step plot
                    bin_centers = 0.5 * (edges[:-1] + edges[1:])
                    # Use step plotting to mimic ROOT histograms
                    ax.step(edges, np.append(values, values[-1]), where="post",
                            color=COLORS[idx % len(COLORS)], label=os.path.splitext(os.path.basename(infile))[0],
                            linewidth=1.8)
                    plotted_any = True

                    if not title_set:
                        # Use the histogram name as title; if the histogram object had a title,
                        # uproot does not always expose it consistently, so we keep it simple.
                        ax.set_title(f"{hist_name} ({neutrino_names.get(inu, 'unknown')}, sector {isec})")
                        title_set = True

                if not plotted_any:
                    plt.close()
                    # Skip pages where no input file contained the histogram
                    continue

                ax.set_xlim(*x_range)
                ax.set_xlabel("Energy [GeV]")
                ax.set_ylabel("Flux")
                ax.grid(True, linestyle=":", alpha=0.5)
                ax.legend(frameon=False, loc="best")
                plt.tight_layout()
                pdf.savefig()
                plt.close()


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Plot neutrino flux histograms from G4BNB ROOT files (using uproot).")
    p.add_argument(
        "-i",
        "--input",
        required=True,
        nargs="+",
        help="One or more simulation ROOT files to plot (histograms named h7{sector}{inu}).",
    )
    p.add_argument(
        "-o",
        "--output",
        default="flux_plots.pdf",
        help="Output multipage PDF filename (default: flux_plots.pdf).",
    )
    return p.parse_args()


def main() -> None:
    args = parse_args()
    try:
        plot_flux(args.input, args.output)
    except Exception as exc:
        print(f"Error: {exc}")
        raise


if __name__ == "__main__":
    main()
