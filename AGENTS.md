# AGENTS.md

This file provides guidance to agents when working with code in this repository.

@TODO.md

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
make all -j8                            # build everything, all configurations
make CONFIGURATION=Debug all -j8        # build just one configuration
make CONFIGURATION=Debug libraries -j8  # just the Stroika libraries
make CONFIGURATION=Debug samples -j8    # sample apps
make TAGS=Unix all -j8                  # build all configs tagged "Unix"
```
Intermediate objects go to `IntermediateFiles/{CONFIGURATION}/`; final libs/executables to
`Builds/{CONFIGURATION}/`.

### Testing
```bash
make CONFIGURATION=Debug run-tests -j8                 # build + run all regression tests for one config
make run-tests                                         # all configurations
VALGRIND=memcheck make CONFIGURATION=Release run-tests # under valgrind - a Release build; Debug takes hours (Documentation/Debugging.md)
```
Regression tests live under `Tests/<NN>/` (numbered directories, each a single `Test.cpp` +
thin `Makefile` including `Tests/Makefile-Test-Template.mk`). Each test file's purpose is documented
in a `//  TEST    <Namespace::Path>` comment near the top (e.g. `Tests/01/Test.cpp` is
`Foundation::Caching`). To build/run **one test in isolation**:
```bash
make CONFIGURATION=Debug -C Tests/07 all -j8
./Builds/Debug/Tests/Test07
```
(substitute the test number). `TEST_FAILURES_CAUSE_FAILED_MAKE=0` lets `run-tests` continue past
failures like `make -k`.

Note that a header edit alone will NOT rebuild a test object (see the header-dependency note below),
so to recompile one test against a changed header, delete its object first:
`rm IntermediateFiles/Debug/Tests/07/Test.obj`. This is the cheap way to compile-check a header change
without the ~12 minute `library-clobber` cycle.

**Bug fixes are test-first**: write the regression test and watch it FAIL on the unchanged code, then fix - a test
written after the fix only proves the fixed code does what it does. Choose assertions that observe what was actually
broken (a defect that shows only walking backward needs a test that walks backward).

Reading results:
- Build `Debug` when the only question is "does it compile"; Release only when timing matters (Tests/52).
- Report warnings as well as errors for every verification build, even a one-test rebuild: the builds are otherwise
  warning-clean, so any warning is new.
- `run-tests`' exit code proves nothing (`TEST_FAILURES_CAUSE_FAILED_MAKE=0`), and gtest's `[  FAILED  ]` misses harness
  deaths (`FAILED: SIGNAL= SIGSEGV`, `FAILED: std::terminate () called`), tests that never ran (a loader error, a failed
  copy) and crashes before the test framework starts (no output at all). Count each executable's `[  PASSED  ]` line
  against the tests run; zero PASSED is never a pass.
- `./configure --no-third-party-components` also drops googletest, and a test built without it is a STUB: it prints
  `[  PASSED  ]` and exits 0, having run nothing. For a quicker scratch configuration drop the slow components instead
  (`--mongo-cxx-driver no`, perhaps `--OpenSSL no`), or add `--googletest use` back.
- To chase a failure in one named configuration, first run that build tree's existing `Builds/<CONFIG>/Tests/TestNN`
  binaries, then compile a one-file probe with its exact flags (`make -n -B` in `Tests/NN` shows the recipe; pkg-config
  under `Builds/<CONFIG>/lib/pkgconfig` gives the flags) - rather than rebuilding Stroika.

### Continuous integration
`.github/workflows/build-N-test.yml` builds and runs the regression suite on push, across Linux
(several gcc versions), Windows (VS2022/VS2026 × cygwin/msys), and macOS/XCode — so it covers
compilers no single dev box has. **The repo is public, so results are readable with no token and no
`gh` install**, just curl:
```bash
curl -s "https://api.github.com/repos/SophistSolutions/Stroika/actions/runs?branch=v3-Dev&per_page=1"
curl -s "https://api.github.com/repos/SophistSolutions/Stroika/actions/runs/<id>/jobs"
```
Two things to check before concluding a change is covered:
- `build-N-test-Matrix.json` gates most entries with `"run_on_branch": "v3-Release"`, so a push to
  `v3-Dev` runs only the subset marked `"always"`. Read the job list, don't assume the matrix.
- Compare the run's `head_sha` against local `HEAD` — unpushed commits have obviously not been tested,
  and that is the common case mid-session.

That public access covers run/job *status* only. **Logs and artifacts need `actions:read`** and return
401/403 unauthenticated; the credential is usually already on the dev box in Git Credential Manager, so
there is no token file to find and nothing to paste:
```bash
CFG=$(mktemp)   # --config so the token never reaches a command line or the process list
printf 'header = "Authorization: Bearer %s"\n' \
  "$(printf 'protocol=https\nhost=github.com\n\n' | git credential fill | sed -n 's/^password=//p')" > "$CFG"
curl -sL --config "$CFG" ".../actions/jobs/<job_id>/logs" -o out.log
curl -sL --config "$CFG" ".../actions/runs/<run_id>/artifacts"     # then archive_download_url
rm -f "$CFG"
```
For a CRASH the job log is nearly useless — it prints only `FAILED: SIGNAL= SIGSEGV`. The
`Log Data (<job>)` artifact is what you want: CI configures `--trace2file enable`, so it holds a
per-test `tmp/TraceLog_TestNN_PID#….txt` whose tail names the last function entered.

**Read every job before calling a failure toolchain-specific.** The matrices run with
`fail-fast: false` (since ee57c38953), so one failing job no longer cancels its siblings - their
results are there to compare, and a job that WAS cancelled (e.g. by a newer push) is still not a
passing job. The OS matters as much as the compiler version: an Aug 2026 miscompile hit ubuntu-24.04's
g++-14 while ubuntu-26.04's g++-14 was clean.

This is NOT the only place regression tests are run, just the most frequent and the easiest to reach;
a green run here is good evidence, not proof of full coverage.

#### Which platforms and compilers get tested

The current matrix lives in `Documentation/SupportedPlatformsAndCompilers.md`, whose tables are
GENERATED by `Build/Scripts/ReportSupportedPlatforms` from the two files below (`--check` fails if
they are stale). Read it rather than reconstructing coverage by hand, and regenerate after changing
either source.

Standing policy, applied to both `build-N-test-Matrix.json` and
`Build/Scripts/MakeRegressionTestConfigurations`:

- **Support every LTS Ubuntu we reasonably can** (22.04, 24.04, 26.04 as of late 2026). These carry
  the bulk of the coverage, and each keeps several g++ and clang versions.
- **Support the latest non-LTS release, whatever it currently is** - its value is the newer compiler
  versions that are awkward to get elsewhere, plus a basic check that the newest Ubuntu still works.
  An interim release earns its slot only while it is the newest; when the next one ships the coverage
  moves to it, and the outgoing interim keeps just a config or two to confirm it still functions.
  This comes due periodically - normal maintenance, not a per-release task.
- **A compiler with only ONE home is the thing to watch for.** CI and the medusa regression runs
  cover different sets, so it is easy for a compiler to be claimed under "Compilers Tested/Supported"
  in Release-Notes while living in exactly one commented-out line. Cross-check both lists against
  that claim, not against each other.

Matrix mechanics worth knowing before editing it:

- `"run_on_branch"` takes `"always"`, a branch name (usually `"v3-Release"`), or `"never"`.
  **`"never"` really does mean never** - the jq in `build-N-test.yml` reads
  `A or B or (run_all and not-never)`, so even a manually forced `run_all` build skips it. Its use is
  to PARK an entry: kept as a template for the next platform, costing nothing.
- `"always"` entries run on every push, so keep that set small - about one canary per platform,
  favouring the newest compiler on the newest LTS, since that is where new-standard and
  new-diagnostic breakage surfaces first.

### Formatting
```bash
make format-code
```
Runs clang-format (`.clang-format` at repo root) over the codebase — always run before committing
C++ changes. It needs clang-format on `PATH`; on Windows that means the VS LLVM directory
(`export PATH="$PATH:/cygdrive/c/Program Files/Microsoft Visual Studio/<VER>/Community/VC/Tools/Llvm/x64/bin"`),
and the target prints the exact line to add if it can't find it.

**Use the clang-format from the NEWEST installed Visual Studio, and check which one you got.** The
tree is currently formatted with the VS2026 one — which installs as
`Microsoft Visual Studio/18/`, note the bare version number, not the year — and reports
`clang-format version 22.1.3`. Running an older one (VS2022 ships 19.1.5) silently reformats the tree
*backwards*: it "fixed" ~65 files nobody had touched, which is easy to miss in a big diff and
miserable to unpick afterwards. `clang-format --version` before a whole-tree sweep is worth the two
seconds.

**`Build/Scripts/FormatCode` takes a directory then filenames**, so you can format only what you
changed rather than sitting through the ~20 minute whole-tree run:
```bash
Build/Scripts/FormatCode Tests/52 Test.cpp
Build/Scripts/FormatCode Library/Sources/Stroika/Foundation/Containers/Concrete Collection_Array.inl Collection_LinkedList.inl
```
One directory per invocation — a second dir/file pair in the same call is silently treated as more
filenames in the first directory.

**Stray `SomeFile.cpp.tmp` files next to real sources are format-code debris, not yours and not a
mistake — just delete them.** `Build/Scripts/FormatCode` formats through a sibling temp file: for
each source it writes `FILE.tmp`, then either `rm`s it (output identical, nothing to do) or `mv`s it
over `FILE` (file needed reformatting).

So while a run is in progress you will see `.tmp` files blink in and out, one per file being
processed — that is the script working, not an error. Do not try to interpret one you catch
mid-flight: it may be half-written, so comparing it against its source proves nothing. Only a `.tmp`
still present *after* the run has finished is an orphan, meaning the cleanup step never ran — the
`expand | clang-format` pipeline failed (the `&&` then skips cleanup), the script was interrupted,
or the `rm` lost a race with something holding the file open, which on Windows is routinely an
indexer or the IDE.

They are inert — nothing compiles them, since the build's only compile rule is
`$(ObjDir)%${OBJ_SUFFIX} : %.cpp` and `foo.cpp.tmp` does not match `%.cpp`. But they are NOT
gitignored, so they surface as untracked files and `git add -A` will happily commit one. Check
`git status` for `.tmp` before committing.

### Notes
- **A Windows build that stops dead is probably the MSYS2 bug, not your change.** `msys2-runtime`
  3.6.10 regressed console handling and parallel builds hang at any stage and never recover. Tell it
  apart by what is MISSING: no `cl.exe` running, nothing new under `IntermediateFiles/` for minutes,
  while `make.exe`/`sh.exe` persist. CPU proves nothing (0% or a ~2% spin, both seen) and Ctrl+C will
  not break it. Kill that build's process tree by PID (`taskkill /F /T /PID <its make.exe>`, then any
  `sh.exe` it left) - never by image name, which kills every other build and shell on the box too - and
  re-run; make resumes where it stopped, losing nothing. To avoid it, keep stdout/stderr off a console:
  `set -o pipefail; make ... 2>&1 | tee build.txt`. `make check-prerequisite-tools` warns on an
  affected runtime; 3.6.9-2 is the last good one. Do NOT spend time bisecting Stroika for this.
  @see https://github.com/SophistSolutions/Stroika/issues/1169
- `make project-files` regenerates IDE project files (Visual Studio, VS Code); needed after
  installing a new compiler/IDE version, or run `make reconfigure` if a configuration's absolute
  compiler paths go stale.
- **VS Code over Remote-SSH to a Windows host: open the folder with its drive letter**
  (`/c:/Sandbox/Stroika/DevRoot`, not `/Sandbox/Stroika/DevRoot`). The Recent list can hold both and
  they look nearly identical. VS Code passes the workspace folder as each task’s `cwd`, so without
  the drive letter every task shell spawns into a nonexistent directory and exits instantly - no
  output, no error anywhere in the UI. Interactive terminals fall back on a bad cwd and work fine,
  so **"terminals work but tasks do nothing" means suspect the cwd.**
- **A HEADER CHANGE NEVER TRIGGERS A REBUILD. After editing any `.h`/`.inl`, you must
  `make CONFIGURATION=X library-clobber` before the build means anything.** There is no header
  dependency tracking: the build generates no `.d` files, and the compile rule is
  `$(ObjDir)%.obj : %.cpp` — an object depends on its source and nothing else. So editing a header
  leaves every library object looking up-to-date, `make libraries` returns 0 in seconds having
  rebuilt nothing, and the tests then link fresh test objects against a library built from the OLD
  headers. That passes, and proves nothing. Since Stroika is mostly header templates, this affects
  most changes to it.
  `QUICK_BUILD=0` does NOT help here, despite sounding like it should: it forces make to *check*
  staleness rather than skip the check, but staleness is computed only against the `.cpp`, so a
  header edit is invisible either way. (`QUICK_BUILD=1` is the default for
  `make CONFIGURATION=X libraries`, and skips even that check when the library files already exist;
  `make run-tests` inherits the same shortcut.)
  `library-clobber` deletes everything except the third-party products, which are slow to rebuild
  and aren't part of Stroika anyway — so it is the cheap way to get a *trustworthy* rebuild.
  There IS a fast path when you know exactly which `.cpp` instantiates the template you edited, and
  it needs BOTH halves — deleting the object alone is not enough, because `QUICK_BUILD=1` skips even
  looking:
  ```bash
  find IntermediateFiles/$CONFIG -name 'TheOne.o' -delete
  QUICK_BUILD=0 make CONFIGURATION=$CONFIG libraries      # without QUICK_BUILD=0: "libraries exist", builds nothing
  ```
  Verify it did something — `grep -c Compiling` on the output. A rebuild that compiled 0 files after a
  header edit means you are still testing the old code.
- **`cached-list-objs` will lie to you, in both directions.** It is a generated list of the objects
  that go into the library archive, at
  `IntermediateFiles/$(CONFIGURATION)/Library/Foundation/cached-list-objs`:
    - **Adding a new Foundation `.cpp` requires deleting it.** Otherwise the new file compiles happily
      but never enters the archive, and the failure shows up as an unresolved external when some
      *test* links — nowhere near the library build that actually went wrong.
    - **After `library-clobber` it makes the next build print a convincing FALSE failure.** Early in
      the log you get `Makefile:74: *** open: .../cached-list-objs: No such file or directory. Stop.`
      followed by `make: *** [Makefile:216: libraries] Error 2`. Clobber deleted the file and
      something reads it before it is regenerated; make then regenerates it and the build completes
      normally. Trust the exit status and the built artifacts, not the `Error 2` — reading the log
      text alone says the build failed when it did not.
- **Debugging an optimized build: do not trust a debugger's variable values.** In `Release` (`-O2`,
  and LTO on the Linux configs) gdb will cheerfully print a stale register or the wrong stack slot for
  a local, with no indication it is guessing — it sent a real investigation down three dead ends in
  Aug 2026. Make the *program* report its own state instead: an `fprintf` inside the branch you are
  suspicious of cannot be hoisted across it, so what it prints is what actually happened. `.lto_priv`
  and `[clone .constprop.N]` in a backtrace are the tell that frames and inlining have been rearranged
  and the line attribution is approximate.
- Docker images (`sophistsolutionsinc/stroika-buildvm-*`) are the easiest way to get a complete,
  correctly-versioned build environment; see `Documentation/Building-Stroika.md`.
- Stroika ships as a static library only, by design (see Building-Stroika.md for rationale).
- Third-party components (boost, curl, openssl, lzma, sqlite, xerces, zlib, mongo-cxx-driver, ...)
  live under `ThirdPartyComponents/` and are fetched/built automatically, or can be pointed at
  system-installed versions via configure flags/feature flags.
- **One `make` per tree at a time.** Two configurations built at once in one tree race on the shared
  `ThirdPartyComponents/<name>/CURRENT` (lzma and sqlite re-extract every build): one run fails, or builds from a
  half-extracted source. `-jN` within one build is fine; for configurations in parallel, use separate trees.
- **Windows shells.** Git Bash has no `make`; MSYS2 and Cygwin both do, and `~` is a different directory in each. A build
  run through `bash.exe -lc` from another shell starts with `PATHEXT` empty, so Stroika's `FindExecutableInPath` finds no
  `.exe` and Tests/38 fails: `export PATHEXT=".COM;.EXE;.BAT;.CMD"` first. A fresh `./configure` also needs `COMSPEC` and
  `ProgramFiles(x86)` set, or vcvarsall silently runs nothing; the second is not a valid shell name, so pass it with
  `env "ProgramFiles(x86)=C:\Program Files (x86)" ./configure ...`. Python is installed even when `python3` says it is
  not (that is the Microsoft Store alias): use `py`.
- **A hung test inside a release regression container** cannot be attached from inside (no `CAP_SYS_PTRACE`). Attach
  from a sibling container in its PID namespace: `docker run --rm -i --pid=container:<regtest> --cap-add=SYS_PTRACE
  --security-opt seccomp=unconfined --user root -v /Sandbox:/Sandbox <same image> gdb -batch -ex "set sysroot
  /proc/<pid>/root" -ex "attach <pid>" -ex "thread apply all bt" <test binary>` - the `set sysroot` is what gives
  library frames. Copy the binary out first: the next run deletes the build directory.

### Shared dev boxes

The dev boxes run LGP's long builds and release regression runs (one platform takes ~10 hours), often unattended:
- **Never disturb a running build.** Do not build, configure, clobber or delete in a tree that may be mid-run unless asked
  for that very action; read its logs rather than walking `IntermediateFiles/` or `Builds/`. `.claude/` and your own
  scratch are fine to change. A request that seems to need disturbing a build is probably a misunderstanding - ask.
- **Never kill processes by image name** (`taskkill /IM make.exe`, `pkill bash`, `killall make`): LGP's shells and builds
  are the same images. Stop your own by the PID you started, or a `pkill -f` pattern unique to your command.
- **Keep scratch in one directory of your own** (eg `/Sandbox/claude/`), check `df -h` before building several
  configurations - the disk is shared, and has been filled - and delete scratch configurations (`Builds/<cfg>`,
  `IntermediateFiles/<cfg>`, `ConfigurationFiles/<cfg>.xml`) as soon as you are done with them.

## Commits

**Write the commit message to `.claude/COMMIT_MSG.txt` and hand it over for review - do not run the
commit yourself** (unless explicitly asked to). LGP edits the message and commits. After writing the
file, open it in the editor if you can, and always give a clickable link to it rather than only
describing what it says. `.claude/` is gitignored, so the file never lands in a commit.

**Carry proto upgrade notes in the commit message.** Anything downstream code would have to change,
or would want to know about - applications built on Stroika, especially ones scaffolded with `Skel`,
which carry hand-copied forks of pieces of the build - goes in the commit message under an
`UPGRADE NOTE:` heading. At release time (v3-Dev merges back to v3-Release, roughly monthly) these
are gathered and combined into that release’s `#### Upgrade Notes (X to Y)` section in
`Release-Notes.md`. Writing them at commit time is the only thing that makes this work: a month
later nobody can reconstruct which of ~100 commits had downstream impact.

**One commit at a time.** `.claude/COMMIT_MSG.txt` holds exactly ONE message, and the index holds
exactly the files that message describes - so the whole thing is just:

```bash
git commit -F .claude/COMMIT_MSG.txt
```

with no pathspec to get wrong and nothing to review twice. When a session's work naturally splits
into several commits, write the rest to `.claude/COMMIT_MSG-remaining.txt` (numbered, each with the
`git add` line it needs) and promote them into `COMMIT_MSG.txt` one at a time as each is committed.
Staging the files is the agent's job, not LGP's.

Two things that quietly break that on Windows:

- **`core.filemode` is false**, so a newly added script commits as 100644 and loses its executable
  bit - which breaks the build on UNIX while still appearing to work here (MSYS/Cygwin fake the
  mode). Stage new files under `Build/Scripts/` with
  `git update-index --add --chmod=+x <path>` rather than `git add`, and confirm with
  `git ls-files -s <path>` showing 100755.
- **`git reset` throws that away.** If a staged script gets unstaged for any reason, the `--chmod`
  has to be redone - `git add` alone will not bring it back.

**Keep messages terse.** A cleanup is a title alone; a fix is a title plus 2-4 lines of what was wrong. Verification,
measurements and why-not-X go in the chat reply, which LGP reads - he cuts them from the log. The exception is a breaking
change: say what changed and who is affected, plus the `UPGRADE NOTE:`. A commit touching only TODO.md is just "todo".
Re-read this before each message in a long session - drafts drift long.

**To set one file's edits aside while a commit is staged, save a patch** (`git diff -- F > F.patch && git checkout -- F`,
later `git apply F.patch`) - not `git stash push -- F`, which still records the whole index, and on pop re-applies the
old staged versions of the commit's files.

## Issues

- **Priority is a field, never a label** - create no labels (applying existing ones is fine). There are TWO priority
  fields, the "Stroika Issues" project's (#1) and the organization's issue field; setting one does not set the other, so
  set both (`gh api graphql`, from a POSIX shell - PowerShell mangles the quoting).
- **TODO.md outranks any ticket priority**: an entry there - even one that only points at a ticket - is a priority marker.
  Never delete one unless LGP says so. In a triage, every recommendation is NOW (before the release), TICKET, or PROCESS (a
  mechanical trigger) - never "wait for X".
- **When a commit is about an issue, comment on the issue with the commit link** once it is pushed, saying exactly what it
  fixes. A real fix on v3-Dev is closed by hand ("Fixed in <next release>") - GitHub's `fixes #N` fires only on the
  default branch.
- In commit messages and issue comments write `#1165`, which GitHub links; full URLs only where nothing links them.

## Downstream projects

These applications are built on Stroika. Clone and search them freely - they are the best evidence
of what a change would break downstream, so check them before deleting or renaming a public macro
or API, and when deciding whether a commit needs an `UPGRADE NOTE:`.

- https://github.com/SophistSolutions/WhyTheFuckIsMyNetworkSoSlow ("WTF")
- https://github.com/SophistSolutions/IPAM-Root
- https://github.com/SophistSolutions/HearHE
- https://github.com/Records-For-Living-Inc/AskHealthFrame - **private**, so it may not be
  accessible (it is with LGP's GitHub credentials), and part of its C++ is in a private
  `HealthRecordModel` submodule. Never quote its code in anything public - GitHub issues, commit
  messages.

Each keeps its own Stroika in `ThirdPartyComponents/Stroika/StroikaRoot`, so exclude that when
searching, or you are only searching Stroika again. A shallow clone of `v1-Dev` (the newest branch
in all four) is enough: `git clone --depth 1 --branch v1-Dev <url>`. Clone fresh rather than
reusing a checkout found on a dev box - those go stale, and one was 8 months behind when this was
written.

A search answers "does anything use X"; only building against the changed Stroika answers "does it
still compile". Say which one you did.


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
- `Build/` — everything the build system itself needs, divided by *role* (how a file is consumed),
  not by topic:
  - `Build/Scripts/` — standalone programs invoked directly by the Makefiles and `configure`
    (e.g. `Skel`, used to scaffold a new Stroika-based application:
    `./Build/Scripts/Skel --appRoot ../myApp`). These are run as commands, not as `bash X`, so they
    must keep their executable bit — a lost `chmod +x` breaks the build on UNIX while still
    appearing to work on Windows (MSYS/Cygwin fake the mode).
  - `Build/Lib/` — libraries pulled *into* other build files and never executed: `Make/` holds the
    `.mk` fragments (`SharedMakeVariables-Default.mk` for variables, then
    `SharedBuildRules-Default.mk` for rules — that include order is required), `Perl/` holds the
    `.pl` files `require`d by `configure` and the scripts.
  - `Build/Shared/` — data read at build time rather than code: `Skel-Templates/`, the app skeletons
    `Skel` copies and substitutes into.
  - `Build/Tools/Src/` — source for host utilities the build needs *before* Stroika exists, and which
    therefore cannot use it: `realpath.cpp` (a GNU-`realpath` stand-in, for macOS; the top-level
    `Makefile` compiles it ad hoc) and `vswhere/` (fetches Microsoft's Visual Studio locator; nothing
    runs it - `make -C Build/Tools/Src/vswhere` by hand). Neither is part of the normal build.
    **Not to be confused with top-level `Tools/`** — see below.
  - `Build/Docker/` — build-VM container definitions (see `Documentation/Building-Stroika.md`).
  A deprecated top-level `ScriptsLib/` still exists purely to shim the pre-3.0d24 layout: each entry
  warns and forwards to its new home. Don't add to it, and don't reference it from new code.
- `Tools/` — tools built *with* Stroika, by the normal build, into `Builds/{CONFIGURATION}/bin/`;
  they depend on the Foundation/Frameworks and mirror `Library/`'s layout
  (`Tools/Sources/Stroika/Frameworks/...`, plus per-VS-version projects under `Tools/Projects/`).
  Currently `HTMLViewCompiler`, which `SharedBuildRules-Default.mk` invokes to compile `.swsp` files.
  The dividing line versus `Build/Tools/Src/`: **can it use Stroika?** If yes it belongs here; if it
  has to run before Stroika can be built, it belongs under `Build/`.
- `Tests/Scripts/` — helpers specific to the regression-test harness (test naming/listing).
- `Workspaces/` — IDE workspace/solution files (VSCode, Visual Studio.Net).

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
  `_` trailing (private), `I` (concept name), `q` (a macro naming a value - usable in `#if`, unlike a
  `k` constant; see Design-Overview.md "'q' versus 'k'", and "Macro names" for how a macro is named - `qStroika_`
  prefix, scope, the `qStroika_Platform_*` family, rc.exe's 31-character limit). Prefer prefix `++`/`--` over postfix. Prefer `using
  T = ...` over `typedef`. `New()` static methods return smart pointers, not raw allocations.
  `Parse()` static methods return `optional<T>` instead of throwing, for expected-failure parsing.
  A `Quietly` suffix variant returns `nullopt`/empty instead of throwing.
- **`Memory::MakeSharedPtr`, not `make_shared`**: the same, except that it block-allocates a type annotated for it (a
  `Memory::UseBlockAllocationIfAppropriate<T>` base) - so use it even where the type is not annotated. The annotation suits
  a small rep allocated often, making that indirection nearly free; elsewhere it hardly matters either way.
- **Document every overload, with a trailing `///<`.** Doxygen and the VS Code C/C++ extension both
  attach a comment to the *immediately following* declaration only, so one block above a group of
  overloads documents the first and leaves the rest with nothing - and the common
  ```
        /**
         */
        Foo (error_code);
        Foo (error_code, const String&);
  ```
  gives both tools nothing, for all of them. Put the full explanation on the first declaration and a
  trailing one-liner on each sibling, which costs no extra lines and hovers correctly (measured
  2026-09-22 against the extension; clang-format aligns them):
  ```
    bool IsA (const error_code& ec, error_condition cond) noexcept;   // ... full /** */ block above
    bool IsA (const system_error& e, error_condition cond) noexcept;  ///< \brief Does this error MEAN the given condition?
  ```
  The trailing `///<` is for overloads only - declarations of one name. A function of another name, however close
  (`RemoveCallback` right after `AddCallback`), gets its own `/** */` block.
  Keep `\brief` explicit - `JAVADOC_AUTOBRIEF` is NO in `Documentation/Doxygen/Stroika-Library.cfg`.
  Do NOT use `@copydoc` for this: doxygen expands it, the extension does not, so hover shows the raw
  `@copydoc ...` text. `@see` is dropped by the extension entirely. Apply this opportunistically when
  touching a header; a whole-tree sweep would bury real changes.
- **`not` / `or` / `and`, not `!` / `||` / `&&`** - in expressions and in `#if` (`#if not defined(X)`), on lines you
  write or touch, even when the neighbouring line uses the symbol. Not a sweep.
- **Platform `#if` / `#elif` chains in alphabetical order** (Linux, MacOS, ..., Windows); insert a new platform in order.
- **C++ source stays ASCII outside comments - use `\u` escapes in literals.** The MSVC builds pass no `/utf-8`, so a raw
  non-ASCII character is read as cp1252 and the test fails only on Windows. Check new lines with
  `git diff | grep -P '^\+.*[^\x00-\x7F]'`.
- **String literals**: narrow where one initializes a `String` (`"..."sv` in library code) - but keep `L"..."_f`: a wide
  format literal is CHEAPER than a narrow one, which is widened at run time. Leave `\u`/`\x` escapes, and genuinely wide
  targets (`wchar_t` APIs, `EnumName` tables), alone. Opportunistic, not a sweep.
- **Comments state the invariant, not today's callers** - a reason anchored to a call site rots when it changes - and
  describe cost qualitatively ("cheap"), not with measurements ("~1ns"), which date. Numbers go in commits or issues.
- **A note that looks back names the version** - "before Stroika v3.0d25 it was an Execution::Function", not "it used to
  be" - in a `\note` and an `UPGRADE NOTE:` alike. That is the release the change first ships in: while `STROIKA_VERSION`
  says `3.0d25x`, the work is toward 3.0d25.
- **Keep writing code comments**: one LGP deletes still helped at review, so it is not criticism. The ones he keeps are
  constraints and measured negative results - what stops a future reader's wrong "simplification".
- **Claims that a tool lacks something carry a version**: "MSVC has no X, as of 19.51". And before changing a convention
  for a `-std=c++26` quirk, check the paper's status: draft features get withdrawn (P4144 removed `span` from an
  `initializer_list`).
- Run `make format-code` (clang-format) before committing C++ changes; it's the only supported
  formatting workflow.

### Changing things

- **Deleting vs deprecating.** Dead code nothing downstream could use: delete it. A public name an app plausibly uses:
  mark it DEPRECATED, stop using it, and delete it at the end of the `d` stage. Anything already DEPRECATED or OBSOLETE -
  including everything in a deprecated directory such as `Foundation/Configuration/` - stays until the 3.0a1 cleanup,
  which also removes the warning suppressions guarding its uses. A renamed macro keeps its old name as a documented
  `#define OLD NEW` alias, plus an `UPGRADE NOTE:`. Deprecated means "still compiles and warns", not "still behaves the
  same": build no shims to keep old behaviour. In analysis, don't dwell on deprecated APIs, and never invest in one - no
  renames, fixes or forwarders.
- **A workaround carries its own re-test trigger.** A disabled configuration's line names the issue URL; a third-party
  workaround is version-gated (`Build/Scripts/VersionCompare`) so the next bump drops it, with the exact error text in
  its comment. Before removing one, test the OLDEST supported toolchain its history names - for clang both stdlibs, and
  the fmtlib path (no `<format>`) - and build the Samples too, not just the libraries; MSVC has two supported versions.
- **Warnings**: removing a suppression that provably suppresses nothing is welcome. `-Wno-switch`, `-Wno-sign-compare` and
  `-Wno-unused-function` stay on purpose (2026-10-05); never silence sign-compare with a cast - `std::cmp_less` fixes it.
- **Kept on purpose, though nothing calls them**: toolbox scripts such as `Build/Scripts/BuildGCC`, `BuildClang` and
  `BuildGLIBCLocally` ("unreferenced" means dead only for code paths). Led's `qStroika_Frameworks_Led_HeavyDebugging`
  stays off - it is turned on by hand for a build or two when debugging Led. A private downstream depends on Led's
  StyledTextIO (RTF, HTML, PlainText), WordProcessor, SimpleTextStore and HiddenText.
- **Build logic**: hairy Makefile recipes may move into `Build/Scripts/`, keeping the output byte-compatible - proved by
  an old-vs-new diff of real runs. New scripts are Python, not Perl.
- **Test coverage is a case-by-case cost/benefit call** - there is no "no redundant coverage" rule; by default keep what
  exists.
- **No more tweaks to the hand-rolled third-party build** (LGP, 2026-09-25): those components are to come from a package
  manager (#1157), and their builds run inside CI runners and other products' docker builds, where extra parallelism or RAM
  has a wide, hard-to-debug blast radius - so no build caching, `jom`, or more `-MP`.
- **Standing decisions from the workaround audit** (#1177's closing comment): clang-15 and g++-11 stay supported; Release
  assertions stay `[[assume]]`; valgrind is memcheck only, on Release builds.

## Working with LGP

- **Discuss or act?** Imperatives ("do X", "fix", "next") mean act. Musings ("could", "should we consider", "one other
  place...") mean reply with a view, the trade-offs and a recommendation - then stop. If unsure, ask in one line.
- **One staged, tested commit per round**, then the remaining list - numbered, with ONE suggested next item - and wait for
  his pick ("pushed; next" takes the suggestion). Never start the next item unasked. If what you find changes the plan he
  approved, report it and offer the choice before going on.
- **End every reply self-contained**: what is done, staged and pending, the key numbers, and the open question with its
  options - his UI buries earlier messages, so never "see above". Start each new task with a one-line header naming it.
- **Long runs**: `tee` to `.claude/<name>.log`, link the log in every status line, and give the expected finish as a clock
  time ("expected around 14:35"). Check on it - a hung step never sends a completion notice.
- **Links**: code as line-range links (`[F.cpp:12-30](path/F.cpp#L12-L30)`), and any file he might open.
- **Numbers**: performance as OLD -> NEW absolute values, naming both sides of any ratio.
- **"Stroika has no X" needs a functional search** - where that job is done today - not a grep for one name.
- **Concurrency bugs**: show the two-thread timeline with line numbers, prove it with a test that fails on the unchanged
  code, then name the general rule (https://github.com/SophistSolutions/Stroika/issues/1205 collects them).
- **Analyze from the real code** - class definitions, signatures, implementations - not READMEs or summaries. A bug report
  he picks up weeks later wants the raw evidence (symbolized backtraces, syscall arguments, the file:line race window).
- **Don't suggest release prep as filler**: a release takes 2-3 days and does nothing else; he starts one, roughly
  monthly, when he decides.
- **3.0dNN is development - stability is not promised.** Defer a rare, non-blocking bug without apology; argue for fixing it
  now only if it blocks other work, is a new regression from this cycle, or will get harder to diagnose. The deadline for
  correctness bugs is the alpha/beta boundary.
