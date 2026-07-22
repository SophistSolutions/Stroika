# AGENTS.md

This file provides guidance to agents when working with code in this repository.

## What this is

Stroika is a modern, portable C++20+ application framework (a layer over the standard library):
a **Foundation** (containers, strings, streams, networking, crypto, data-interchange, etc.) plus a
series of domain-specific **Frameworks** (web server/services, auth, system performance monitoring,
UPnP, Modbus, a rich-text editing framework "Led", etc). This is the `v3-Dev` branch (unstable,
requires C++20+); `v2.1` (C++17+) and `v2.0` (C++14+) are older stable branches maintained separately.

Full narrative docs live in `Documentation/` (see especially `Design-Overview.md`, `Patterns.md`,
`Thread-Safety.md`, `Building-Stroika.md`). This file only covers what's needed for day-to-day work.

## Build system

Stroika uses a hand-rolled GNU Make + custom `configure` script build (not CMake/autotools). It is
**slow** — a full build of one configuration takes 10-20 minutes, so always pass `-j`.

### One-time setup
```bash
make check-prerequisite-tools   # verify required tools are installed
make default-configurations     # create default named configurations under ConfigurationFiles/*.xml
```
Configurations are named, XML-described build variants (compiler, flags, feature flags for optional
components like boost/openssl/sqlite/mongocxx/etc). List them with `make list-configurations` /
`make list-configuration-tags`. **Never hand-edit a `ConfigurationFiles/*.xml` file** — amend the
`configure` command line stored as the first element of the file and re-run `make apply-configurations`
(or `make CONFIGURATION=X apply-configuration`).

### Building
```bash
make all -j10                                   # build everything, all configurations
make CONFIGURATION=Debug all -j10                # build just one configuration
make CONFIGURATION=Debug libraries -j10          # just the Stroika libraries
make CONFIGURATION=Debug samples -j10            # sample apps
make TAGS=Unix all -j10                          # build all configs tagged "Unix"
```
Intermediate objects go to `IntermediateFiles/{CONFIGURATION}/`; final libs/executables to
`Builds/{CONFIGURATION}/`.

### Testing
```bash
make CONFIGURATION=Debug run-tests -j10          # build + run all regression tests for one config
make run-tests                                   # all configurations
VALGRIND=memcheck make CONFIGURATION=Debug run-tests   # under valgrind
```
Regression tests live under `Tests/<NN>/` (numbered directories, each a single `Test.cpp` +
thin `Makefile` including `Tests/Makefile-Test-Template.mk`). Each test file's purpose is documented
in a `//  TEST    <Namespace::Path>` comment near the top (e.g. `Tests/01/Test.cpp` is
`Foundation::Caching`). To build/run **one test in isolation**:
```bash
make CONFIGURATION=Debug -C Tests/07 all -j10
./Builds/Debug/Tests/Test07
```
(substitute the test number). `TEST_FAILURES_CAUSE_FAILED_MAKE=0` lets `run-tests` continue past
failures like `make -k`.

### Formatting
```bash
make format-code
```
Runs clang-format (`.clang-format` at repo root) over the codebase — always run before committing
C++ changes.

### Notes
- `make project-files` regenerates IDE project files (Visual Studio, Qt Creator); needed after
  installing a new compiler/IDE version, or run `make reconfigure` if a configuration's absolute
  compiler paths go stale.
- Docker images (`sophistsolutionsinc/stroika-buildvm-*`) are the easiest way to get a complete,
  correctly-versioned build environment; see `Documentation/Building-Stroika.md`.
- Stroika ships as a static library only, by design (see Building-Stroika.md for rationale).
- Third-party components (boost, curl, openssl, lzma, sqlite, xerces, zlib, mongo-cxx-driver, ...)
  live under `ThirdPartyComponents/` and are fetched/built automatically, or can be pointed at
  system-installed versions via configure flags/feature flags.

## Architecture

- `Library/Sources/Stroika/Foundation/` — building blocks with no dependencies outside the
  Foundation itself (besides the standard library and optional third-party components). Key areas:
  `Characters/` (Unicode `String`), `Containers/` (`Set`, `Sequence`, `Mapping`, `Stack`, ... each
  with multiple swappable backend data-structure implementations), `Streams/`, `Execution/`
  (threads, thread pools, synchronization), `IO/Network/`, `Cryptography/`, `DataExchange/`
  (`VariantValue`, JSON/XML serialization, `ObjectVariantMapper`), `Database/`, `Debug/`
  (assertions, tracing), `Cache/`, `Math/`, `Memory/`, `Time/`, `Traversal/` (iterators/ranges).
- `Library/Sources/Stroika/Frameworks/` — domain-specific libraries that depend on the Foundation:
  `WebServer/`, `WebService/`, `Auth/`, `SystemPerformance/`, `NetworkMonitor/`, `UPnP/`, `Modbus/`,
  `Led/` (rich text editing), `Service/` (OS service/daemon wrapping), `Test/` (test harness used by
  `Tests/`). Frameworks may depend on the Foundation and on each other; Foundation code never
  depends on Frameworks.
- `Samples/` — one directory per example app (Containers, Serialization, WebService, ...), each
  with its own `ReadMe.md`; good entry points for seeing idiomatic usage.
- `ScriptsLib/` — helper shell scripts used by the Makefiles/configure (e.g. `Skel`, used to
  scaffold a new Stroika-based application: `./ScriptsLib/Skel --appRoot ../myApp`).
- `Tools/`, `Workspaces/` — IDE workspace files (VSCode, Visual Studio.Net) and dev tools.

### Design conventions (see `Documentation/Design-Overview.md` and `Patterns.md` for full detail)

- **Copy-by-value semantics everywhere**, implemented internally via copy-on-write
  (`Memory::SharedByValue<T>`) for performance — e.g. `String`/`Sequence<T>` copy cheaply.
  Exceptions are explicitly named `...Ptr` (e.g. `Thread::Ptr`, `Socket::Ptr`,
  `InputStream::Ptr`) — these are reference/shared-pointer semantics to things that can't
  logically be duplicated.
- **Assertions are load-bearing documentation.** Nearly every API has `Require`/`Ensure`
  pre/post-condition assertions. They fully evaluate (and abort on failure) in Debug builds and
  compile to zero cost in Release builds — write and rely on them freely.
- **Thread safety** follows the STL convention (const methods safe for concurrent readers,
  non-const methods need exclusive access), enforced in Debug builds via
  `Debug::AssertExternallySynchronizedMutex<T>`. For `Ptr`/rep-style objects, the "envelope" vs.
  shared "rep" have independently-documented thread-safety rules — check the specific class's
  `New()` docs. Use `Execution::Synchronized<T>` to wrap shared mutable state.
- **Naming**: CamelCase (upper-case start) for Stroika-semantics methods; lowercase/STL-style
  (`empty()`, `size()`, `push_back()`, `c_str()`, ...) only when a method deliberately mirrors STL
  semantics — this is a meaningful visual cue, not inconsistency. Prefixes: `f` (field), `k`
  (constant), `e` (enumerator), `t` (thread_local), `s` (static var), `_` leading (protected),
  `_` trailing (private), `I` (concept name). Prefer prefix `++`/`--` over postfix. Prefer `using
  T = ...` over `typedef`. `New()` static methods return smart pointers, not raw allocations.
  `Parse()` static methods return `optional<T>` instead of throwing, for expected-failure parsing.
  A `Quietly` suffix variant returns `nullopt`/empty instead of throwing.
- Run `make format-code` (clang-format) before committing C++ changes; it's the only supported
  formatting workflow.
