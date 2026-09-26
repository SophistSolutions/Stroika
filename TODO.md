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

- DO PLANNING for CMAKE change
  - discuss staging
  - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
  - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
  - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
  