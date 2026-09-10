# Supported Platforms and Compilers {#Supported-Platforms-And-Compilers}

What Stroika v3 is built and tested against, and where each combination is actually exercised.

"Supported" here means **something builds it and runs the regression tests against it regularly** -
either GitHub Actions, or the per-release regression runs, and preferably both. A compiler nobody
builds is not supported, however plausible it looks.

The tables further down are **generated** from the files that decide the answer, so they cannot drift
from reality:

```bash
Build/Scripts/ReportSupportedPlatforms            # regenerate
Build/Scripts/ReportSupportedPlatforms --check    # exit 1 if stale
```

The prose, the two lists below, and the caveats at the end are hand-written - regenerating leaves
them alone.

## Platforms supported

- **Linux** - Ubuntu LTS releases: **22.04**, **24.04**, **26.04**. Plus whichever non-LTS release is
  currently newest, kept at a config or two. x86_64.
- **Linux / ARM** - Raspberry Pi, via cross-compile from Ubuntu 22.04.
- **Windows 11** - Visual Studio.Net **2022** and **2026**, x86 and x86_64, under **cygwin** and
  **MSYS**, natively and in docker.
- **macOS** - arm64 (m1), **XCode 16.4** and **XCode 26.3**.
- **WSL2** - Ubuntu, tested per release (WSL1 works but is too slow to be worth testing).

Everything must be a C++20-or-later toolchain; see [Building-Stroika.md](Building-Stroika.md).

## Compilers supported

- **g++** 11, 12, 13, 14, 15, 16
- **clang++** 15, 16, 17, 18, 19, 20, 21, 22 - against *both* libstdc++ and libc++, which behave
  differently enough to matter (see the caveats)
- **Apple XCode** 16.4, 26.3
  <br/>XCode 15 was dropped in 3.0d23 along with MacOS 14 - lots of bugs, and 15.3 misreports its
  clang version, which breaks the `__clang_major__` gating Stroika's bug workarounds depend on.
- **MSVC** - Visual Studio.Net 2022 (17.x) and 2026 (18.x)

Not every combination of the above is built; the table says which are.

## Policy - which platforms earn a slot

- **Every LTS Ubuntu we reasonably can** is supported well, and keeps several g++ and clang versions.
- **The latest non-LTS Ubuntu, whatever it currently is**, earns a slot for the newer compilers that
  are awkward to get elsewhere, plus a basic check that the newest Ubuntu still works. When the next
  interim ships, that coverage moves to it and the outgoing one keeps a config or two to confirm it
  still functions. This comes due periodically; it is routine maintenance, not a per-release task.
- **Watch for a compiler with only one home.** CI and the release regression runs cover different
  sets, so it is easy for a compiler to be claimed as supported while living in exactly one
  commented-out line. The table's two coverage columns are there to make that visible at a glance.

<!-- BEGIN GENERATED TABLES - regenerate with Build/Scripts/ReportSupportedPlatforms -->

### Compiler / platform combinations

Which compiler is built against which standard library, and where each combination is actually exercised. `always` runs on every push; `v3-Release` runs on the release branch and on a manually forced all-configurations build; `parked` is defined but disabled.

| compiler | stdlib | GitHub Actions | release regression runs | notes |
|---|---|---|---|---|
| `g++ (distro default)` | libstdc++ | - | 26.04 |  |
| `g++-11` | libstdc++ | 22.04 v3-Release | 22.04 | [^f] |
| `g++-12` | libstdc++ | 22.04 **always** | 22.04, 24.04, 26.04 | [^f] |
| `g++-13` | libstdc++ | 24.04 v3-Release, 26.04 v3-Release | 22.04, 24.04, 26.04 |  |
| `g++-14` | libstdc++ | 24.04 **always**, 26.04 v3-Release | 24.04, 26.04 |  |
| `g++-15` | libstdc++ | 25.04 v3-Release, 26.04 v3-Release | 26.04 |  |
| `g++-16` | libstdc++ | 26.04 **always** | 26.04 |  |
| `clang++ (distro default)` | libstdc++ | - | 26.04 |  |
| `clang++-15` | libc++ | 22.04 v3-Release | 22.04 | [^f] [^j] |
| `clang++-15` | libstdc++ | 22.04 v3-Release | 22.04 | [^f] |
| `clang++-16` | libc++ | 24.04 v3-Release | 24.04 | [^j] |
| `clang++-16` | libstdc++ | - | 24.04 |  |
| `clang++-17` | libc++ | 24.04 v3-Release | 24.04, 26.04 (parked) | [^j] |
| `clang++-17` | libstdc++ | - | 24.04 |  |
| `clang++-18` | libc++ | 24.04 v3-Release | 24.04, 26.04 (parked) | [^j] |
| `clang++-18` | libstdc++ | - | 24.04, 26.04 (parked) |  |
| `clang++-19` | libstdc++ | 25.04 parked, 26.04 v3-Release | 26.04 |  |
| `clang++-20` | libc++ | 26.04 v3-Release | - |  |
| `clang++-20` | libstdc++ | 25.04 v3-Release | 26.04 |  |
| `clang++-21` | libc++ | 26.04 v3-Release | - |  |
| `clang++-21` | libstdc++ | - | 26.04 |  |
| `clang++-22` | libc++ | - | 26.04 (parked) |  |
| `clang++-22` | libstdc++ | 26.04 **always** | 26.04 |  |
| `XCode 16.4` | libc++ | macos-15 **always** | - | [^j] |
| `XCode 26.3` | libc++ | macos-26 **always** | MacOS_XCode26_m1 |  |
| `MSVC (VS2k22)` | MSVC STL | windows **always** | Windows_VS2k22 |  |
| `MSVC (VS2k26)` | MSVC STL | windows **always** | Windows_VS2k26 |  |
| `arm-linux-gnueabihf-g++-11` | libstdc++ | - | raspberrypi (cross) | [^f] |
| `arm-linux-gnueabihf-g++-12` | libstdc++ | - | raspberrypi (cross) | [^f] |
| `arm-linux-gnueabihf-g++-13` | libstdc++ | - | raspberrypi (cross) |  |

### Release regression-run targets

Run by hand for each release (see [Regression-Tests.md](Regression-Tests.md)); these are whole-platform runs, each building many configurations.

- `MacOS_XCode26_m1`
- `Windows_VS2k22`
- `Windows_VS2k26`
- `Ubuntu2204_x86_64`
- `Ubuntu2404_x86_64`
- `Ubuntu2504_x86_64`
- `Ubuntu2604_x86_64`
- `Ubuntu2204-Cross-Compile2RaspberryPi`

### Notes

[^f]: requires the **fmtlib** polyfill - no usable `<format>` (see `NeedsFmtLib_ ()` in `configure`)

[^j]: **no `std::jthread`/`stop_token`** - `Thread::Abort ()` cannot `request_stop ()`, so only the POSIX interrupt signal / Windows APC breaks into a blocking call, and code that waits has to fall back on lesser mechanisms (see `Execution/Thread.h`)

<!-- END GENERATED TABLES -->

## Caveats worth knowing before picking a toolchain

Beyond the per-row notes above:

- **libc++ and libstdc++ are not interchangeable for threading.** libc++ implemented
  `jthread`/`stop_token` in LLVM 18 but kept it behind `-fexperimental-library` until LLVM 20, and
  Stroika passes that flag nowhere - so every `--stdlib libc++` build with clang < 20 lacks it, as do
  XCode 15 and 16 (measured 2026-09-10). Where it is missing, `Thread::Abort ()` cannot call
  `request_stop ()`, and only the POSIX interrupt signal or Windows APC breaks into a blocking call.
- **`std::mutex` speed varies a lot by standard library**, which is why
  `Execution::kSpinLock_IsFasterThan_mutex` keys off the std-library macros rather than the compiler.
  See the measured table in `Execution/SpinLock.h`.
- **Ubuntu 26.04 has no Release clang configuration.** Both were tried 2026-09-09: they configure and
  build the library fine, but every test fails to link. That is a link-line/LTO problem, so the
  clang optimizer and LTO are untested on 26.04. See the comment in
  `Build/Scripts/MakeRegressionTestConfigurations`.
- **Sanitizer and valgrind coverage is narrower than the compiler coverage above** - every sanitizer
  configuration is a `g++` one, and GitHub Actions runs no sanitizer or valgrind job at all. Dynamic
  analysis happens in the per-release regression runs.
- **Third-party components are optional** and can be turned off or pointed at system copies per
  configuration, so "supported" for a compiler does not imply every optional component builds with
  it. `fmtlib` is the exception - it is a polyfill, pulled in automatically where `<format>` is
  unusable rather than being a choice.

## See also

- [Building-Stroika.md](Building-Stroika.md) - how to build, and the `configure` script
- [Regression-Tests.md](Regression-Tests.md) - how the per-release runs are driven
- [Release-Notes.md](../Release-Notes.md) - per-release history of what was tested at the time
