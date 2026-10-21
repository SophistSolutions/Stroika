# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- **plato: finish its Remote-SSH client setup, as `lewis`** (2026-09-19; the `stroika-dev` account it was to log in as
  was deleted 2026-10-20). Its key and shared ssh files are prepared; what is left - test from plato - is in the shared
  notes' ssh-cleanup checklist. protagoras is already done.

- **3.0d25 release - TONIGHT, before the release builds** (LGP 2026-10-20): bring the sample HTML app up to date -
  `Samples/HTMLUI/QuasarBasedHTMLApp` (the only npm project in Stroika): `npm update`/upgrade to the latest Quasar
  (quasar, @quasar/app-vite, @quasar/extras) and current npm (the build images now carry Node 26), and fix what
  `npm audit` reports. A major-version bump can break silently at run time (3.0d24's app-vite 3 move did), so run the
  app, not just build it.

- v3.0d26x - at the start
   - **Issues scheduled for this release: milestone 3.0d26** - https://github.com/SophistSolutions/Stroika/milestone/3.
     What follows here has no issue.

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
