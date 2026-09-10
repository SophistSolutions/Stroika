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
   - **2026-09-10: close https://github.com/SophistSolutions/Stroika/issues/1165 once the libc++ run
     passes.** Left open on purpose: the fix is in (feff64174b) and verified on mechanism 1 (Windows/MSVC
     and 2604 g++/libstdc++) and on mechanism 2 with the macro forced, but no real libc++ toolchain has
     compiled it - see the next item. Run `clang++-18-debug-libc++`, then close the issue.

   - **No `--stdlib libc++` build has compiled the new WaitForIOReady wakeup.** Those configurations take
     mechanism 2 (`ppoll` + blocked signal mask), because `__cpp_lib_jthread` is undefined for every libc++
     before LLVM 20 - implemented in 18, but behind `-fexperimental-library` until 20, which Stroika passes
     nowhere. The #1165 fix (feff64174b) is verified on mechanism 1 (Windows/MSVC, and 2604 g++/libstdc++)
     and on mechanism 2 with the macro forced on 2604 - but no actual libc++ toolchain has seen it. One
     `clang++-18-debug-libc++` run closes this, and overlaps the clang++-19 gap below.

   - **Mechanism 3 of the WaitForIOReady wakeup (chunked `poll`) has never been compiled anywhere.** It is
     selected only where neither jthread nor `ppoll` exists - i.e. old XCode. lewis-Mac2 is XCode 17, which
     DOES define `__cpp_lib_jthread`, so it takes mechanism 1 and cannot exercise this. Not verifiable with
     the hardware on hand; decide whether that is acceptable or whether the fallback should just go.

   - **`make check-prerequisite-tools` cannot tell BSD `realpath` from GNU, so a macOS build fails far from
     its cause.** The check only runs `type realpath`, which finds `/bin/realpath` and passes. The build then
     dies building third-party components (`zstd`, `zlib`, `gtest`) with `realpath: illegal option -- -`,
     followed by a runaway recursive make (`make[522]`, `"Debug" is not a valid configuration`). Stroika
     already ships the stand-in (`Build/Tools/Src/realpath.cpp`, target `install-realpath`) - the check just
     never verifies the flavour. Cost real time 2026-09-09.

   - **`e.code () == errc::X` vs `e.code ().value () == SOME_CONSTANT` - the right form is subtle and nothing
     enforces it.** Raised in the same design review. The condition test is correct and portable; the raw-value
     test compiles, looks reasonable, and is usually wrong - it only matches if the category happens to be the
     one you assumed. This is inherited from the standard, not created by Stroika, but Stroika could make the
     right thing shorter than the wrong thing. Note `Tests/37` already has a regression test naming this exact
     trap ("the condition-vs-code trap"), so the failure mode is understood - what is missing is an API that
     steers people. Ideas, unevaluated: a `Execution::IsA (e, errc::X)` helper; a `[[nodiscard]]`-ish wrapper;
     or just a documented lint. Cheap to think about, no urgency.

   - **`Execution::TimedLockGuard` vs `Execution::UniqueLock` - decide which should survive, then fix or delete.**
     Deliberately left out of the 3.0d25 exception/TimeOutException change as a separable question.
     Facts established 2026-09-08, so don't re-derive them:
       - **`TimedLockGuard` is used NOWHERE.** The only reference in the whole tree is a `\see also` in
         `TimeOutException.h`. LGP believes it was written with an intended use that never materialized.
       - **It has never been instantiated, and cannot be.** `TimedLockGuard.inl` has three bugs in the one
         ctor body: `d <= 0` (no such comparison for `chrono::duration<double>`), `m.try_lock_for ()` missing
         its argument, and `Exeuction::Throw` (typo). Templates aren't checked until instantiated, so these
         sat undetected - but gcc 15's `-Wtemplate-body` diagnoses uninstantiated bodies, which means
         **`TimedLockGuard.h` currently cannot be `#include`d in any gcc-15 TU**. Nothing includes it, so
         nothing breaks today; it is a live trap for the first person who tries to use it.
       - **`Execution::UniqueLock (m, d)`** (in `TimeOutException.h`) already does the same job and strictly
         dominates: the returned `unique_lock` is movable, returnable, can be released early, and is the only
         form `condition_variable` accepts. `TimedLockGuard` is the `lock_guard` analogue - saves an owns-flag,
         non-movable, scope-bound - so it is cheaper and nothing else. That is the likely reason for the disuse.
       - **If kept, its default template arg needs changing**: `FAILURE_EXCEPTION = TimeOutException`, and
         `TimeOutException` is deprecated as of 3.0d25. Defaulting to a deprecated type would warn at every use.
     So: fix the three bugs + repoint the default (and find it a use), or delete the class and its two files.
   - **Settle the `TimeOut` vs `Timeout` capitalization, as its own commit.** Split out of the 3.0d25
     exception work deliberately - it is a pure rename and does not belong in a behavior diff.
     `Timeout` is overwhelmingly the house spelling (`ThrowIfTimeout`, `ThrowTimeoutExceptionAfter`,
     `GetTimeout`/`SetTimeout`, `SetBusyTimeout`, `kDefaultTimeout`, `fPingTimeout_`, `eTimeout`, ~20 more).
     `TimeOut` survives in only four: `ThrowTimeOutException`, `TimeOutException`, `TimeOutAt`,
     `measurementTimeOut`.
     It is already causing real mistakes: **three doc comments refer to a `TimeoutException` that does not
     exist** - `IO/Network/HTTP/ClientErrorException.h:27`, `IO/Network/Transfer/ConnectionPool.h:74`, and
     `Frameworks/NetworkMonitor/Ping.h:106`. The two spellings even sit side by side in one header, where
     `TimeOutException.h` declares `ThrowTimeOutException` next to `ThrowTimeoutExceptionAfter` and
     `ThrowIfTimeout`.
     Scope note: `TimeOutException` itself is deprecated as of 3.0d25 and scheduled for removal, so renaming
     *it* is pointless - the one worth fixing is `ThrowTimeOutException` -> `ThrowTimeoutException` (plus a
     deprecated forwarder under the old name), and the file/include-guard names that follow from it.
   - **`clang++-19` is listed in Release-Notes as tested but is covered nowhere - close the gap or drop
     the claim.** Found while validating 3.0d24; deliberately left alone for that release. The
     "Compilers Tested/Supported" line says `Clang++ { unix: 15, 16, 17, 18, 19, 20, 21, 22 }`, but
     clang++-19 appears in ZERO of the 15 platform result files - the matrix jumps 18 -> 20:
       - on 24.04 it is simply not installed (`/usr/bin/clang++-1[5-8]` only), so that box tops out at 18
       - **but clang++-19 IS installed on stroika-dev-2604** (`/usr/bin/clang++-19`, verified
         2026-09-06) - so this is a config change, not an install
       - on 26.04 the clang-19 line in `Build/Scripts/MakeRegressionTestConfigurations` (~line 130) is
         commented out, together with the clang-17 and clang-18 lines - see the 3.0d24 note "clang++18
         dropped from 2604 (fails to build several third-party components); for clang < 20 on 2604, use
         libstdc++"
     So decide which it is: install clang-19 somewhere and give it a config, or uncomment the 26.04 line
     if it builds now, or remove `19` from the Release-Notes list. Cheap either way, but the list should
     not claim coverage that does not exist - that is what made the 3.0d24 validation slower to trust.

   - **medusa desktop: Chrome still software-decodes Frigate video. PARTIALLY FIXED 2026-09-05.**
     Not a Stroika issue. Everything below is measured - do not re-derive it.

     **DONE and working:**
       - `nomodeset` removed from `/etc/default/grub` (it was blocking `amdgpu` from binding; the
         module was loaded with zero users and the kernel logged no probe attempt at all). NVIDIA
         was unaffected only because `nvidia-graphics-drivers-kms.conf` sets `nvidia_drm modeset=1`
         explicitly. The AMD iGPU `79:00.0` now binds and adds a second DRM node.
       - Both GPUs now have full VA-API decode, confirmed with `vainfo`:
         NVIDIA 3090 = H264 / HEVC Main,10,12 / VP9 / AV1 via `nvidia-vaapi-driver` [NVDEC direct];
         AMD iGPU = H264 / HEVC Main,10 / VP9 / AV1 plus *encode*, via radeonsi (in-tree Mesa).
       - `nvidia-vaapi-driver` + `vainfo` installed. **On 26.04 the package is `vainfo`, NOT
         `libva-utils`** - that name no longer exists, and naming it makes apt abort the whole
         install, silently taking the other packages down with it.
       - All four `~/.config/autostart/google-chrome-*.desktop` launchers carry
         `env LIBVA_DRIVER_NAME=nvidia NVD_BACKEND=direct` plus
         `--enable-features=VaapiVideoDecodeLinuxGL --ignore-gpu-blocklist`
         (originals saved as `*.bak-vaapi`).

     **STILL BROKEN - Chrome does not actually engage VA-API.** After a reboot the flag IS on the
     command line (`ps` shows `VaapiVideoDecodeLinuxGL`), but NO `*_drv_video.so` appears in any
     chrome process's `/proc/PID/maps`, so it is still software-decoding at ~1.5 CPUs.
     **That maps check is the reliable test**, much better than reading chrome://gpu prose:
     `for p in $(pgrep -f chrome); do grep -o '[a-z0-9_]*_drv_video[.]so' /proc/$p/maps; done | sort -u`
     Next lead: Chrome's **GPU sandbox** blocking the nvidia-vaapi bridge - a known issue needing
     more than the feature flag. Check the chrome://gpu "Video Decode" line first.
     Untried alternative: move the monitor cable to a MOTHERBOARD port so the desktop runs on the
     iGPU, where radeonsi needs no bridge at all. Check iGPU headroom first, since it now also
     serves Frigate decode. Note Chrome decodes on whichever GPU drives its display; cross-device
     (decode on AMD, present on NVIDIA) is not something Chrome does cleanly.

     **Two corrections worth keeping** - the original 2026-09-04 note had both backwards:
       1. The desktop is on the **RTX 3090, NOT the AMD iGPU**. `card1` is `DRIVER=nvidia
          PCI_SLOT_NAME=0000:01:00.0` and carries every output; the monitor is on `card1-HDMI-A-3`.
       2. `modinfo amdgpu | grep 13c0` finding nothing proves NOTHING - amdgpu matches by
          **wildcard** (`pci:v00001002d*sv*sd*bc03sc00i00*` = any AMD class-0x030000 device),
          not per-device IDs.

   - **medusa: Frigate CPU cut 217% -> ~129% on 2026-09-05; remaining lever is a GPU detector.**
     Config is `/Sandbox/frigate/config/config.yaml` (**`.yaml`, not `.yml`**) and is root-owned -
     edit it through `docker exec frigate ...` rather than hunting for sudo. Backups in place:
     `config.yaml.bak-2026-09-05` (original) and `config.yaml.bak-presubstream`.
     What changed:
       - added a global `ffmpeg: hwaccel_args:` block pinned to `/dev/dri/renderD129` (the AMD
         iGPU). **Frigate's `preset-vaapi` would be WRONG here** - it defaults to renderD128, which
         on this box is the NVIDIA card. `privileged: true` already exposes both nodes and
         `radeonsi_drv_video.so` is present inside the container, so no compose change was needed.
       - split every camera's inputs: `subtype=1` (704x480) for `detect`, `subtype=0` (3-4K) for
         `record`. Previously all five fed the MAIN stream to detection - **32.1 Mpix per frame-set
         decoded just to look for objects, vs 1.69 Mpix now, about 19x less**. Recordings unaffected.
     Measured: CPU 217% -> 161% (hwaccel alone) -> ~129% settled (after substreams); memory
     5.35 -> 3.13 GiB; host load 7.31 -> 4.78; amdgpu 99% busy -> near idle.
     Still open:
       - **`frigate.detector.cpu` was the single biggest consumer at 77%.** There is **no Coral TPU
         on this box** - `lsusb` finds no Google/Global Unichip device, despite the compose passing
         `/dev/bus/usb` - so detection silently falls back to CPU. Moving it to the idle 3090 needs
         the `stable-tensorrt` image, container GPU access and a model build.
         `nvidia-container-toolkit` IS now installed (2026-09-05, from NVIDIA's repo), but
         `docker run --gpus all` still fails `Failed to initialize NVML: Unknown Error` - a cgroups
         issue to solve before any of that is worth starting.
       - `WestSoffitCamera` has a stray `detect: enabled: false` block indented *under it* (leftover
         Frigate template cruft - its comment says "until you have a working camera feed"), so
         **detection is disabled on that one camera**. Left alone deliberately; confirm intent.
       - `semantic_search` and `face_recognition` are both `model_size: large`, plus `lpr` and bird
         classification, all enabled and all running on CPU. Unmeasured, but likely most of what
         remains after the decode win.

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

