#!/usr/bin/env python3
"""Build the PotentialSuite manual: MANUAL.md (for GitHub) and PotentialSuite-Manual.pdf, from manual/src.md.

    python3 manual/build.py --plugins <folder holding the ten plugin repos>

The plugin repos (potentialpunch or mpc-vst-potentialpunch, and so on) supply the facts that are generated: versions,
parameters, chain preset values, tested devices and CPU figures. Needs: pip install markdown pypdf playwright.
The PDF is typeset in Chromium (HTML + style.css), in two passes so the contents page has page numbers.
"""
import argparse, html, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
EDITION = "Edition 1 · November 2026"
ORDER = ["breaks", "wax", "crates", "kits", "noise", "patina", "punch", "heat", "grit", "bleed", "warp"]
KIND = {"breaks": "Instrument", "wax": "Instrument", "crates": "Instrument", "kits": "Instrument", "noise": "Instrument"}
CHAINS = ["Dusty Loop", "90s Stab", "Break Bus", "Dub Drums"]
# PotentialPunch keeps its presets in src/punch.c: attack, sustain, speed, focus, clip, mix, output
PUNCH_CHAINS = {"90s Stab": (30, 0, "TIGHT", "FULL", "Off", 80, 0), "Break Bus": (50, 15, "MEDIUM", "FULL", "On", 100, -1),
                "Dub Drums": (60, -25, "TIGHT", "LOWS", "Off", 100, -1)}
# measured before a plugin's CHANGELOG recorded figures
CPU_EXTRA = {"bleed": "about 4 % (1.0.0)", "grit": "13.1 % (1.0.0). 1.0.1 made it lighter and has not been measured again",
             "crates": "not measured (it streams audio rather than processing it)", "kits": "not measured yet"}


def repo(plugins, p):
    for d in ("potential" + p, "mpc-vst-potential" + p):
        if os.path.isdir(os.path.join(plugins, d, "vst")):
            return os.path.join(plugins, d)
    sys.exit("plugin repo not found: potential%s under %s" % (p, plugins))


def table(head, rows):
    out = ["| " + " | ".join(head) + " |", "|" + "---|" * len(head)]
    out += ["| " + " | ".join(str(c).replace("|", "\\|") for c in r) + " |" for r in rows]
    return "\n".join(out)


def ver(n):
    return "%d.%d.%d" % (n // 1000, n % 1000 // 100, n % 100)


def versions(plugins):
    rows = []
    for p in ORDER:
        v = json.load(open(repo(plugins, p) + "/vst/vst.json"))
        rows.append(("**%s**" % v["name"], ver(v["version"]), KIND.get(p, "Insert effect")))
    rows.append(("**MPC TOOLS**", "1.5.9", "Companion app (Windows, Mac)"))
    return table(["Plugin", "Version", "Type"], rows)


def tested(plugins):
    rows = []
    for p in ORDER:
        r = repo(plugins, p)
        name = json.load(open(r + "/vst/vst.json"))["name"]
        try:
            t = json.load(open(r + "/tested.json"))
        except (OSError, ValueError):
            t = []
        seen = {}
        for x in t:
            fw = re.sub(r"\s*\(.*\)$", "", x.get("firmware", ""))
            seen.setdefault(x.get("device", "?"), set()).add(fw)
        cell = "; ".join("%s (%s)" % (d, ", ".join(sorted(f))) for d, f in sorted(seen.items())) or "MPC One (run by the author; not yet recorded)"
        rows.append(("**%s**" % name, cell))
    return table(["Plugin", "Tested on"], rows)


def cpu(plugins):
    """the newest MPC TOOLS bench result (vst/bench.txt), else the figure the CHANGELOG recorded"""
    rows = []
    for p in ORDER:
        r = repo(plugins, p)
        name = json.load(open(r + "/vst/vst.json"))["name"]
        try:
            lines = [l for l in open(r + "/vst/bench.txt") if l.strip()]
            v = re.search(r"([0-9]+\.[0-9]+\.[0-9]+)\s*$", lines[0]).group(1)
            b = json.loads(lines[-1])
            rows.append(("**%s** %s" % (name, v), "%s %%" % b["p99_pct"], "%s %%" % b["max_pct"]))
            continue
        except (OSError, ValueError, IndexError, AttributeError, KeyError):
            pass
        m = re.search(r"PASS\*\*, ([0-9.]+)% of an audio block at the 99th percentile and ([0-9.]+)% at worst", open(r + "/CHANGELOG.md").read())
        rows.append(("**%s**" % name, "%s %%" % m.group(1), "%s %%" % m.group(2)) if m else ("**%s**" % name, CPU_EXTRA.get(p, "not measured"), ""))
    return table(["Plugin and version", "99th percentile", "Worst block"], rows)


def chain_values(plugins):
    out = []
    for p in ("patina", "grit", "bleed"):
        r = repo(plugins, p)
        name = json.load(open(r + "/vst/vst.json"))["name"]
        ps = {x["name"]: x["values"] for x in json.load(open(r + "/vst/presets.json"))["presets"]}
        keys = []
        for c in CHAINS:
            keys += [k for k in ps[c] if k not in keys]
        out.append("**%s** (`vst/presets.json`)\n\n" % name + table(["Parameter"] + CHAINS, [["`%s`" % k] + [ps[c].get(k, "") for c in CHAINS] for k in keys]))
    keys = ["attack", "sustain", "speed", "focus", "clip", "mix", "output"]
    out.append("**PotentialPunch** (the preset table in `src/punch.c`)\n\n" + table(
        ["Parameter"] + CHAINS, [["`%s`" % k] + [PUNCH_CHAINS[c][i] if c in PUNCH_CHAINS else "" for c in CHAINS] for i, k in enumerate(keys)]))
    return "\n\n".join(out)


def param_rows(params):
    """(index, key, name, range, default, kind) rows; runs like res_1 .. res_8 become one row."""
    rows, i = [], 0
    while i < len(params):
        x = params[i]
        m = re.fullmatch(r"(.+?_?)(\d+)", x["key"])
        j = i
        if m:   # a run of numbered keys with the same shape (list rows, waveform columns)
            shape = lambda y: {k: v for k, v in y.items() if k not in ("key", "name")}
            nxt = lambda n: "%s%0*d" % (m.group(1), len(m.group(2)), int(m.group(2)) + n)
            while j + 1 < len(params) and params[j + 1]["key"] == nxt(j + 1 - i) and shape(params[j + 1]) == shape(x):
                j += 1
            if j - i < 3:
                j = i
        kind = x.get("type", "")
        if x.get("display") == "string" and kind in ("", "readout"):
            kind = "text"
        if x.get("legacy"):
            kind = (kind + " legacy").strip()
        if "options" in x:
            o = x["options"]
            rng = ", ".join(str(v) for v in o) if len(o) <= 8 else "%s … %s (%d options)" % (o[0], o[-1], len(o))
            d = x.get("default", 0)
            dflt = o[d] if isinstance(d, int) and 0 <= d < len(o) else d
        elif "min" in x:
            rng = "%g to %g%s" % (x["min"], x["max"], (" " + x["unit"]) if x.get("unit") else "")
            dflt = x.get("default", "")
        else:
            rng, dflt = "", ""
        if j > i:
            rows.append(("%d–%d" % (i, j), "`%s` … `%s`" % (x["key"], params[j]["key"]), "%s … %s" % (x["name"], params[j]["name"]), rng, dflt, kind))
        else:
            rows.append((i, "`%s`" % x["key"], x["name"], rng, dflt, kind))
        i = j + 1
    return rows


def all_params(plugins):
    out = []
    for p in ORDER:
        r = repo(plugins, p)
        name = json.load(open(r + "/vst/vst.json"))["name"]
        ps = json.load(open(r + "/vst/params.json"))["params"]
        out.append("## %s\n\n%d parameters.\n\n%s" % (name, len(ps), table(["#", "Key", "Name", "Range or options", "Default", "Kind"], param_rows(ps))))
    return "\n\n".join(out)


def slug(t):   # GitHub's heading anchor
    return re.sub(r"\s", "-", re.sub(r"[^\w\s-]", "", t.lower()).strip())


def headings(md):
    """(level, text) of the # and ## headings outside code blocks"""
    out, code = [], False
    for l in md.split("\n"):
        if l.startswith("```"):
            code = not code
        m = None if code else re.match(r"(#{1,2}) (.+)", l)
        if m:
            out.append((len(m.group(1)), m.group(2).strip()))
    return out[1:]   # not the title


def expand(src, plugins):
    for k, fn in (("VERSIONS", versions), ("TESTED", tested), ("CPU", cpu), ("CHAIN_VALUES", chain_values), ("ALL_PARAMS", all_params)):
        src = src.replace("{{%s}}" % k, fn(plugins))
    return src.replace("{{EDITION}}", "*%s*" % EDITION)


def github(md):
    hs = [h for h in headings(md) if h[1] != "About this manual"]
    toc = "## Contents\n\n" + "\n".join("%s- [%s](#%s)" % ("" if lv == 1 else "  ", t, slug(t)) for lv, t in hs)
    note = "\n\n> A typeset copy of this manual: [PotentialSuite-Manual.pdf](PotentialSuite-Manual.pdf)."
    return md.replace("{{TOC}}", toc + note)


# ---- the PDF ----

ACCENT = {"PotentialBleed": "4f6d8a", "PotentialBreaks": "c77800", "PotentialCrates": "4f8a1e", "PotentialGrit": "8a6a3c",
          "PotentialHeat": "e85d10", "PotentialKits": "a87c00", "PotentialNoise": "d94f8f", "PotentialPatina": "1f8f80", "PotentialPunch": "2d5be3",
          "PotentialWarp": "6b3fd4", "PotentialWax": "d7263d"}


def to_html(md, pages=None):
    import markdown
    hs = [h for h in headings(md) if h[1] != "About this manual"]
    rows = "".join('<a class="toc%d" href="#%s"><span>%s</span><i></i><b>%s</b></a>' % (lv, slug(t), html.escape(t), (pages or {}).get(t, "00"))
                   for lv, t in hs)
    md = md.replace("{{TOC}}", '<nav class="toc" markdown="0"><h2 class="toctitle">Contents</h2>%s</nav>' % rows)
    md = re.sub(r"\A# .+\n\n\*.+\*\n\n\*.+\*\n", "", md)   # the title block: the cover has it
    body = markdown.markdown(md, extensions=["tables", "fenced_code", "sane_lists", "md_in_html"])
    ids = {}

    def head(m):
        lv, text = m.group(1), re.sub(r"<[^>]+>", "", m.group(2))
        s = slug(html.unescape(text))
        ids[s] = ids.get(s, 0) + 1
        s = s if ids[s] == 1 else "%s-%d" % (s, ids[s] - 1)
        cls, style = "", ""
        if lv == "1":
            cls = ' class="part"'
        for name, c in ACCENT.items():
            if lv in ("2", "3") and name in text:
                cls, style = ' class="plugin"', ' style="--c:#%s"' % c
        return '<h%s id="%s"%s%s>%s</h%s>' % (lv, s, cls, style, m.group(2), lv)
    body = re.sub(r"<h([1-4])>(.*?)</h\1>", head, body)

    def chain(m):
        parts = [x.strip() for x in m.group(1).split("→")]
        chips = '<span class="arrow">→</span>'.join(
            '<span class="chip" style="--c:#%s">%s</span>' % (ACCENT.get("Potential" + x, "1b1a18"), x) for x in parts)
        return '<span class="chain">%s</span>' % chips
    body = re.sub(r"<strong>([A-Za-z ]+(?: → [A-Za-z ]+)+)</strong>", chain, body)
    # a short table stays on one page
    body = re.sub(r"<table>(.*?)</table>", lambda m: ('<table class="keep">' if m.group(1).count("<tr>") <= 12 else "<table>") + m.group(1) + "</table>", body, flags=re.S)
    body = body.replace('src="manual/', 'src="').replace('src="docs/', 'src="../docs/')
    cover = ('<section class="cover"><div class="marks"></div><p class="kicker">Akai MPC standalone · eleven free plugins</p>'
             '<h1>Potential<br>Suite</h1><p class="sub">The Manual</p>'
             '<p class="lead">User guide, FX chains and load orders, and a developer reference</p>'
             '<div class="swatches">%s</div><p class="ed">%s · PotentialBreaks</p></section>'
             % ("".join('<span style="background:#%s"></span>' % ACCENT["Potential" + p.capitalize()] for p in ORDER), EDITION))
    css = open(os.path.join(HERE, "style.css")).read()
    return ('<!DOCTYPE html><html lang="en"><head><meta charset="utf-8"><title>PotentialSuite Manual</title><style>%s</style></head>'
            '<body>%s<main>%s</main></body></html>' % (css, cover, body))


def pdf(htmlfile, out):
    """the cover (no footer) and the body (page numbers from 1) are printed apart and joined"""
    from playwright.sync_api import sync_playwright
    foot = ('<div style="width:100%;font-family:Jost,Helvetica,Arial,sans-serif;font-size:8px;color:#6b665c;padding:0 16mm;display:flex;'
            'justify-content:space-between"><span>PotentialSuite Manual</span><span class="pageNumber"></span></div>')
    with sync_playwright() as p:
        b = p.chromium.launch()
        pg = b.new_page()
        pg.goto("file://" + htmlfile)
        pg.evaluate("document.fonts.ready")
        pg.wait_for_timeout(300)
        pg.pdf(path=out + ".cover", format="A4", print_background=True, page_ranges="1", prefer_css_page_size=True)
        pg.evaluate("document.querySelector('.cover').remove()")
        pg.pdf(path=out + ".body", format="A4", print_background=True, display_header_footer=True, header_template="<span></span>",
               footer_template=foot, prefer_css_page_size=True)
        b.close()
    from pypdf import PdfReader, PdfWriter
    w = PdfWriter()
    for part in (".cover", ".body"):
        w.append(out + part)
    w.add_metadata({"/Title": "PotentialSuite Manual", "/Author": "PotentialBreaks", "/Subject": "User guide and developer reference for the PotentialSuite MPC plugins"})
    w.write(out)
    for part in (".cover", ".body"):
        os.remove(out + part)


def page_numbers(pdffile, md):
    """the printed page number of each # and ## heading (the body's own text, after the contents pages)"""
    from pypdf import PdfReader
    texts = [re.sub(r"\s+", " ", p.extract_text() or "") for p in PdfReader(pdffile).pages]
    hs = [t for _, t in headings(md) if t != "About this manual"]
    start = next(i for i, t in enumerate(texts) if "Part 1: User guide" in t and "Contents" not in t and len(t) < 400)
    out, at = {}, start
    for t in hs:
        want = re.sub(r"\s+", " ", t)
        for i in range(at, len(texts)):
            if want in texts[i]:
                out[t], at = i, i          # page 1 is the cover, which has no number
                break
        else:
            sys.exit("contents: heading not found in the PDF: %s" % t)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--plugins", required=True)
    a = ap.parse_args()
    md = expand(open(os.path.join(HERE, "src.md"), encoding="utf-8").read(), os.path.abspath(a.plugins))
    open(os.path.join(ROOT, "MANUAL.md"), "w", encoding="utf-8").write(github(md))
    tmp, out = os.path.join(HERE, "_manual.html"), os.path.join(ROOT, "PotentialSuite-Manual.pdf")
    open(tmp, "w", encoding="utf-8").write(to_html(md))
    pdf(tmp, out)
    pages = page_numbers(out, md)
    open(tmp, "w", encoding="utf-8").write(to_html(md, pages))
    pdf(tmp, out)
    if page_numbers(out, md) != pages:
        sys.exit("contents: the page numbers moved between passes")
    os.remove(tmp)
    from pypdf import PdfReader
    print("MANUAL.md and %s (%d pages)" % (os.path.basename(out), len(PdfReader(out).pages)))


if __name__ == "__main__":
    main()
