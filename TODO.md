# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- **ModuleInit's deprecation text does not cover the case that actually bites** (2026-10-12). "Use call_once or
  static inline" is right for construction order, and for destruction where the dependency is formed AT
  construction. It is not right for a global that LATER acquires a reference to another module's object - the
  reverse-destruction guarantee of [basic.start.term] does not reach that. A cached XML Schema::Ptr was exactly that
  case, until each schema and document came to share its XML library's lifetime with the provider (3.0d25): the answer
  is that - what is held keeps alive what it needs - or not to hold one. Worth a sentence in the deprecation note so the
  next person does not re-derive it.

- config var name ObjDir_ToolsSafe funny name - probably lose the Safe!! part and DOCUMENT what it means!

- Consider rewrite of remaining perl stuff - mostly configure - to use python?
  (test speed of existing make defualt-configs and applyconfigs - that may justify the rewrite).

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
   - **dynamic-analysis coverage - what is left.** Valgrind itself was settled 2026-09-29 (#1177): kept, memcheck
     only, Release builds, on 24.04 and 26.04 - see Documentation/Debugging.md. The audit's sanitizer and valgrind
     retests are in https://github.com/SophistSolutions/Stroika/issues/1185. Still open:
       - **GitHub Actions runs ASan+UBSan, but no TSan or valgrind** (corrected 2026-10-13). Each Debug job's
         --apply-default-debug-flags turns them on - ASan+UBSan with gcc and clang (not on 22.04), UBSan alone on macOS,
         ASan with MSVC - so clang++-22-debug and the two Windows x86 Debug jobs run them on every push. Since 2026-10-13 a
         UBSan finding fails the test there (UBSAN_OPTIONS=halt_on_error=1); before, it printed into a green log. TSan and
         valgrind stay local-release-run only: TSan in CI waits for #1205's work (3.0d26), valgrind is too slow for it.
       - msan is not usable with gcc (clang-only, and needs a specially rebuilt libc++) - see the note
         near the top of MakeRegressionTestConfigurations. So the realistic menu is asan/ubsan/leak,
         tsan, and valgrind - and valgrind does find what the first two cannot: its first 26.04 run caught
         libstdc++'s from_chars overread (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=127666), which ASan missed.
       - **no clang TSan anywhere** - every TSan configuration is a `g++-*` one (clang's ASan+UBSan do run: in CI, and in
         the local clang Debug configurations - corrected 2026-10-13). clang's sanitizers are the better-supported
         ones upstream and diagnose somewhat different things, so this is a real second axis of the same
         single-platform problem, not a duplicate of it.
       - **no tool sees inside BlockAllocator's pools**, though block allocation is on by default -
         https://github.com/SophistSolutions/Stroika/issues/1181
       - optional: one memcheck run of the suite on the Pi (aarch64; valgrind 3.24 is installed there). Its unsigned
         `char` and 128-bit software `long double` differ from x86_64 in ways memcheck can see. The 2019 attempts that
         gave up on it (#837) were 32-bit armhf on Debian 10.
   - **strace and tcpdump in the stroika-dev image** (LGP 2026-10-18, a suggestion) - for interactive debugging, not for any
     test, so not in the RegressionTests images. tcpdump -v shows what went on the wire (a packet's TTL, SSDP/GENA/HTTP
     messages); strace, the system call a hung thread is blocked in, and sendto/recvfrom addresses with their IPv6 scope id
     (#1195, #1201). One line in Build/Docker/Stroika-Dev/Dockerfile's dev-tools install (beside iproute2 and lldb), plus
     `setcap cap_net_raw+ep /usr/bin/tcpdump` for a non-root user (Docker grants CAP_NET_RAW by default; capture with -p, as
     promiscuous mode needs NET_ADMIN). strace works on a process it starts; attaching to a running one needs
     CAP_SYS_PTRACE, which these containers lack.
- v3.0d26x - at the start
   - **Issues scheduled for this release: milestone 3.0d26** - https://github.com/SophistSolutions/Stroika/milestone/3.
     What follows here has no issue.
   - **With #1205's copy-on-write race fixed, restore Tests/55's reproducer**: its SSDP_CachingListener_ and
     SSDP_CachingListener_Search_ read told under its lock (a BWA, so 3.0d25's ThreadSanitizer runs are quiet) - put their
     told.load () back.
   - **Replace Ubuntu 25.04 with 26.10** ("Stonking Stingray", released 2026-10-15) as the latest non-LTS. 25.04 has
     been unsupported since 2026-01, and so has 25.10. CI still has 25.04 entries in build-N-test-Matrix.json, plus
     the Build/Docker/Ubuntu2504-* images. Regenerate Documentation/SupportedPlatformsAndCompilers.md afterwards.
   - **WATCH: does Node 26 stop Windows CI's HTMLUI failures?** Every build image moved to Node 26 on 2026-10-20 for
     them: "Error 127" at Samples/HTMLUI/QuasarBasedHTMLApp/Makefile:60, its Build.txt ending in "Build succeeded" then
     libuv's "Assertion failed: !(handle->flags & UV_HANDLE_CLOSING)" - Node aborting on exit
     (https://github.com/nodejs/node/issues/56645). Seen on Node 24 2026-10-05, 10-09 and 10-20, always
     windows-vs2k26-msys-x86_64-Release; 26 fixing it rests on one report that 26.5 did not reproduce it. If it recurs on
     26, retry `npm run build` once at that Makefile line. Drop this entry if CI stays clean to the 3.0d26 release. Once
     26 is LTS (expected late 2026-10), Windows-Cygwin's Dockerfile goes back to its commented-out nodejs-lts line.


  - DO PLANNING for CMAKE change
    - discuss staging
    - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
    - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
    - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)
