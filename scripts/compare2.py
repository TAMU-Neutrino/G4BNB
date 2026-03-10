#!/usr/bin/env python3
"""
uproot-based replacement for compare.py
Usage:
  python compare_uproot.py -d validation/april07_baseline_rgen610.6_fixrnd_20171212.root \
                           -c hist_sbnd.root -o sbnd_plot.pdf
"""
import argparse
import os
import numpy as np
import uproot
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import mplhep as hep

# Colors roughly matching ROOT kRed, kBlue, kMagenta, kGreen+2
COLORS = ["C0", "C1", "C3", "C2"]

def getLevelOfAgreement_from_arrays(edges, counts_ref, counts_cmp):
    """
    Reproduce getLevelOfAgreement logic:
    - low: bin low edge < 0.2
    - mid: 0.2 <= bin low edge < 2.0
    - high: 2.0 <= bin low edge < 3.0
    Returns [low_ratio, mid_ratio, high_ratio] = cmp/ref
    """
    low_ref = low_cmp = mid_ref = mid_cmp = high_ref = high_cmp = 0.0
    for i in range(len(counts_ref)):
        lowedge = edges[i]
        if lowedge < 0.2:
            low_ref += counts_ref[i]
            low_cmp += counts_cmp[i]
        elif lowedge < 2.0:
            mid_ref += counts_ref[i]
            mid_cmp += counts_cmp[i]
        elif lowedge < 3.0:
            high_ref += counts_ref[i]
            high_cmp += counts_cmp[i]
    # avoid division by zero, follow original script behavior (set denom to 1 if zero)
    if low_ref == 0:
        low_ref = 1.0
    if mid_ref == 0:
        mid_ref = 1.0
    if high_ref == 0:
        high_ref = 1.0
    return [low_cmp / low_ref, mid_cmp / mid_ref, high_cmp / high_ref]

def read_hist_numpy(rootfile, histname):
    """
    Return (counts, edges, title) for a TH1-like object.
    Uses flow=True to include under/overflow if present; adjust if you prefer otherwise.
    """
    obj = rootfile.get(histname)
    if obj is None:
        return None
    # uproot returns TH1 objects with to_numpy()
    counts, edges = obj.to_numpy(flow=False)  # flow=True if you want under/overflow
    title = obj.member("fTitle") if hasattr(obj, "member") else getattr(obj, "title", "")
    # fallback: try reading title attribute if present
    try:
        title = obj.member("fTitle")
    except Exception:
        try:
            title = obj.title
        except Exception:
            title = ""
    return counts, edges, title

def safe_divide(a, b):
    """Elementwise divide with safe handling of zeros in denominator."""
    a = np.asarray(a, dtype=float)
    b = np.asarray(b, dtype=float)
    out = np.ones_like(a)
    mask = (b != 0)
    out[mask] = a[mask] / b[mask]
    out[~mask] = 0.0
    return out

def main():
    parser = argparse.ArgumentParser(description="Compare ROOT histograms using uproot + matplotlib")
    parser.add_argument("-d", "--data", required=True, help="Histogram file to compare to (plotted with dots).")
    parser.add_argument("-c", "--compare", required=True, nargs='+', help="List histogram file(s) being compared to data.")
    parser.add_argument("-o", "--output", default="plots.pdf", help="Output pdf file with all the plots.")
    args = parser.parse_args()

    # sanity checks
    for ff in [args.data] + args.compare:
        if not os.path.isfile(ff):
            raise SystemExit(f"{ff} does not exist.")

    # open files with uproot
    fdata = uproot.open(args.data)
    fmc = {ff: uproot.open(ff) for ff in args.compare}

    # store ratios for printing at the end
    rat = {inu: {} for inu in range(1, 5)}

    # iterate same loops as original script
    for inu in range(1, 5):
        for isec in range(0, 6):
            # determine neutrino type (not strictly needed, kept for parity)
            ntype = "numu"
            if inu == 1:
                ntype = "nue"
            elif inu == 2:
                ntype = "nuebar"
            elif inu == 4:
                ntype = "numubar"

            histname = f"h7{isec}{inu}"
            # read data histogram
            data_tuple = read_hist_numpy(fdata, histname)
            if data_tuple is None:
                print(f"Warning: {histname} not found in data file.")
                continue
            counts_data, edges_data, title = data_tuple
            centers = 0.5 * (edges_data[:-1] + edges_data[1:])
            # prepare figure with two pads (top: main, bottom: ratio)
            plt.style.use(hep.style.CMS)  # optional; remove if you don't want HEP style
            fig = plt.figure(figsize=(6, 7))
            gs = fig.add_gridspec(2, 1, height_ratios=[2, 1], hspace=0.05)
            ax_main = fig.add_subplot(gs[0])
            ax_ratio = fig.add_subplot(gs[1], sharex=ax_main)

            # main: data as points with errorbars (sqrt(N) errors)
            data_err = np.sqrt(counts_data)
            ax_main.errorbar(centers, counts_data, yerr=data_err, fmt='o', color='k', label=os.path.splitext(os.path.basename(args.data))[0])
            ax_main.set_xlim(0, 3)
            # set y label
            ax_main.set_ylabel("Flux")
            # track maximum for y-limits
            ymax = counts_data.max() if counts_data.size else 1.0

            # draw MC histograms as step lines and compute ratios
            icol = 0
            draw_labels = []
            for ff in args.compare:
                mc_tuple = read_hist_numpy(fmc[ff], histname)
                if mc_tuple is None:
                    print(f"Warning: {histname} not found in {ff}.")
                    continue
                counts_mc, edges_mc, _ = mc_tuple
                # ensure edges match; if not, rebin or interpolate (here we assume same binning)
                if not np.allclose(edges_mc, edges_data):
                    # simple fallback: rebin/interpolate mc counts to data edges
                    # compute bin centers for mc and interpolate counts to data centers
                    centers_mc = 0.5 * (edges_mc[:-1] + edges_mc[1:])
                    counts_mc = np.interp(centers, centers_mc, counts_mc, left=0, right=0)
                # scaling placeholder (original script scaled by 500/500)
                # counts_mc = counts_mc * (500.0 / 500.0)

                # step plotting: use edges and append last value for step plotting
                ax_main.step(edges_data, np.append(counts_mc, counts_mc[-1]), where='post',
                             color=COLORS[icol % len(COLORS)], linestyle='-' if icol < len(COLORS) else '--',
                             linewidth=1.5, label=os.path.splitext(os.path.basename(ff))[0])
                ymax = max(ymax, counts_mc.max() if counts_mc.size else ymax)

                # ratio
                ratio = safe_divide(counts_mc, counts_data)
                ax_ratio.step(edges_data, np.append(ratio, ratio[-1]), where='post',
                              color=COLORS[icol % len(COLORS)], linewidth=1.5)
                # set ratio y-range as in original script
                ax_ratio.set_ylim(0.4, 1.6)
                ax_ratio.set_ylabel("Ratio")
                ax_ratio.set_xlabel("Variable")
                ax_ratio.set_xlim(0, 3)

                # compute level of agreement for isec==0 (original script only computed when isec==0)
                if isec == 0:
                    rat_val = getLevelOfAgreement_from_arrays(edges_data[:-1], counts_data, counts_mc)
                    rat[inu][ff] = rat_val

                icol += 1

            # finalize main pad
            ax_main.set_ylim(0, ymax * 1.1)
            ax_main.legend(loc='upper right', frameon=False)
            # remove x tick labels on main
            plt.setp(ax_main.get_xticklabels(), visible=False)

            # draw horizontal line at 1 on ratio pad
            ax_ratio.axhline(1.0, color='gray', linestyle='--', linewidth=1)

            # save page to PDF
            with PdfPages(args.output) as pdf:
                # To mimic the original script's multi-page behavior, we append pages.
                # But PdfPages context manager overwrites if opened each time; instead we will
                # collect pages and write once. To keep code simple, we will open once outside loop.
                pass

            # Instead of opening PdfPages per histogram, we'll collect figures and write later.
            # To do that, we return fig and let outer code handle PdfPages.
            # But to keep parity with original script (printing pages sequentially), we will
            # append to a global list.
            if 'figures' not in globals():
                globals()['figures'] = []
            globals()['figures'].append(fig)

    # write all figures to a single multipage PDF
    if 'figures' in globals() and globals()['figures']:
        with PdfPages(args.output) as pdf:
            for fig in globals()['figures']:
                pdf.savefig(fig)
                plt.close(fig)

    # print the level-of-agreement table similar to original script
    for inu in range(1, 5):
        for ff in args.compare:
            if ff in rat.get(inu, {}):
                low, mid, high = rat[inu][ff]
                print(f"{ff:40s} {low:5.2f} {mid:5.2f} {high:5.2f}")
            else:
                print(f"{ff:40s} {'N/A':>5} {'N/A':>5} {'N/A':>5}")

if __name__ == "__main__":
    main()
