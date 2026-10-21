# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- **plato: finish its Remote-SSH client setup, as `lewis`** (2026-09-19; the `stroika-dev` account it was to log in as
  was deleted 2026-10-20). Its key and shared ssh files are prepared; what is left - test from plato - is in the shared
  notes' ssh-cleanup checklist. protagoras is already done.

- v3.0d26x - at the start
   - **Issues scheduled for this release: milestone 3.0d26** - https://github.com/SophistSolutions/Stroika/milestone/3.
     What follows here has no issue.
   - **Early: a flaky test - `Frameworks_UPnP.SSDP_CachingListener_StaleSearchAnswer_`** (LGP 2026-10-21) - FAILED once,
     MacOS_XCode26_m1-3.0d25x (commit d7a28b1ea3, 10/20, line 3325 of its result file); passed in the 3.0d25 run
     (10b4202af3, 10/21). CachingListener is new in 3.0d25; timing-sensitive on macOS? Not yet looked at.
   - **Early: remove the Perl cross-check of configure and ApplyConfiguration** (#1240, closed; LGP 2026-10-21) - once the
     3.0d25 release runs and CI show no `PYTHON/PERL CONFIGURATION MISMATCH` or `... NOT CHECKED`: delete configure.pl,
     Build/Scripts/ApplyConfiguration.pl, Build/Lib/Python/PerlCheck.py, the code that runs them, and the Build/Lib/Perl
     files only they require. It caught no Python mistake, and failed builds twice on 10/20-21 (perl handed a C:/ path).

## Watching

Not work items: checks Claude runs (how often, in each) and raises with LGP only when one finds something - so they
never appear in a list of what to do next.

- **WATCH: the release regression results - check once or twice a day** (LGP 2026-10-20) for new failures, and raise
  each with its run's GIT COMMIT (it may be fixed already). Where they are, the one-command scan, and the known-benign
  warnings: the shared notes' regression-results-share note.

- **WATCH: does Node 26 stop Windows CI's HTMLUI failures?** Every build image moved to Node 26 on 2026-10-20 for
  them: "Error 127" at Samples/HTMLUI/QuasarBasedHTMLApp/Makefile:60, its Build.txt ending in "Build succeeded" then
  libuv's "Assertion failed: !(handle->flags & UV_HANDLE_CLOSING)" - Node aborting on exit
  (https://github.com/nodejs/node/issues/56645). Seen on Node 24 2026-10-05, 10-09 and 10-20, always
  windows-vs2k26-msys-x86_64-Release; 26 fixing it rests on one report that 26.5 did not reproduce it. If it recurs on
  26, retry `npm run build` once at that Makefile line. Drop this entry if CI stays clean to the 3.0d26 release. Once
  26 is LTS (expected late 2026-10), Windows-Cygwin's Dockerfile goes back to its commented-out nodejs-lts line.
