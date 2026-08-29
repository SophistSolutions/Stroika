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
   - **verify if valgrind still useful, and revisit dynamic-analysis coverage broadly** - deliberately
     deferred from 3.0d24; LGP wants to look at the accumulated workarounds and ask what part of
     valgrind still earns its keep, rather than just switching it on somewhere new. Groundwork already
     done, so start from here:
       - valgrind **3.26.0 is already installed** on Stroika-Dev-2604. The 26.04 branch of
         `Build/Scripts/MakeRegressionTestConfigurations` has the `valgrind-release-SSLPurify-NoBlockAlloc`
         line present but **commented out** - uncommenting is the whole change, but it is unvalidated
         there. (The 24.04 one needs `ulimit -n 1024`, already handled in `Build/Scripts/RegressionTests`.)
       - 22.04 disabled valgrind entirely in Aug 2024 ("some tests fail inside valgrind code - looks
         like bug there"), so it has effectively been a single-platform tool for two years.
       - sanitizer coverage as of 3.0d24 was 24.04-ONLY (asan+ubsan+leak, tsan, valgrind all on one
         platform, one compiler) - and that compiler turned out to miscompile Tests/47 under LTO+TSAN.
         3.0d25 adds the sanitizer configs to 26.04; valgrind is the remaining single-platform one.
       - **GitHub Actions runs no sanitizer or valgrind job at all** - so dynamic analysis is entirely
         a local-release-run activity. Worth deciding if that is intentional.
       - msan is not usable with gcc (clang-only, and needs a specially rebuilt libc++) - see the note
         near the top of MakeRegressionTestConfigurations. So the realistic menu is asan/ubsan/leak,
         tsan, and valgrind; the question is whether valgrind still finds anything the first two do not.
   - **`LinearAlgebra::Vector<T>::operator[]` non-const returns a writeback proxy, which is a varargs
     footgun.** `TMP_` (see `Vector.h`) holds a `Vector<T>&` plus a copy of the element and calls
     `SetAt ()` in its destructor, with an implicit `operator T& ()`. So on a NON-const Vector,
     `printf ("%g", v[0])` compiles and passes a class type through varargs - undefined, and it prints
     garbage rather than the element (cost real debugging time on 2026-08-29; assigning to a `double`
     first, or taking the Vector by const ref, gives the right answer). Consider hardening: make the
     conversion explicit, add a `[[nodiscard]]`-ish guard, or drop the proxy in favor of `SetAt ()`
     (compare `Sequence<T>`, which deliberately does NOT do this - see the note on
     `Sequence<T>::operator[]` about `TemporaryElementReference_` being too costly).

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

