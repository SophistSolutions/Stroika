# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- **Tests/40 timing failures - decide what to do, once there is data** (2026-09-22). Tests/40
  intermittently fails on Windows CI with a 1s `WaitForDone` taking ~32s. `ClockContinuitySampler`
  now reports, in the failure text, the largest gap in which the process was not running - a healthy
  machine reads ~0.06s, so a reading of seconds means the HOST stalled and the measurement says
  nothing about Stroika. Full analysis is in the commit message for 118e1e2b88; do not re-derive it.
  Next: wait for a CI failure, read that number, then decide - demote the assertion to a warning when
  the host stalled, or investigate Stroika's wait for real. Do NOT just raise the margin again (it has
  gone 2 -> 5 -> 7 -> 10 -> 15s already, each time without knowing which cause was being accommodated).
  Note LGP has never seen this on medusa-windows-dev's own regression runs - only in CI.

- MakeBuildRoot / out-of-source builds: moved to
  https://github.com/SophistSolutions/Stroika/issues/1170 - too big for this list. The Windows
  symbolic-link background it came out of is consolidated in
  https://github.com/SophistSolutions/Stroika/issues/1075.

- **New non-admin `stroika-dev` account set up on medusa-windows-dev for Remote-SSH dev work**
  (2026-09-19), to avoid Win32-OpenSSH granting an unfiltered admin token to Administrators-group
  SSH logons (root cause + full analysis: see git history / prior session around this date if this
  entry has gone stale). Status:
    - **medusa-windows-dev side (done):** local standard user `stroika-dev` created (not in
      Administrators; in `docker-users` + `Performance Log Users`), profile provisioned, SSH pubkey
      auth working (`administrators_authorized_keys` copied to its own `.ssh\authorized_keys` with a
      locked-down ACL). Granted `stroika-dev` WMI `root\cimv2` rights (Enable + Execute Methods +
      **Remote Access** + Read Control) - required because VS Code's Remote-SSH Windows install
      script calls `Get-CimInstance win32_process` to verify its own sshd parent, and SSH is a
      *network* logon so WMI treats it as remote access, which a standard account lacks by default
      (see microsoft/vscode-remote-release#2648 - long-standing upstream issue, admin accounts don't
      hit this). `C:\Users\stroika-dev\.vscode-server\data\Machine\settings.json` has a `bash
      (MSYS2)` terminal profile (`MSYSTEM=MINGW64`, non-login `-i` bash - login shells fought the
      `cwd` setting; PATH setup lives in `.bashrc` accordingly) plus
      `terminal.integrated.cwd: ${workspaceFolder}`, which correctly lands new terminals in whatever
      workspace is open (verified dynamic, not hardcoded). `~/.bashrc` (at `/c/Users/stroika-dev`,
      note: NOT `/home/stroika-dev` - see the MSYS "wrong HOME" gotchas below) has the mingw64/bin
      and VS2026-clang-format PATH lines. Copied `id_rsa`/`id_rsa.pub` + `known_hosts` (github.com
      entries only) from lewis into `/home/stroika-dev/.ssh` (yes, the *other* MSYS home - `ssh`/
      `git` resolve home differently than bash's own `$HOME`) with a locked-down ACL on `id_rsa`, so
      `git pull`/push work over plain SSH. Verified `lewis` and `stroika-dev` can both
      read/write/delete files in the shared `C:\Sandbox\Stroika\DevRoot` checkout with no ACL
      conflicts.
    - **Gotcha found 2026-09-19, orthogonal to the ACL check above: Windows Mandatory Integrity
      Control (MIC), not the DACL, can still block `stroika-dev`.** Hit via `make distclean` failing
      "Permission denied" on `Builds/Debug/Samples-HTMLUI/Stroika-Sample-HTMLUI-Windows-x86_64-Debug.msi`
      even though `icacls` showed the DACL granting Authenticated Users Modify. `icacls` also showed
      `Mandatory Label\High Mandatory Level:(NW)` on that one file - a "No-Write-Up" label that blocks
      any Medium-integrity process from writing/deleting it regardless of DACL. Confirmed `lewis`'s
      own *non-elevated* MSYS shell hit the identical error - being in Administrators does not make a
      normal shell's token elevated (UAC), so ordinary `lewis` builds are Medium integrity too, same as
      `stroika-dev`. Root cause isn't the account, it's **elevation**: a file only gets stamped High if
      some process actually ran elevated against it - here, almost certainly test-installing the MSI
      (double-click "Install", or `msiexec /i`, which triggers a UAC-elevated `msiexec` that stamps the
      source file it installs from). Only one file in the whole `Builds/` tree had the label (checked
      via recursive `icacls /t /c` + grep), so this is not tree-wide contamination, just a one-off from
      that workflow. Fixed by running, from an elevated shell, either
      `icacls "<path>" /setintegritylevel Medium` or just deleting the file. **Guidance going forward:**
      don't test-install a built `.msi` in place inside the shared checkout - copy it to `%TEMP%` (or
      similar) first, so the elevated `msiexec` never touches a tracked file. If this resurfaces, the
      tell is "Permission denied" despite `icacls` showing a permissive DACL - check for a Mandatory
      Label before assuming an ACL/ownership problem.
    - **protagoras (client) side (done):** `~/.ssh/config` `Host medusa-windows-dev` entry points
      `User stroika-dev`; VS Code Remote-SSH connects, installs its server, and the terminal profile
      above works correctly (confirmed `MINGW64`, `python3`, `clang-format`, dynamic workspace cwd
      all resolve).
    - **plato (client) side (not started):** needs the same `~/.ssh/config` edit, plus check whether
      plato's own SSH key is already in `stroika-dev`'s `authorized_keys` on medusa-windows-dev (it's
      a static copy taken at setup time, not synced) - if not, it needs appending (as `lewis`,
      elevated - that file's ACL only allows `stroika-dev`/`SYSTEM`/`Administrators`).
    - **General gotcha for anyone continuing this:** several "wrong HOME" traps exist depending on
      *how* a shell/tool is launched for this account - bash's own `$HOME` resolves to
      `C:\Users\stroika-dev` in most contexts, but `ssh.exe`/`git`'s bundled ssh resolve home via
      nsswitch to `C:\msys64\home\stroika-dev` instead. Check both if something that depends on
      dotfiles/known_hosts/keys mysteriously doesn't see them.
  A `medusa-windows-dev` reboot was done ~2026-09-19 as a sanity check that all of the above (SAM
  account, WMI security descriptor, files/ACLs) survives a restart - since it's all disk-backed, it
  should, but verify the checklist above still holds after any future reboot too, just in case.

- **[FIX SINCE BACKED OUT - see 2026-09-22 note at the end] Pasting into any MSYS2 bash session on medusa-windows-dev was slow,
  proportional to paste length** (a few chars: instant; a few lines: up to ~1 min). Isolated
  2026-09-19 while setting up the `stroika-dev` non-admin SSH account; root-caused the same day once
  other work reached a good breaking point. Root cause: an upstream Cygwin/MSYS2 regression
  (`msys2-runtime` 3.6.10-1 through 3.6.10-3) in `process_input_message()` - pasted characters got
  processed twice, once by `select()` and once by `read()`, doubling up echo/side-effect work
  per-character, which is why cost scaled with paste length and pegged a CPU core. Confirmed
  installed version on medusa-windows-dev was `3.6.10-3`. Upstream:
  https://github.com/msys2/msys2-runtime/issues/358, fixed by
  https://github.com/msys2/msys2-runtime/pull/361, released as `msys2-runtime 3.6.10-4`
  (2026-09-09). Fixed by running `pacman -Syu` on medusa-windows-dev, which pulled `3.6.10-4` plus
  ~20 other routine package updates - confirmed via `pacman -Q msys2-runtime` before/after and
  `pacman -Qu` showing nothing left pending. **Any MSYS2 shell that was already open across the
  upgrade (e.g. an existing WindowsTerminal/RDP session) is still running the old `msys-2.0.dll` in
  memory and won't get the fix until it's closed and reopened** - pacman's own core-update step
  tries to kill all other MSYS2 processes to force this and failed with Access Denied against a
  same-box RDP session's shells (different Windows session, no privilege to kill across sessions),
  which is expected, not a bug. If paste is still slow in a shell you had open before this date,
  that's why - restart the shell. New shells opened after 2026-09-19 should be unaffected. (Prior
  workaround, no longer needed for new shells: paste into a plain-text editor tab and `Save As` to
  write the target file directly, rather than pasting multi-line content into the MSYS bash terminal
  itself.)
  **Update same day: fix is real but only partial, and now fully root-caused - closing this out as
  "as good as it gets without upstream project-level work."** Post-upgrade, a fresh shell (post-reboot,
  so definitely on `3.6.10-4`) still took **~7 seconds** to paste a ~6-line/~2000-character block -
  down from "up to ~1 min" (a real ~5-10x win, confirming the regression fix helped) but nowhere near
  instant. Isolated the remaining cost by pasting the *identical* block into three tabs of the same
  Windows Terminal window: `cmd.exe` <1s, MSYS2 bash (patched, `3.6.10-4`) ~7s, Cygwin bash
  (a separate, unrelated install at `C:\cygwin`, still on unpatched `cygwin 3.6.10-1`) ~11s.
  `cmd.exe` being instant in the *same* ConPTY-backed window rules out ConPTY itself as the
  bottleneck (ruling out the earlier "leading suspect") - cmd relies on Windows' native cooked-mode
  console line reader, which hands it a whole buffered paste in one call. Cygwin and MSYS2 landing in
  the same ballpark (unpatched Cygwin worse than patched MSYS2, for the identical paste) points at the
  shared Cygwin-lineage console-input code both runtimes use: unlike cmd, they bypass native
  cooked-mode and read `ReadConsoleInput` record-by-record themselves (needed for POSIX tty semantics -
  raw mode, signals, etc.), and that per-record handling is the real remaining cost. The regression
  already fixed made this dramatically worse; the ~7s left looks like that architecture's baseline
  cost, not a further discrete bug - fixing it would mean reworking Cygwin/MSYS2's own console-input
  layer upstream, well beyond a "nice to have." Not planning further root-causing past this point.
  (Aside, not acted on: the separate `C:\cygwin` install is still on the unpatched `3.6.10-1` and would
  need its own upgrade if anyone actually uses it day-to-day - it isn't part of the standard MSYS2-based
  dev setup this TODO entry is about.)
  **2026-09-22: medusa-windows-dev is back on `msys2-runtime 3.6.9-2`**, so everything above that
  describes the box as running `3.6.10-4` is stale (`pacman -Q` and `uname -r` agree on 3.6.9-2, so
  it is a real package downgrade, not the single-DLL swap). Paste is unaffected by the downgrade:
  3.6.9 predates the regression entirely, and 3.6.10-4 only restored what 3.6.9 already had - so
  either way you get the ~7s architectural baseline described above, not the ~1min regression.
  The reason to be down there is instead
  https://github.com/SophistSolutions/Stroika/issues/1169 - the parallel-build hang, which affects
  3.6.10-1 through 3.6.10-4 with no fixed release, so 3.6.9 is the only version that avoids it.
  Revisit when upstream ships one with both the hang fix and 3.6.10's other changes.

- v3.0d25
   - Consider losing SystemErrorException - a bit of a footgun (someone could catch it - like TimeOutException)
     Instead - all it does can easily be done just with Exception<system_error> {};
     
   - **`e.code () == errc::X` vs `e.code ().value () == SOME_CONSTANT` - the right form is subtle and nothing
     enforces it.** The condition test is correct and portable; the raw-value test compiles, looks
     reasonable, and is usually wrong - it only matches if the category happens to be the one you
     assumed. This is inherited from the standard, not created by Stroika, but Stroika could make the
     right thing shorter than the wrong thing. Note `Tests/37` already has a regression test naming this exact
     trap ("the condition-vs-code trap"), so the failure mode is understood - what is missing is an API that
     steers people. Ideas, unevaluated: a `Execution::IsA (e, errc::X)` helper; a `[[nodiscard]]`-ish wrapper;
     or just a documented lint. Cheap to think about, no urgency.

   - **RaspberryPi is stuck on old compilers - see about supporting more recent ones.** The
     cross-compile target builds only `arm-linux-gnueabihf-g++-11/12/13`, so it is the oldest
     toolchain set Stroika still tests, and g++-11/12 there are two of the few configurations still
     needing the fmtlib polyfill (no usable `<format>` before g++ 13). Worth checking what newer
     `arm-linux-gnueabihf-g++` versions are available for the build container, and whether the Pi's
     own OS release supports them.
     @see Documentation/SupportedPlatformsAndCompilers.md for the current matrix.

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

   - **GitHub Actions: consider a `concurrency` group with `cancel-in-progress` on v3-Dev.** Deferred
     2026-08-29 at LGP's request - revisit ~mid/late Sep 2026, after other work. Measured over the last
     199 `build-N-test` runs (2026-07-08..08-27, all v3-Dev):
       - median run wall-clock **145 min** (max 1827 min = 30 h)
       - **146 of 199 runs (73%) were superseded** by a newer push while still running, and ran to
         completion anyway; those obsolete runs kept going for **418 h of wall-clock**
     There is no `concurrency:` key in the workflow at all today. Proposed (scope to the dev branch so
     release-branch runs always finish):
     ```yaml
     concurrency:
       group: ${{ github.workflow }}-${{ github.ref }}
       cancel-in-progress: ${{ github.ref == 'refs/heads/v3-Dev' }}
     ```
     Trade-off: you lose CI results for intermediate commits when pushes come faster than 145 min -
     which matters for bisecting. Note this dwarfs the `fail-fast: false` change (committed
     2026-08-29), which costs only ~9 h/week.
     Related: no `timeout-minutes` is set on any job, so GitHub's 6 h/job default is the only bound -
     worth setting alongside this (cf. that 30 h run).

   - https://github.com/SophistSolutions/Stroika/issues/843

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

