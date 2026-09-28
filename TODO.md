# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

  - https://github.com/SophistSolutions/Stroika/issues/1075
    Issue generates: on WTF:....
        (use "git push" to publish your local commits)

      Changes not staged for commit:
        (use "git add <file>..." to update what will be committed)
        (use "git restore <file>..." to discard changes in working directory)
              typechange: Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props
              typechange: Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.user.props

      Untracked files:
        (use "git add <file>..." to include in what will be committed)
              nw1.json
              nwinterface1.json
              test1.json
              test2.json

      no changes added to commit (use "git add" and/or "git commit -a")

      lewis@Protagoras MSYS ~/Sandbox/WTF/DevRoot
      $ git diff Workspaces/VisualStudio.net/
      warning: in the working copy of 'Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props', LF will be replaced by CRLF the next time Git touches it
      diff --git a/Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props b/Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props
      deleted file mode 120000
      index e9e2f47d..00000000
      --- a/Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props
      +++ /dev/null
      @@ -1 +0,0 @@
      -../../ThirdPartyComponents/Stroika/StroikaRoot/Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.ConfigurationBased.props
      \ No newline at end of file
      diff --git a/Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props b/Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props
      new file mode 100644
      index 00000000..7278e2a0
      --- /dev/null
      +++ b/Workspaces/VisualStudio.net/Microsoft.Cpp.stroika.ConfigurationBased.props
      @@ -0,0 +1,60 @@


  - KEEP GOING with 1177
  - Update the build time estimates - everything has
    been rebuilt since last round of estimates and its genreally improved.
  - take steps to reduce warnings/skips on rasp pi


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
    - NOTE WORKING NOW - this became https://github.com/SophistSolutions/Stroika/issues/1177

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

   - **#1177 bug-workaround audit - handoff** (2026-09-27). Progress is in the issue comments; the full Phase 1
     report is `.claude/BWA-AUDIT.md` on protagoras only (gitignored), summarized in the issue. Lists below are the
     Phase 1 summary's:
       - "Live Stroika bugs" and "fix regardless": all done except macOS `ifreq` (next entry).
       - "Not compiler bugs": items 1-8 done; next is 9, `template_optionalDeclareIncompleteType`, then 10-12 and the
         4 suspects.
       - "Dead once minimums match": the MSVC-only and Apple-clang-15 ones went with the minimum raise; still there
         are the Apple-clang-16 branches of shared macros, the armhf pair, and `ASAN_With_OpenSSL3_LoadLegacyProvider`.
       - Waiting on LGP: valgrind keep/drop, minimum compiler versions, naming convention. (clang+LTO: answered -
         GNU ld, now lld.)
       - **DEPRECATE, don't delete** these two - downstream apps (AGENTS.md "Downstream projects") use them:
         `qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy` (WTF) and
         `qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy` (HearHE). Re-grep those apps before
         removing any other public macro.

   - **`GetInterfaces_POSIX_` (IO/Network/Interface.cpp) - SIOCGIFCONF is the wrong API; decide the fix**
     (#1177 bucket 1, the last live bug; parked 2026-09-26, no code changed). `qMacUBSanitizerifreqAlignmentIssue_Buggy`
     only silences UBSan on the misaligned `const ifreq*` walk - macOS packs the records (`IFNAMSIZ + sa_len`), so
     most are not 8-aligned. Probing on lewis-Mac2 / stroika-dev-2604 found that is the least of it:
       - macOS: all 6 IPv6 addresses come back as GARBAGE (`::1` -> `::ee:ce6f:100:0`) - `SocketAddress{i->ifr_addr}`
         copies `sizeof (sockaddr)` = 16 of `sockaddr_in6`'s 28 bytes, and the rest is uninitialized stack
       - macOS: even the whole sockaddr is wrong for link-local - the kernel embeds the scope id in the address
         (`fe80:6::...` for `fe80::...%en0`); `getifaddrs` undoes that, SIOCGIFCONF does not
       - macOS: each IPv6 CIDR gets the interface's IPv4 prefix - `getNetMaskAsPrefix` uses SIOCGIFNETMASK (IPv4 only)
       - Linux: SIOCGIFCONF returns only AF_INET, so Stroika reports NO IPv6 addresses at all (getifaddrs has `lo ::1`)
     Options: (a) replace the walk with `getifaddrs` (aligned, full IPv6, scope fixed, `ifa_netmask` replaces the
     SIOCGIFNETMASK lambda; behavior change + UPGRADE NOTE: Linux gains IPv6 and no-IPv4 interfaces) - recommended;
     (b) alignment only (memcpy each record into an aligned `ifreq`: 8 UBSan reports -> 0, same 24 records) + file the
     IPv6 bug; (c) patch SIOCGIFCONF in place (full copy + manual scope fixup + SIOCGIFNETMASK_IN6; Linux still no
     IPv6). Test-first for (a)/(c): Tests/43 `Test3_NetworkInterfaceList_` only DbgTraces - make it check every reported
     address is one `getifaddrs` reports for that interface (fails on macOS today). Probes: C:/Sandbox/claude/skips/
     ifreq.cpp (alignment), ifaddr.cpp (IPv6 vs getifaddrs), ifconf-linux.cpp.

- DO PLANNING for CMAKE change
  - discuss staging
  - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
  - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
  - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
