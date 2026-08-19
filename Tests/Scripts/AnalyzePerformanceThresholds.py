#!/usr/bin/env python3
"""Health of the perf suite itself: threshold calibration, warning drift, and the real noise floor.

    Tests/Scripts/AnalyzePerformanceThresholds.py [CURRENT.txt]

(on Windows invoke via 'py' - plain 'python'/'python3' resolve to the Microsoft Store alias and fail.)

Three questions the raw dumps do not answer directly:
  1. Can each test's threshold still fire? A warnIfPerfScore far above the score the test actually
     produces is a dead detector - it would take a huge regression to trip.
  2. Is the warning rate drifting over releases? Rising rate = thresholds calibrated for an older Stroika.
  3. What is the true run-to-run noise floor? Cross-PLATFORM spread conflates real platform differences
     with noise, so measure consecutive versions on ONE platform instead.
"""
import re
import statistics
import sys
from collections import defaultdict
from pathlib import Path

# Tests/Scripts/X.py -> repo root two levels up, so this runs from any cwd
ROOT = Path (__file__).resolve ().parent.parent / 'HistoricalPerformanceRegressionTestResults'
RE_FNAME = re.compile (r'PerformanceDump-(.+)-(\d+\.\d+(?:[a-z]\d+|(?:\.\d+)?))\.txt$')
RE_SCORE = re.compile (r'PERFORMANCE_SCORE\s+([\d.eE+-]+)')
RE_DET = re.compile (r'baseline test\s+([\d.eE+-]+)\s+secs.*?comparison\s+([\d.eE+-]+)\s+sec'
                     r'(?:.*?warnIfPerfScore\s*>\s*([\d.eE+-]+))?(?:.*?perfScore=([\d.eE+-]+))?')
REF_PLATFORM = 'Windows_MSYS_VS2k22-x86_64'   # the one this box matches, and the longest run


def parse (p):
    out, cur = {}, None
    for line in Path (p).read_text (errors='replace').splitlines ():
        if line.startswith ('Test ') and not line.startswith ((' ', '\t')):
            cur = line[5:].strip ()
            out[cur] = {'score': None, 'thresh': None, 'warn': False, 'base': None, 'comp': None}
        elif cur:
            s = line.strip ()
            if (m := RE_SCORE.search (s)):
                out[cur]['score'] = float (m.group (1))
            elif (m := RE_DET.search (s)):
                out[cur]['base'] = float (m.group (1))
                out[cur]['comp'] = float (m.group (2))
                if m.group (3):
                    out[cur]['thresh'] = float (m.group (3))
                if m.group (4):
                    out[cur]['score'] = float (m.group (4))
            elif 'WARNING - expected' in s:
                out[cur]['warn'] = True
    return {k: v for k, v in out.items () if v['score'] is not None}


def vkey (v):
    m = re.match (r'(\d+)\.(\d+)(?:([a-z])(\d+)|\.(\d+))?$', v)
    if not m:
        return (99, 99, 99, 99)
    if m.group (3):
        return (int (m.group (1)), int (m.group (2)), {'a': 0, 'b': 1, 'd': 2}.get (m.group (3), 3), int (m.group (4)))
    return (int (m.group (1)), int (m.group (2)), 1, int (m.group (5) or 0))


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
    per_ver_warn = defaultdict (lambda: [0, 0])        # ver -> [fired, total]
    scores = defaultdict (list)                        # test -> all scores ever
    thresh_now = {}                                    # test -> most recent threshold seen
    ref = defaultdict (dict)                           # test -> ver -> score, REF_PLATFORM only
    for f in sorted (ROOT.rglob ('PerformanceDump-*.txt')):
        m = RE_FNAME.search (f.name)
        if not m:
            continue
        plat, ver = m.group (1), m.group (2)
        tests = parse (f)
        if not tests:
            continue
        for t, r in tests.items ():
            t = norm (t)
            per_ver_warn[ver][1] += 1
            per_ver_warn[ver][0] += bool (r['warn'])
            scores[t].append (r['score'])
            if r['thresh']:
                thresh_now[t] = r['thresh']
            if plat == REF_PLATFORM:
                ref[t][ver] = r['score']

    cur = {}
    if len (sys.argv) > 1:
        cur = {norm (k): v for k, v in parse (sys.argv[1]).items ()}

    # ------------------------------------------------------------- 1. threshold health
    print ('=' * 100)
    print ('1. THRESHOLD HEALTH  (can the test still detect a regression?)')
    print ('=' * 100)
    print (f'  {"test":<56} {"typ score":>9} {"thresh":>8} {"headroom":>9}  state')
    rows = []
    for t, ss in scores.items ():
        th = cur.get (t, {}).get ('thresh') or thresh_now.get (t)
        if not th:
            continue
        typ = statistics.median (ss)
        if typ <= 0:
            continue
        rows.append ((th / typ, t, typ, th, len (ss)))
    rows.sort (reverse=True)
    dead = hair = 0
    for head, t, typ, th, n in rows:
        if head >= 3.0:
            state, dead = 'DEAD - needs a %.0fx regression to fire' % head, dead + 1
        elif head < 1.0:
            state, hair = 'HAIR-TRIGGER - fires at the typical value', hair + 1
        else:
            state = 'ok'
        if head >= 3.0 or head < 1.15:
            print (f'  {t[:56]:<56} {typ:>9.2f} {th:>8.2f} {head:>8.1f}x  {state}')
    print (f'  -> {dead} dead thresholds, {hair} hair-trigger, {len (rows)-dead-hair} usable (of {len (rows)} with a threshold)')

    # ------------------------------------------------------------- 2. warning drift
    print (f'\n{"="*100}\n2. WARNING RATE BY RELEASE  (rising = thresholds calibrated for an older Stroika)\n{"="*100}')
    vers = sorted (per_ver_warn, key=vkey)
    for v in vers:
        fired, tot = per_ver_warn[v]
        bar = '#' * int (round (fired / tot * 50)) if tot else ''
        print (f'  {v:>9} {fired:>4}/{tot:<5} {fired/tot*100:>5.1f}%  {bar}')
    if cur:
        fired = sum (1 for r in cur.values () if r['warn'])
        print (f'  {"CURRENT":>9} {fired:>4}/{len (cur):<5} {fired/len (cur)*100:>5.1f}%  {"#"*int (round (fired/len (cur)*50))}')

    # ------------------------------------------------------------- 3. true noise floor
    print (f'\n{"="*100}\n3. RUN-TO-RUN NOISE FLOOR on ONE platform ({REF_PLATFORM})\n{"="*100}')
    print ('  consecutive-release |change| per test - this is the size of move that means NOTHING')
    floor = []
    for t, byv in ref.items ():
        vs = sorted (byv, key=vkey)
        deltas = []
        for a, b in zip (vs, vs[1:]):
            if byv[a] and byv[b]:
                deltas.append (abs (byv[b] - byv[a]) / byv[a])
        if len (deltas) >= 4:
            floor.append ((statistics.median (deltas), max (deltas), t, len (deltas) + 1))
    floor.sort (reverse=True)
    print (f'  {"test":<56} {"median move":>12} {"worst":>8}  releases')
    for md, mx, t, n in floor:
        print (f'  {t[:56]:<56} {md*100:>11.0f}% {mx*100:>7.0f}%  {n}')
    if floor:
        print (f'  -> suite median noise floor: {statistics.median ([f[0] for f in floor])*100:.0f}% '
               f'per release, on ONE platform with no code change attributable')


if __name__ == '__main__':
    main ()