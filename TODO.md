# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- **Update the per-target runtime estimates in `Documentation/Regression-Tests.md`** - Lewis says
  they are stale and highly in doubt. Every run writes its own `Finished at ... (N minutes)`, and
  sar on medusa keeps 10-min CPU history, so real durations can be recovered AND annotated with the
  host contention they ran under (which varied 56-95% busy across the 3.0d24 release week).

- v3.0d25
   - verify if valgrind still useful - and look at getting it on Ubuntu 2604 (maybe ask Claude about
     coverage of various sanitizers - and ask for suggestions)
   - **release build-time work.** Investigated 2026-08-27; all measurements and
   detail in `.claude/medusa-perf-knobs.md` (gitignored, on protagoras). Headline: host/VM/BIOS
   tuning is a DEAD END - governor, KSM, swappiness, VM socket topology, balloon sizing, EXPO and
   disk each measured at ~0-3%. Do not re-litigate those. Three real items:
      1. **Third-party builds are 471 of 627 min of a platform run; the tests themselves are 33 min.**
         Per-component caching (keyed version+toolchain+flags) keeps the guaranteed-clean-Stroika
         property a release run exists to prove, while cutting ~75% of wall clock. Biggest win by far.
         This is the cmake work.
      2. **`cmake --build .` passes no `--parallel`, and `/MP` is nowhere** - so mongo-cxx-driver,
         Xerces, libxml2, GoogleTest, zstd and zlib build serially on both axes. Do NOT just add a
         job count: outer `-j5` already runs up to 5 components at once, so a per-cmake number
         multiplies into sludge. Right fix is GNU make's jobserver (globally bounded, no hardwired
         value) - see the commented-out 2025-03-19 experiment at
         `Build/Lib/Make/Makefile-CMake-Common.mk:68`. That probably failed because make 4.3 passed
         the jobserver by FD; make 4.4 uses a named FIFO. Caveats to test first: only Ubuntu2604 has
         make 4.4.1 (2204/2404 are 4.3, Windows/MSBuild has no jobserver at all), and openssl/curl
         deliberately `--unset=MAKEFLAGS`. Deferred from 3.0d24 as too risky mid-release.
      3. **Stop oversubscribing medusa.** 32 threads, and five Ubuntu runs at `-j8` plus the Windows
         VM is already past it. Measured: load 28 -> 604 min, load 34 -> 612 min, load 39 -> 961 min
         for the SAME work - a cliff at ~32 runnable. Staggering runs, or moving the Ubuntu matrix to
         hercules, beats every tuning knob. (hercules = older/slower twin of medusa, currently off.)
   Still open: medusa-windows-dev measured only ~1.04x protagoras despite ~2x hardware. Best
   remaining suspects are the guest's 8 vCPUs and VM per-file-operation overhead (NOT disk bandwidth
   - `%iowait` was 0.0-0.2% all week). Raise guest vCPUs at some restart and re-measure.

