#!/usr/bin/env python3
"""
List h7{sector}{inu} histograms in a ROOT file using uproot.
Prints histogram name, number of bins, and x-range (if readable).
"""
import sys
import argparse
import uproot

def main():
    p = argparse.ArgumentParser(description="List h7 histograms")
    p.add_argument("rootfile", help="ROOT file to inspect")
    args = p.parse_args()

    with uproot.open(args.rootfile) as f:
        for key in f.keys():
            # handle both bytes and str keys
            name = key.decode() if isinstance(key, (bytes, bytearray)) else key
            if not name.startswith("h7"):
                continue
            try:
                h = f[name]
                # try to read bin contents and edges
                values, edges = h.to_numpy(flow=False)
                nbins = len(values)
                xmin, xmax = edges[0], edges[-1]
                print(f"{name}: nbins={nbins}, x-range=({xmin:.6g}, {xmax:.6g})")
            except Exception as e:
                print(f"{name}: found but could not read histogram ({e})")

if __name__ == "__main__":
    main()
