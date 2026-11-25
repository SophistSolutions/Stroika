# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- document or reconsider the practice of #include Stroika-Frameowkrs-Precome
  Stroika-Foundation-Precompe.h on every cpp and .h file.
  

- **plato: finish its Remote-SSH client setup, as `lewis`** (2026-09-19; the `stroika-dev` account it was to log in as
  was deleted 2026-10-20). Its key and shared ssh files are prepared; what is left - test from plato - is in the shared
  notes' ssh-cleanup checklist. protagoras is already done.

- **Consider dropping ThirdPartyComponents/Origs-Cache, now that Conan caches the components** (LGP 2026-11-20) - once
  Conan's cache is vetted and has been used a while: probably no need for both. With every component through Conan, an
  archive is needed only when Conan builds a package, and Conan can keep that itself (a recipe's source (), done once per
  recipe revision; its download cache) - so the per-tree Origs-Cache, written into the source tree, may go.

- **Cold third-party builds: measure, then maybe build the components in parallel** (LGP 2026-11-25; #1157's build
  speed) - each component builds inside `conan install`, under Conan's lock (ThirdPartyComponents/CMakeLists.txt), so a
  build with nothing in Conan's cache makes them one at a time. First count how many builds are still cold, once CI's
  per-job caches and the Windows docker runs' Conan volumes (721cc8703a) have run a while. If still common: build each
  component in the tree, outside Conan, and hold the lock only for Conan's short steps (export, then `conan export-pkg`,
  then the install into Builds) - the builds sharing make's job slots, or a job pool under Ninja; check the debug path
  maps against the tree's build folder.

- v3.0d26x - at the start
   - **Issues scheduled for this release: milestone 3.0d26** - https://github.com/SophistSolutions/Stroika/milestone/3.
     What follows here has no issue.
   - **String: a UTF-8 (char8_t) rep** (LGP 2026-11-07; may postpone, once discussed) - so a constant u8 string that is not ASCII
     is shared, not decoded and copied: enum names are UTF-8 now (EnumNameText), shared already when ASCII, as nearly all are.
     String.inl's FromStringConstant has the @todo and the design question (a UTF-8 rep cannot index directly).
   - **After the float and Duration formatting redesign: which other Options want `With`** (LGP 2026-11-06) - scan
     Stroika's Options structs and option classes for constructors that merge option sets or convert from each field's
     type, and `optional<>` fields there only to overlay; convert those whose callers lay settings over a base
     (Design-Overview.md, "Options structs", has the style).

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

- **WATCH: does installing WebView2 first stop the VS 2022 docker image's intermittent save failure? - each Windows
  image run** (LGP 2026-11-03) Windows-VS2k22/Dockerfile installs the current Edge WebView2 before VS, so the VS
  installer's own 131.0.2903.86 - and the background update whose download, if the install step ends during it, leaves
  a layer docker cannot save - never happens. In each vs2k22 job's install step log, the updater lines should show no
  install. If the step still fails with hcsshim::ImportLayer (0x3), read that log: was the updater still downloading (a
  `DoDownloadPackage` with no `[Running installer]` after it)? Now and then, check the two Microsoft reports linked in
  that WebView2 step's comment for replies: once either is fixed, try the build without the step. Drop the logging and
  this entry if CI stays clean to the 3.0d26 release.
