# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- MakeBuildRoot / out-of-source builds: moved to
  https://github.com/SophistSolutions/Stroika/issues/1170 - too big for this list. The Windows
  symbolic-link background it came out of is consolidated in
  https://github.com/SophistSolutions/Stroika/issues/1075.

- **plato: finish the `stroika-dev` Remote-SSH client setup** (2026-09-19, not started). Needs the
  `Host medusa-windows-dev` / `User stroika-dev` entry in `~/.ssh/config`, and a check that plato's
  own SSH key is in `stroika-dev`'s `authorized_keys` on medusa-windows-dev - that file was a static
  copy taken at setup time, not synced. Appending to it must be done as `lewis`, elevated: its ACL
  allows only `stroika-dev`/`SYSTEM`/`Administrators`. protagoras is already done.

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
       - **every sanitizer configuration is a `g++-*` one - there is no clang asan/tsan/ubsan anywhere**
         (noted 2026-09-10 during the compiler-coverage audit). clang's sanitizers are the better-supported
         ones upstream and diagnose somewhat different things, so this is a real second axis of the same
         single-platform problem, not a duplicate of it.

   - **Re-test the Ubuntu 24.04 gcc workarounds when that toolchain updates, and delete them if fixed.**
     `configure`'s `ApplyCompilerBugWorkarounds_` currently forces `-O2` for sanitizer configs on 24.04
     and warns about optimizing without LTO there. Both exist purely because gcc 13.3/14.2 *as packaged
     on 24.04* generate wrong code (measured 2026-08-30; 25.04, 26.04, g++-12 and clang++-18 are all
     clean). Cheap re-check, ~15 min on stroika-dev-2404:
       - sanitizer bug:  build `g++-release-sanitize_thread` at `-O3 -flto -fsanitize=thread`, run
         `Tests/47` - passes means workaround #1 can go
       - container bug:  build release `-O3` with `--lto disable`, run `Tests/21` and `Tests/51` - clean
         means workaround #2 (and the warning + the `--only-if-has-compiler` skip) can go
     Not worth filing upstream - it is confined to one distro's packaging, so Launchpad rather than GCC
     bugzilla, and it needs a reduced testcase we do not have.

   - **NEXT: EventFD Set ()/Clear () race** - https://github.com/SophistSolutions/Stroika/issues/1175. Found
     while documenting pollable2Wakeup for #843: concurrent Set/Clear can leave the flag and the descriptor
     disagreeing - a spin, or a lost wakeup in UpdatableWaitForIOReady (WebServer's ConnectionManager). Plan
     (in the issue): regression test first in Tests/40, then a mutex over the Set/Clear bodies.

   - **release build-time work.** Investigated 2026-08-27; all measurements and
     detail in `.claude/medusa-perf-knobs.md` (gitignored, on protagoras). Headline: host/VM/BIOS
     tuning is a DEAD END - governor, KSM, swappiness, VM socket topology, balloon sizing, EXPO and
     disk each measured at ~0-3%. Do not re-litigate those. Three real items:
      1. **Third-party builds are 471 of 627 min of a platform run; the tests themselves are 33 min.**
         Per-component caching (keyed version+toolchain+flags) keeps the guaranteed-clean-Stroika
         property a release run exists to prove, while cutting ~75% of wall clock. Biggest win by far.
         This is the cmake work.
      2. **Windows builds the cmake components serially** (no `-MP`; MSBuild cannot join make's
         jobserver): moved to https://github.com/SophistSolutions/Stroika/issues/1173, with the
         measurements. UNIX is already parallel via the `@+` recipe prefix.
      3. **Stop oversubscribing medusa.** 32 threads, and five Ubuntu runs at `-j8` plus the Windows
         VM is already past it. Measured: load 28 -> 604 min, load 34 -> 612 min, load 39 -> 961 min
         for the SAME work - a cliff at ~32 runnable. Staggering runs, or moving the Ubuntu matrix to
         hercules, beats every tuning knob. (hercules = older/slower twin of medusa, currently off.)
     Still open: medusa-windows-dev measured only ~1.04x protagoras despite ~2x hardware. Best
     remaining suspects are the guest's 8 vCPUs and VM per-file-operation overhead (NOT disk bandwidth
     - `%iowait` was 0.0-0.2% all week). Raise guest vCPUs at some restart and re-measure.

