# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- document or reconsider the practice of #include Stroika-Frameowkrs-Precome
  Stroika-Foundation-Precompe.h on every cpp and .h file.
  
- delete Stroika's TODO.md entry about AppVersion.h (now #1252);


- **plato: finish its Remote-SSH client setup, as `lewis`** (2026-09-19; the `stroika-dev` account it was to log in as
  was deleted 2026-10-20). Its key and shared ssh files are prepared; what is left - test from plato - is in the shared
  notes' ssh-cleanup checklist. protagoras is already done.

- v3.0d26x - at the start
   - **Issues scheduled for this release: milestone 3.0d26** - https://github.com/SophistSolutions/Stroika/milestone/3.
     What follows here has no issue.
   - **Windows In-Docker regression runs: test against mongo** (LGP 2026-10-23) - Tests/36's 3 mongo tests skip there
     (12 SKIPPED per run in 3.0d25): Build/Scripts/RunLocalWindowsDockerRegressionTests passes only USE_TEST_BASENAME into
     the container, though Regression-Tests.md's command line sets MONGO_CONNECTION_STRING - pass it as
     RunRemoteRegressionTests does. Check the container resolves medusa.lan: that script's comment has Docker's DNS set to
     8.8.8.8 (#876).
   - **VS Code IntelliSense: `#include "AppVersion.h"` not found** (LGP 2026-10-23) in Samples/Service, and likely
     WebServer, WebService, HTMLUI/Backend and Skel's Basic app - each Makefile generates it into
     IntermediateFiles/$(CONFIGURATION)/<app>/ and adds that to the compiler's include path, which c_cpp_properties.json
     lacks. Since f4caa91a7a ApplyConfiguration's default myDefaultIncludePath is [] (not ${workspaceFolder}, which took
     a Skel app's root VERSION file for <version>), and nothing gives IntelliSense that directory instead. Only a
     c_cpp_properties.json with no myDefaultIncludePath gets the new default. IntelliSense only: every 3.0d25 release run
     built the samples.
   - **IO::Network::Transfer::Cache: a quick review, and docs for its Ptr** (LGP 2026-10-24) -
     Library/Sources/Stroika/Foundation/IO/Network/Transfer/Cache.h: `struct Ptr : shared_ptr<Cache::IRep> {};` has an
     empty doc comment.
   - **Time::Duration: review, then probably deprecate PrettyPrint; rename PrettyPrintInfo to FormatOptions, and
     AgePrettyPrintInfo to AgeFormatOptions** (LGP 2026-11-06) - the old names kept DEPRECATED. PrettyPrint's own comment
     says "Consider deprecating, and using 'Format'", which takes the same PrettyPrintInfo. PrettyPrintAge might become a
     Format overload taking the AgeFormatOptions explicitly - `Format (const AgeFormatOptions&, const FormatOptions& = {})`
     - whose first argument cannot be defaulted, or `d.Format ()` would be ambiguous.
   - **Time::Duration: `operator* (long double lhs, const Duration& rhs)` as `template <floating_point T>`?** (LGP
     2026-11-06) - Duration.h; today a double or float is promoted to long double, then DurationSeconds (double) times
     it. Check overload resolution against chrono::duration's own operator*, and an integer left-hand side (2 * d).
   - **Common/Enumeration.h: constrain GetDistanceSpanned and OffsetFromStart with IBoundedEnum** (LGP 2026-11-07) - they use
     eCOUNT / eSTART, so say so in the signature. And GetDistanceSpanned is declared taking an ENUM e but defined taking none - the
     declared one has no definition.
   - **String: a UTF-8 (char8_t) rep** (LGP 2026-11-07; may postpone, once discussed) - so a constant u8 string that is not ASCII
     is shared, not decoded and copied: enum names are UTF-8 now (EnumNameText), shared already when ASCII, as nearly all are.
     String.inl's FromStringConstant has the @todo and the design question (a UTF-8 rep cannot index directly).
   - **A clearer spelling for a span from a pointer and a size** (LGP 2026-11-07: "I don't like the fix even with the
     comment") - since 6f00f5debb, generic code writes `span<const T> (p, n)`, 23 sites each with a `(), not {}`
     comment, because braces let the C++26 draft's span (initializer_list) constructor (P2447, in libstdc++ 16; P4144
     withdrew it) take {p, n} as two Ts when T converts from both - a container of bool read freed memory. Find an idiom
     that needs no comment (a `Memory::MakeSpan (p, n)`, say) and replace them; check first whether GCC 16's release still
     has P2447 (Ubuntu 26.04's g++-16 is a 2026-03-22 trunk snapshot), and whether non-generic sites (span<const bool>, or
     a class type with converting constructors) are at risk too.
   - **After the float and Duration formatting redesign: which other Options want `With`** (LGP 2026-11-06) - scan
     Stroika's Options structs and option classes for constructors that merge option sets or convert from each field's
     type, and `optional<>` fields there only to overlay; convert those whose callers lay settings over a base
     (Design-Overview.md, "Options structs", has the style).

## Watching

Not work items: checks Claude runs (how often, in each) and raises with LGP only when one finds something - so they
never appear in a list of what to do next.

- **WATCH: the release regression results - check once or twice a day** (LGP 2026-10-20) for new failures, and raise
  each with its run's GIT COMMIT (it may be fixed already). Where they are, the one-command scan, and the known-benign
  warnings: the shared notes' regression-results-share note.

- **WATCH: does Node 26 stop Windows CI's HTMLUI failures?** Every build image moved to Node 26 on 2026-10-20 for
  them: "Error 127" at Samples/HTMLUI/QuasarBasedHTMLApp/Makefile:60, its Build.txt ending in "Build succeeded" then
  libuv's "Assertion failed: !(handle->flags & UV_HANDLE_CLOSING)" - Node aborting on exit
  (https://github.com/nodejs/node/issues/56645). Seen on Node 24 2026-10-05, 10-09 and 10-20, always
  windows-vs2k26-msys-x86_64-Release; 26 fixing it rests on one report that 26.5 did not reproduce it. If it recurs on
  26, retry `npm run build` once at that Makefile line. Drop this entry if CI stays clean to the 3.0d26 release. Once
  26 is LTS (expected late 2026-10), Windows-Cygwin's Dockerfile goes back to its commented-out nodejs-lts line.

- **WATCH: does installing WebView2 first stop the VS 2022 docker image's intermittent save failure? - each Windows
  image run** (LGP 2026-11-03) Windows-VS2k22/Dockerfile installs the current Edge WebView2 before VS, so the VS
  installer's own 131.0.2903.86 - and the background update whose download, if the install step ends during it, leaves
  a layer docker cannot save - never happens. In each vs2k22 job's install step log, the updater lines should show no
  install. If the step still fails with hcsshim::ImportLayer (0x3), read that log: was the updater still downloading (a
  `DoDownloadPackage` with no `[Running installer]` after it)? Now and then, check the two Microsoft reports linked in
  that WebView2 step's comment for replies: once either is fixed, try the build without the step. Drop the logging and
  this entry if CI stays clean to the 3.0d26 release.
