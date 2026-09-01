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
   - **Update the MSYS and cygwin runtimes on Medusa-Windows-Dev - they are ~16 months stale, and it
     is the only box that is.** Measured from the 3.0d24 run headers:
       - Protagoras native: MSYS `3.6.10` (2026-07-31)
       - the Windows docker images: MSYS `3.6.10` (2026-08-13), cygwin `3.6.10-1` (2026-07-13)
       - **Medusa-Windows-Dev: MSYS `3.6.1` (2025-04-20), cygwin `3.6.2-1` (2025-05-26)**
     Two reasons this matters beyond hygiene:
       - It ran 4 of the 15 3.0d24 platform targets (`Windows_{Cygwin,MSYS}_VS2k{22,26}`), so
         **native-cygwin was validated ONLY on a 15-month-old runtime** this release; current cygwin
         3.6.10 got exercised only via the in-Docker images.
       - cygwin/MSYS `fork()`+spawn cost dominates a make-heavy build, and that is exactly where this
         box underperforms. Evidence (3.0d24, same target, same `-j5`, matched by test name, both perf
         dumps at TIME MULTIPLIER 15 pinned to core 0): on CPU-pinned work Medusa-Windows-Dev is
         **0.75-0.77x** of Protagoras (ie ~1.3x FASTER per core, replicated across VS2k22 and VS2k26),
         and its test phase is faster too (27 vs 31-34 min) - yet whole-run wall clock is a tie or
         worse (620 vs 625 min; 797 vs 639 min). All of the loss is in the parallel build phase, which
         is ~95% of wall clock and almost entirely process spawning. Cheapest untested lever there is.
     Do NOT read the 797-vs-639 VS2k26 gap as a toolchain effect: sar shows medusa the host was 23.1%
     busy during the VS2k22 window vs 52.6% during VS2k26, so that pair is confounded by 2.3x load.

   - **START OF CYCLE: re-review the Medusa-Windows-Dev VM config (LGP asked to be reminded here).**
     Current state from `virsh dumpxml Medusa-Windows-Dev` as of 2026-09-01:
       - `<vcpu placement='static'>8</vcpu>` - 8 of the host's 32
       - `<topology sockets='2' dies='1' clusters='1' cores='2' threads='2'/>` - presents 2 SOCKETS
       - `<memory>24 GiB` max, `<currentMemory>18 GiB` (balloon), guest page file maxes at 28 GB
       - already done: `<cpu mode='host-passthrough'>`, virtio disk (`qcow2`, `cache='writeback'`,
         `discard='unmap'`), hyperv enlightenments
       - not present: hugepages (`<access mode='shared'/>` only), `<numa>`, `cputune`/`vcpupin`
     **Already measured at ~0-3% and recorded as dead ends - do not re-litigate:** socket topology,
     balloon sizing, governor, KSM, swappiness, EXPO, disk (see `.claude/medusa-perf-knobs.md`).
     **Raising vCPUs 8 -> 16 is probably NOT the lever, despite being the long-standing suspect.**
     Both boxes run `PARALELLMAKEFLAG=-j5`, and the inner cmake builds pass no `--parallel`, so
     neither box is core-limited: 8 vCPUs is already comfortably more than 5 concurrent jobs. The
     arithmetic that follows is the real finding - at equal load the build phases tie (462 vs 464 min)
     while the VM is 1.3x faster per core, so build-shaped work on the VM carries a **~1.3x penalty
     that exactly cancels its compute advantage**. With `%iowait` at 0.0-0.2% all week that is
     per-operation cost (process spawn, file metadata), not bandwidth. So attack the penalty, not the
     core count: newer cygwin/MSYS (above), Windows Defender exclusions for the build tree inside the
     guest (never checked - worth doing first, it is free), and raising `-j` on the VM to hide
     per-op latency. Raising vCPUs only helps in combination with a higher `-j`.
     Whatever is tried, change ONE thing at a time, and take every measurement in a window when the
     Ubuntu matrix is NOT running on medusa - the 23.1%-vs-52.6% host-load difference between the two
     3.0d24 windows is larger than any effect being chased.
     Also in scope at that point (LGP, 2026-09-01, explicitly a next-month item): **can the four
     `-In-Docker` Windows targets move off Protagoras onto Medusa-Windows-Dev?** He wants it if not
     too costly; early impression is "very slow", and there are two structural reasons to expect that:
       - Win11 Pro is a CLIENT SKU so only Hyper-V isolation works, which means a utility VM per
         container - on the KVM guest that is KVM -> Hyper-V -> utility VM, ie DOUBLE nesting, on top
         of the ~1.3x build penalty already measured on that box. Protagoras also uses hyperv
         isolation but is not nested.
       - Resource math does not fit as configured: `RunLocalWindowsDockerRegressionTests` asks for
         `CPUS_=6`, `MEMORY_=13G`, `DISK_=175G`, inside a guest with 8 vCPUs / 18 GB and 46.3 GB free
         of 510.8 GB on C:. Disk is the hard gate (~25-30 GB per VS BuildTools image, four images).
     Cheap way to decide instead of speculating: run ONE target (`Windows-MSYS-VS2k22-In-Docker`) there
     and compare against Protagoras' 3.0d24 time of 676 min. Under ~2x, it is viable; at 3x, drop it.

   - **`RegressionTests` expected-pass count is off by one per pass - `Tests/Tests-Description.txt`
     has no trailing newline.** `NUM_REGTESTS=$(wc -l Tests/Tests-Description.txt)` (line 177) returns
     **53** while 54 tests exist, because its last line (`[54]\tFrameworks::WebService`) ends without a
     newline. Two consequences, both of which cost real time during 3.0d24 validation:
       - `TOTAL_REGTESTS_EXPECTED_TO_PASS` is one-per-pass low, so the `*** WARNING: N tests succeeded
         and expected M` check at line 502 (`X1 -lt TOTAL`) cannot fire for a single missing test. The
         3.0d24 Ubuntu2204 run reported `485 items succeeded` / `0 items failed` with Test53 SIGKILLed;
         true count was 486 and the expectation was 477, so nothing tripped.
       - `X1 -eq TOTAL` at line 510 is therefore never true, so **every** run prints the weak
         `items succeeded (expected N * 53)` form and none ever prints the `AS expected` confirmation.
         That kills the one signal separating "complete" from "close enough" - the wording difference
         is the whole tell, and it is currently dead on every platform.
     Fix is one character (newline at EOF), but check nothing else parses that file by `wc -l` and
     expects 53. Worth also deriving the count from `ls -d Tests/[0-9][0-9]` so adding a test directory
     cannot silently desync it again.

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
   - **`Execution::SpinLock` - can the standalone `atomic_thread_fence` calls just go away?** They are
     the reason for `-Wtsan` ("atomic_thread_fence is not supported with -fsanitize=thread"), which
     means TSAN cannot see the happens-before edge SpinLock establishes - so anything synchronized by
     a SpinLock is analyzed only approximately, tending toward FALSE POSITIVE race reports. That is a
     real coverage hole, not just log noise, and it is suppressed twice over today
     (`DISABLE_COMPILER_GCC_WARNING_START` in `SpinLock.inl`, plus `-Wno-tsan` in `configure` because
     pragmas do not survive LTO).
     Observation worth checking: the fences look REDUNDANT. `try_lock ()` already does
     `fLock_.test_and_set (memory_order_acquire)` and `unlock ()` already does
     `fLock_.clear (memory_order_release)`, so for `BarrierType::eReleaseAcquire` the extra fences add
     nothing; `eMemoryTotalOrder` could use `memory_order_seq_cst` on those same operations instead.
     If so, deleting them silences `-Wtsan` legitimately AND lets TSAN analyze SpinLock-protected code.
     BUT: subtle memory-ordering code, and the current shape is deliberate - see
     https://github.com/SophistSolutions/Stroika/issues/628 (STK-494), and the in-source comment
     admitting "I don't understand why memory_order_acquire is good enough here". Read that first.

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

