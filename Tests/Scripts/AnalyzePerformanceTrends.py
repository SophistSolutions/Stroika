#!/usr/bin/env python3
"""Trend analysis across the whole checked-in performance archive, plus an optional current run.

    Tests/Scripts/AnalyzePerformanceTrends.py [CURRENT.txt]

(on Windows invoke via 'py' - plain 'python'/'python3' resolve to the Microsoft Store alias and fail.
CURRENT.txt is a PerformanceDump.txt, or the stdout of 'Test52 --show -x 15'.)

WHY THIS IS MORE TRUSTWORTHY THAN COMPARING TWO DUMPS: a PERFORMANCE_SCORE is comparison/baseline
measured in the same run on the same box, so it is self-normalising - the machine cancels out. What it
does NOT cancel is that different CPUs weigh different operations differently. The fix is agreement: a
change that shows up on Windows AND Linux AND MacOS is real; one that shows up on a single platform is a
machine artifact. So everything below aggregates ACROSS platforms and reports how many platforms agree.
"""
import re
import statistics
import sys
from collections import defaultdict
from pathlib import Path

# Tests/Scripts/X.py -> repo root two levels up, so this runs from any cwd
ROOT = Path (__file__).resolve ().parent.parent / 'HistoricalPerformanceRegressionTestResults'
RE_SCORE = re.compile (r'PERFORMANCE_SCORE\s+([\d.eE+-]+)')
RE_DETAILS = re.compile (r'baseline test\s+([\d.eE+-]+)\s+secs.*?comparison\s+([\d.eE+-]+)\s+sec'
                         r'(?:.*?warnIfPerfScore\s*>\s*([\d.eE+-]+))?(?:.*?perfScore=([\d.eE+-]+))?')
RE_FNAME = re.compile (r'PerformanceDump-(.+)-(\d+\.\d+(?:[a-z]\d+|(?:\.\d+)?))\.txt$')


def parse (path):
    out, cur = {}, None
    for line in Path (path).read_text (errors='replace').splitlines ():
        if line.startswith ('Test ') and not line.startswith ((' ', '\t')):
            cur = line[5:].strip ()
            out[cur] = {'score': None, 'warn': False, 'thresh': None}
        elif cur:
            s = line.strip ()
            if (m := RE_SCORE.search (s)):
                out[cur]['score'] = float (m.group (1))
            elif (m := RE_DETAILS.search (s)):
                if m.group (4):
                    out[cur]['score'] = float (m.group (4))
                if m.group (3):
                    out[cur]['thresh'] = float (m.group (3))
            elif 'WARNING - expected' in s:
                out[cur]['warn'] = True
    return {k: v for k, v in out.items () if v['score'] is not None}


def vkey (v):
    """sortable version key: 2.0a234 -> (2,0,0,234); 2.1.7 -> (2,1,1,7); 3.0d23 -> (3,0,2,23)"""
    m = re.match (r'(\d+)\.(\d+)(?:([a-z])(\d+)|\.(\d+))?$', v)
    if not m:
        return (99, 99, 99, 99)
    maj, mnr = int (m.group (1)), int (m.group (2))
    if m.group (3):
        return (maj, mnr, {'a': 0, 'b': 1, 'd': 2}.get (m.group (3), 3), int (m.group (4)))
    return (maj, mnr, 1, int (m.group (5) or 0))


def norm (t):
    # Key on the title with every non-alphanumeric character removed. Three reasons, all of which
    # otherwise split one test's history into several unrelated series:
    #   - the 2.1-era dumps space titles differently from the 3.0-era ones (tabs vs spaces)
    #   - some older dumps are not valid UTF-8, so decoding leaves replacement chars inside titles
    #   - the archive carries a long-lived typo, 'Charactes::String', fixed around 3.0d12-d16
    # NOTE a joined series is not automatically a COMPARABLE one - some workloads were rewritten at the
    # same time. See the +=wchar_t[] 100x test, whose BASELINE went 5.8s -> 93.9s at d16: its score
    # "improved" 66 -> 7.1 while Stroika's own side got slower. Treat 3.0d11 as a measurement boundary.
    return re.sub (r'[^A-Za-z0-9<>:_+=#\[\]]', '', t.replace ('Charactes::String', 'Characters::String'))


def main ():
    # ---------------------------------------------------------------- load everything
    # data[test][version][platform] = score
    data = defaultdict (lambda: defaultdict (dict))
    warns = defaultdict (lambda: defaultdict (dict))
    files = ok = 0
    versions_seen = set ()
    for f in sorted (ROOT.rglob ('PerformanceDump-*.txt')):
        files += 1
        m = RE_FNAME.search (f.name)
        if not m:
            continue
        plat, ver = m.group (1), m.group (2)
        tests = parse (f)
        if not tests:
            continue                      # the d18-d21 capture bug - known, ignored
        ok += 1
        versions_seen.add (ver)
        for t, rec in tests.items ():
            data[norm (t)][ver][plat] = rec['score']
            warns[norm (t)][ver][plat] = rec['warn']

    cur = {}
    if len (sys.argv) > 1:
        cur = {norm (k): v for k, v in parse (sys.argv[1]).items ()}
        for t, rec in cur.items ():
            data[t]['CURRENT']['thisbox'] = rec['score']
            warns[t]['CURRENT']['thisbox'] = rec['warn']

    vers = sorted (versions_seen, key=vkey)
    print (f'{ok} usable dumps of {files} ({files-ok} with no perf data - the d18-d21 capture bug, ignored)')
    print (f'versions {vers[0]} .. {vers[-1]}   ({len (vers)} releases, {len (data)} distinct tests)')
    if cur:
        print (f'plus CURRENT run ({len (cur)} tests)')

    def med (test, ver):
        vals = list (data[test].get (ver, {}).values ())
        return statistics.median (vals) if vals else None

    def nplat (test, ver):
        return len (data[test].get (ver, {}))

    # latest historical version that actually has this test
    def latest_hist (test):
        for v in reversed (vers):
            if data[test].get (v):
                return v
        return None

    # ---------------------------------------------------------------- 1. where Stroika stands NOW
    if cur:
        print (f'\n{"="*104}\n1. WHERE STROIKA STANDS vs THE STANDARD LIBRARY (current run; score = Stroika / stdlib)\n{"="*104}')
        rows = sorted (cur.items (), key=lambda kv: -kv[1]['score'])
        print (f'  {"SLOWER than stdlib":<62} {"score":>7} {"thresh":>7}')
        for k, v in rows:
            if v['score'] > 1.05:
                print (f'    {k[:60]:<60} {v["score"]:>7.2f} {str (v["thresh"]):>7}{"  WARN" if v["warn"] else ""}')
        print (f'  {"FASTER than stdlib":<62} {"score":>7}')
        for k, v in rows:
            if v['score'] <= 1.05:
                print (f'    {k[:60]:<60} {v["score"]:>7.2f}')

    # ---------------------------------------------------------------- 2. long-run trend, 2.1 -> now
    print (f'\n{"="*104}\n2. LONG-RUN TREND per test: median score across platforms, oldest -> newest\n{"="*104}')
    first_v = {}
    for t in data:
        for v in vers:
            if data[t].get (v):
                first_v[t] = v
                break
    trend = []
    for t in data:
        fv = first_v.get (t)
        lv = 'CURRENT' if (cur and t in cur) else latest_hist (t)
        if not fv or not lv or fv == lv:
            continue
        a, b = med (t, fv), med (t, lv)
        if not a or not b:
            continue
        trend.append ((b / a, t, fv, a, lv, b, nplat (t, fv), nplat (t, lv)))
    trend.sort (key=lambda r: r[0])
    print (f'  {"test":<52} {"from":>8} {"score":>7}  {"to":>8} {"score":>7}  {"x":>6}  platforms')
    for r in trend:
        ratio, t, fv, a, lv, b, na, nb = r
        arrow = 'BETTER' if ratio < 0.9 else ('WORSE' if ratio > 1.1 else 'flat')
        print (f'  {t[:52]:<52} {fv:>8} {a:>7.3f}  {lv:>8} {b:>7.3f}  {ratio:>5.2f}x  {na}->{nb}  {arrow}')

    # ---------------------------------------------------------------- 3. noise: cross-platform spread
    print (f'\n{"="*104}\n3. HOW NOISY IS EACH TEST? (spread of the same test across platforms, within a version)\n{"="*104}')
    noise = []
    for t in data:
        cvs = []
        for v in vers:
            vals = [x for x in data[t].get (v, {}).values () if x]
            if len (vals) >= 4:
                m = statistics.median (vals)
                if m:
                    cvs.append ((max (vals) - min (vals)) / m)
        if cvs:
            noise.append ((statistics.median (cvs), t, len (cvs)))
    noise.sort (reverse=True)
    print (f'  {"test":<62} {"typical spread":>15}')
    for sp, t, n in noise[:12]:
        print (f'  {t[:62]:<62} {sp*100:>13.0f}%   (over {n} releases)')
    print (f'  ... median test spread across the suite: {statistics.median ([n[0] for n in noise])*100:.0f}%')

    # ---------------------------------------------------------------- 4. threshold calibration
    print (f'\n{"="*104}\n4. WARNING CALIBRATION (a threshold that fires almost always is not a regression detector)\n{"="*104}')
    cal = []
    for t in data:
        tot = fired = 0
        for v in vers:
            for p, w in warns[t].get (v, {}).items ():
                tot += 1
                fired += bool (w)
        if tot >= 20:
            cal.append ((fired / tot, t, tot, fired))
    cal.sort (reverse=True)
    print (f'  {"test":<62} {"fires":>8}  {"of":>5}')
    for frac, t, tot, fired in cal:
        if frac > 0.30:
            nowtxt = ''
            if cur and t in cur:
                nowtxt = '  [WARNS NOW]' if cur[t]['warn'] else '  [quiet now]'
            print (f'  {t[:62]:<62} {frac*100:>7.0f}% {tot:>5}{nowtxt}')
    print (f'  suite-wide: {sum (r[3] for r in cal)} warnings fired out of {sum (r[2] for r in cal)} test-runs '
           f'({sum (r[3] for r in cal)/max (1,sum (r[2] for r in cal))*100:.0f}%)')


if __name__ == '__main__':
    main ()
