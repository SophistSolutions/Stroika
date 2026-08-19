#!/usr/bin/env python3
"""Compare a Test52 performance dump against the checked-in historical results.

    Tests/Scripts/ComparePerformanceDumps.py CURRENT.txt HISTORICAL.txt [HISTORICAL2.txt ...]

(on Windows invoke via 'py' - plain 'python'/'python3' resolve to the Microsoft Store alias and fail.
CURRENT.txt is a PerformanceDump.txt, or the stdout of 'Test52 --show -x 15'; the historical files are
under Tests/HistoricalPerformanceRegressionTestResults/.)

CAVEAT, learned the hard way: comparing two dumps from DIFFERENT machines is mostly not meaningful. The
checked-in dumps under one platform label span boxes differing by up to 2.4x on the unchanged baseline
code, so treat the archive as a per-machine time series. See AnalyzePerformanceTrends.py for the
cross-platform-agreement approach, which does survive that.

PERFORMANCE_SCORE is a RATIO (comparison/baseline) and lower is better, so scores compare across
machines in a way raw seconds do not. Absolute seconds are still reported, because they say whether the
two runs were even on comparable hardware - a ratio that held steady while both sides got 3x slower is a
different story from one that held steady outright.
"""
import re
import sys
from pathlib import Path

# Test <title>
# \tPERFORMANCE_SCORE\t<score>
# \tDETAILS: \t[baseline test <b> secs, and comparison <c> sec, and warnIfPerfScore > <t>, and perfScore=<p>]
RE_TEST = re.compile (r'^Test\s+(.*?)\s*$')
RE_SCORE = re.compile (r'PERFORMANCE_SCORE\s+([\d.eE+-]+)')
RE_DETAILS = re.compile (
    r'baseline test\s+([\d.eE+-]+)\s+secs.*?comparison\s+([\d.eE+-]+)\s+sec'
    r'(?:.*?warnIfPerfScore\s*>\s*([\d.eE+-]+))?(?:.*?perfScore=([\d.eE+-]+))?')


def parse (path):
    """-> (dict title -> record, multiplier or None)"""
    out, mult, cur = {}, None, None
    for line in Path (path).read_text (errors='replace').splitlines ():
        if (m := re.search (r'TIME MULTIPLIER:\s*([\d.]+)', line)):
            mult = float (m.group (1))
            continue
        stripped = line.strip ()
        if (m := RE_TEST.match (line)) and not line.startswith ((' ', '\t')):
            cur = m.group (1)
            out[cur] = {'score': None, 'base': None, 'comp': None, 'thresh': None, 'warn': False}
            continue
        if cur is None:
            continue
        if (m := RE_SCORE.search (stripped)):
            out[cur]['score'] = float (m.group (1))
        elif (m := RE_DETAILS.search (stripped)):
            out[cur]['base'] = float (m.group (1))
            out[cur]['comp'] = float (m.group (2))
            if m.group (3):
                out[cur]['thresh'] = float (m.group (3))
            if m.group (4):
                out[cur]['score'] = float (m.group (4))   # more precise than the rounded line
        elif 'WARNING - expected' in stripped:
            out[cur]['warn'] = True
    return {k: v for k, v in out.items () if v['score'] is not None}, mult


def label (path):
    n = Path (path).stem
    return n[len ('PerformanceDump-'):] if n.startswith ('PerformanceDump-') else n


def main ():
    if len (sys.argv) < 3:
        sys.exit (__doc__)
    cur_path, hist_paths = sys.argv[1], sys.argv[2:]
    cur, cur_mult = parse (cur_path)
    print (f'CURRENT : {label (cur_path)}  ({len (cur)} tests, multiplier {cur_mult})')

    for hp in hist_paths:
        hist, hist_mult = parse (hp)
        print (f'\n{"="*118}\nBASELINE: {label (hp)}  ({len (hist)} tests, multiplier {hist_mult})')
        if cur_mult != hist_mult:
            print (f'  !! multipliers differ ({hist_mult} -> {cur_mult}); scores still comparable, absolute seconds are not')
        shared = [k for k in cur if k in hist]
        print (f'{"="*118}')
        # A score is comparison/baseline. Comparing scores across MACHINES is only valid if the two sides
        # scaled together. So decompose: how much did the baseline side slow down, and how much did the
        # Stroika side? If base_x == comp_x the score is unchanged and nothing happened. A score
        # "improvement" that is really just the BASELINE getting slower is an artifact, not a win - which
        # is the trap this whole comparison sets, since the baselines are plain vector/wstring/std::set
        # that Stroika never touched.
        rows = []
        for k in shared:
            o, n = hist[k]['score'], cur[k]['score']
            chg = (n - o) / o * 100 if o else 0.0
            ob, nb, oc, nc = hist[k]['base'], cur[k]['base'], hist[k]['comp'], cur[k]['comp']
            base_x = (nb / ob) if (ob and nb) else None
            comp_x = (nc / oc) if (oc and nc) else None
            rows.append ({'chg': chg, 'k': k, 'o': o, 'n': n, 'base_x': base_x, 'comp_x': comp_x,
                          'cw': cur[k]['warn'], 'hw': hist[k]['warn']})
        # the machine factor: how much slower this box ran the UNCHANGED baseline code
        bxs = sorted (r['base_x'] for r in rows if r['base_x'])
        machine = bxs[len (bxs) // 2] if bxs else 1.0

        print (f'  machine factor (median baseline slowdown, code Stroika did not change): {machine:.2f}x')
        print (f'  => scores are only trustworthy where comp_x tracks base_x; a score drop with '
               f'comp_x ~= base_x ~= {machine:.2f} is an ARTIFACT\n')
        print (f'{"test":<58} {"score old -> new":>17} {"chg":>7} {"base_x":>7} {"comp_x":>7}  verdict')
        print (f'{"-"*118}')
        rows.sort (key=lambda r: -abs (r['chg']))
        real = []
        for r in rows:
            bx, cx = r['base_x'], r['comp_x']
            # real change = the Stroika side moved differently from the baseline side
            rel = (cx / bx) if (bx and cx) else None
            if rel is None:
                verdict = '?'
            elif rel <= 0.80:
                verdict = 'REAL WIN'
                real.append ((r, rel))
            elif rel >= 1.25:
                verdict = 'REAL LOSS'
                real.append ((r, rel))
            else:
                verdict = 'artifact/noise'
            name = r['k'] if len (r['k']) <= 57 else r['k'][:54] + '...'
            warn = ' W-new' if (r['cw'] and not r['hw']) else (' W-gone' if (r['hw'] and not r['cw']) else '')
            bxs_ = f'{bx:.2f}' if bx else '-'
            cxs_ = f'{cx:.2f}' if cx else '-'
            print (f'{name:<58} {r["o"]:>7.3f} -> {r["n"]:<6.3f} {r["chg"]:>+6.1f}% {bxs_:>7} {cxs_:>7}  {verdict}{warn}')
        print (f'{"-"*118}')
        print (f'  {len (shared)} tests in both. By raw score: {sum (1 for r in rows if r["chg"] >= 10)} regressed >=10%, '
               f'{sum (1 for r in rows if r["chg"] <= -10)} improved >=10%')
        print (f'  After removing the machine factor: {sum (1 for r, x in real if x <= 0.80)} real wins, '
               f'{sum (1 for r, x in real if x >= 1.25)} real losses, {len (rows) - len (real)} noise/artifact')
        obs = [r['base_x'] for r in rows if r['base_x']]
        print (f'  baseline slowdown spread: {min (obs):.2f}x .. {max (obs):.2f}x (median {machine:.2f}x)')
        print (f'  warnings: {sum (1 for r in rows if r["hw"])} -> {sum (1 for r in rows if r["cw"])} '
               f'(NOT comparable - thresholds changed between releases, e.g. As<vector<int>> 1.5 -> 10)')
        only_cur = [k for k in cur if k not in hist]
        only_hist = [k for k in hist if k not in cur]
        for tag, lst in (('only in CURRENT (new tests)', only_cur), ('only in BASELINE (removed/renamed)', only_hist)):
            if lst:
                print (f'  {tag}: {len (lst)}')
                for k in lst:
                    print (f'      {k}')


if __name__ == '__main__':
    main ()
