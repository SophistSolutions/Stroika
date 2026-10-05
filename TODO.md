# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

  - Consider rewrite of remaining perl stuff - mostly configure - to use python?

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
   - **SSDP, AFTER 3.0d25** (https://github.com/SophistSolutions/Stroika/issues/1194 - the rest of it is done):
       - **a network change waits for a running SSDP callback.** Listener's `Rejoin_` and Search's `SearchAgain_`
         stop (AbortAndWaitForDone) the thread that runs callOnFinds - so one doing a slow HTTP fetch (WTF's does) holds up the
         re-join, on the LinkMonitor thread every SSDP object shares. Documented ("Keep callOnFinds quick") for 3.0d25; the fix
         is to re-join without stopping the thread that calls back (or call back from another).
       - a reconnect re-joins / re-searches once per address it brings up (IPv4, then each IPv6), several in a row -
         `SSDP::Private_::FollowNetworkChanges` acts on every `eAdded`. Collapse a burst into one.
   - **dynamic-analysis coverage - what is left.** Valgrind itself was settled 2026-09-29 (#1177): kept, memcheck
     only, Release builds, on 24.04 and 26.04 - see Documentation/Debugging.md. The audit's sanitizer and valgrind
     retests are in https://github.com/SophistSolutions/Stroika/issues/1185. Still open:
       - **GitHub Actions runs no sanitizer or valgrind job at all** - so dynamic analysis is entirely
         a local-release-run activity. Worth deciding if that is intentional.
       - msan is not usable with gcc (clang-only, and needs a specially rebuilt libc++) - see the note
         near the top of MakeRegressionTestConfigurations. So the realistic menu is asan/ubsan/leak,
         tsan, and valgrind - and valgrind does find what the first two cannot: its first 26.04 run caught
         libstdc++'s from_chars overread (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=127666), which ASan missed.
       - **every sanitizer configuration is a `g++-*` one - there is no clang asan/tsan/ubsan anywhere**
         (noted 2026-09-10 during the compiler-coverage audit). clang's sanitizers are the better-supported
         ones upstream and diagnose somewhat different things, so this is a real second axis of the same
         single-platform problem, not a duplicate of it.
       - **no tool sees inside BlockAllocator's pools**, though block allocation is on by default -
         https://github.com/SophistSolutions/Stroika/issues/1181
       - optional: one memcheck run of the suite on the Pi (aarch64; valgrind 3.24 is installed there). Its unsigned
         `char` and 128-bit software `long double` differ from x86_64 in ways memcheck can see. The 2019 attempts that
         gave up on it (#837) were 32-bit armhf on Debian 10.
   - **WATCH: Windows Release-x86_64 SSDPClient segfault at startup** (parked 2026-09-27) - **DROP this entry if it
     has not recurred by 2026-10-27.** Seen once: Windows_MSYS_VS2k22 run at eb58defd98 (2026-09-26), both SSDPClient
     sample runs, Release-x86_64 only. It crashes before main: `Xerces::kDefaultProvider`'s initializer ->
     `XMLPlatformUtils::Initialize` -> ... `XMLString::parseInt`, whose machine code called `gTranscoder->transcode`
     with `this` = 0 (`xor ecx,ecx; call rax`) although `gTranscoder` was set. Relinking the same objects and libs with
     the same linker (MSVC 19.44.35229) 4 times on medusa gave correct code every time. Xerces is built `-GL`, so its
     code is generated at the final link - so a nondeterministic or link-environment-dependent LTCG miscompile.
     If it recurs: keep the failing exe + pdb; `C:/Sandbox/claude/ssdp/crashstack2.exe EXE ARGS` prints the stack and
     Xerces globals, `symaddr.exe EXE SYMBOL` + `dumpbin /disasm /range:` shows the code; the failing copy is kept
     there to compare. Candidate workaround: build Xerces without `-GL` under MSVC.

- v3.0d26x - at the start
   - **Replace Ubuntu 25.04 with 26.10** ("Stonking Stingray", released 2026-10-15) as the latest non-LTS. 25.04 has
     been unsupported since 2026-01, and so has 25.10. CI still has 25.04 entries in build-N-test-Matrix.json, plus
     the Build/Docker/Ubuntu2504-* images. Regenerate Documentation/SupportedPlatformsAndCompilers.md afterwards.

  - DO PLANNING for CMAKE change
    - discuss staging
    - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
    - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
    - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
