#!/usr/bin/env python3

"""
 Inspects properties of h7-- plots from output histogram root file. 

 Usage:
     (see old inputs)
"""

import sys
import uproot

def get_member(obj, name):
    try:
        return obj.member(name)
    except Exception:
        return None

def axis_title(axis_obj):
    if axis_obj is None:
        return None
    # try common member names
    for m in ("fTitle", "fName", "fLabel", "fTitleString"):
        try:
            val = axis_obj.member(m)
            if val:
                return val
        except Exception:
            pass
    # fallback: try to read name attribute
    try:
        return getattr(axis_obj, "name", None)
    except Exception:
        return None

if len(sys.argv) != 2:
    print("Usage: python3 inspect_h7.py file.root")
    sys.exit(1)

fname = sys.argv[1]
with uproot.open(fname) as f:
    for key in f.keys():
        name = key.decode() if isinstance(key, (bytes, bytearray)) else key
        if not name.startswith("h7"):
            continue
        try:
            h = f[name]
        except Exception as e:
            print(f"{name}: cannot open ({e})")
            continue

        # try several ways to get a title
        title = None
        for m in ("fTitle", "title", "name"):
            try:
                title = h.member(m)
                if title:
                    break
            except Exception:
                title = None

        # try to get x/y axis titles
        xaxis = None
        yaxis = None
        try:
            xaxis = h.member("fXaxis")
            yaxis = h.member("fYaxis")
        except Exception:
            pass

        xt = axis_title(xaxis)
        yt = axis_title(yaxis)

        # fallback: try to read bin edges to show range
        try:
            values, edges = h.to_numpy(flow=False)
            nbins = len(values)
            xmin, xmax = edges[0], edges[-1]
        except Exception:
            nbins = "?"
            xmin = xmax = "?"

        print(f"{name}: title={title!r}, xaxis={xt!r}, yaxis={yt!r}, nbins={nbins}, x-range=({xmin}, {xmax})")
