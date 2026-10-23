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
   - **Early: remove the Perl cross-check of configure and ApplyConfiguration** (#1240, closed; LGP 2026-10-21) - once the
     3.0d25 release runs and CI show no `PYTHON/PERL CONFIGURATION MISMATCH` or `... NOT CHECKED`: delete configure.pl,
     Build/Scripts/ApplyConfiguration.pl, Build/Lib/Python/PerlCheck.py, the code that runs them, and the Build/Lib/Perl
     files only they require. It caught no Python mistake, and failed builds twice on 10/20-21 (perl handed a C:/ path).
   - **Windows In-Docker regression runs: test against mongo** (LGP 2026-10-23) - Tests/36's 3 mongo tests skip there
     (12 SKIPPED per run in 3.0d25): Build/Scripts/RunLocalWindowsDockerRegressionTests passes only USE_TEST_BASENAME into
     the container, though Regression-Tests.md's command line sets MONGO_CONNECTION_STRING - pass it as
     RunRemoteRegressionTests does. Check the container resolves medusa.lan: that script's comment has Docker's DNS set to
     8.8.8.8 (#876).
   - **VS Code IntelliSense: `#include "AppVersion.h"` not found** (LGP 2026-10-23) in Samples/Service, and likely
     WebServer, WebService, HTMLUI/Backend and Skel's Basic app - each Makefile generates it into
     IntermediateFiles/$(CONFIGURATION)/<app>/ and adds that to the compiler's include path, which c_cpp_properties.json
     lacks. Since f4caa91a7a ApplyConfiguration's default myDefaultIncludePath is [] (not ${workspaceFolder}, which took
     a Skel app's root VERSION file for <version>), and nothing gives IntelliSense that directory instead. Only a
     c_cpp_properties.json with no myDefaultIncludePath gets the new default. IntelliSense only: every 3.0d25 release run
     built the samples.

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
