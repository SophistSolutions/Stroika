# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- **Does `SerializationOptions::fPrettyPrint = false` PROMISE no whitespace, or merely not add any?** (2026-10-12)
  Nothing asserts it, nothing documents it, and the two XML providers would quite likely disagree about a document
  they did not write. It came up because a regression test matched exact output and LGP rightly objected that the
  spec may promise no such thing. Either write the contract down and test both providers against it, or say that
  output is not byte-stable and stop tests depending on it.

- **ModuleInit's deprecation text does not cover the case that actually bites** (2026-10-12). "Use call_once or
  static inline" is right for construction order, and for destruction where the dependency is formed AT
  construction. It is not right for a global that LATER acquires a reference to another module's object - the
  reverse-destruction guarantee of [basic.start.term] does not reach that. A cached XML Schema::Ptr is exactly that
  case (a Schema::Ptr may not outlive its provider), and the answer is not to hold one - which is what
  WebService::SOAP::EnvelopeSchema and UPnP's two schema accessors all do, compiling afresh per call. Worth a
  sentence in the deprecation note so the next person does not re-derive it.

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
   - **SSDP - every remaining item, as one list** (2026-10-05), in the order to do them (2026-10-08). The planned
     https://github.com/SophistSolutions/Stroika/issues/1194 work is all done.
       - **Decide #954's fate** - its Reader/Writer work is done (2026-10-11), and UPnP's descriptions and SOAP
         messages, which were the intended first callers, were built with the DOM instead (it suits a document of
         fixed shape carrying someone else's XML). So close #954, or reduce it to the Binding producers its
         2026-10-10 comment adds: an XSD/WSDL, and ObjectVariantMapper (#1217). #955 (a web service answering JSON
         or XML) is unblocked by the same work and not started.
       - (mention SSDP, but not SSDP work) #1195 thread interruption (incl. ConnectionlessSocket ReceiveFrom), #1201 an
         IPv6 scope id in InternetAddress, #1059 threads -> IntervalTimer, #795 mDNS.
   - **Variant::XML + XML::Binding has no caller** (2026-10-13) - built, and Tests/32 exercises it, but nothing in Stroika
     writes through it: UPnP's two descriptions and SOAP all went to the DOM instead, which suits a document whose shape is
     fixed and which carries XML of someone else's. What Variant::XML is actually for is a value that must serve EITHER
     format - #955, a web service answering JSON or XML from one ObjectVariantMapper - so that, or #1217 (generate a
     Binding from an ObjectVariantMapper), is what would drive it against a real schema from a real caller. Until one
     does, it is a capability tested only by its own tests.
   - **Variant::XML::Writer.h's TODO block is mostly wrong or dated** (2026-10-12) - "probably wrong, and certainly
     incomplete, but it is now at the point of being testable" was written before any of this release's work, and Tests/32
     asserts against it now. Of the rest: the XML declaration and pretty-printing are no longer wishes but unconditional
     behaviour, so the todo is to make them OPTIONS - XML::DOM::SerializationOptions (fPrettyPrint, fIndent) is the model,
     and the note about writing one for the writer alone, apart from SerializationConfiguration which reader and writer
     share, still stands. The thread-safety one (cloning rep, lock for config data) is unexamined and is on Reader.h too.
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

   - **UPnP's Options types want rationalizing - there are NINE of them** (LGP 2026-10-13, "a bit of a morass"):
     UPnP::Options, GENA::Publisher::Options, GENA::Subscriber::Options, SSDP::Client::{CachingListener, Listener,
     Search}::Options, SSDP::Server::{BasicServer, PeriodicNotifier, SearchResponder}::Options - plus
     WebService::SOAP::Options next door, which UPnP forwards to. What is actually wrong, as against merely numerous:
       - **UPnP::SOAP::Action takes NO Options at all** - not Invoke, HandleAction, CheckRequest or any of the six
         Serialize/DeSerialize overloads. So a device that picked Xerces for its descriptions silently gets the
         default provider for its control traffic, and nobody can switch on envelope validation
         (SOAP::Options::fValidateReadAgainst) on the UPnP path at all. Same family as the fault-detail provider bug
         fixed in a3eb3ff6f3, just not reachable yet. Deciding WHICH Options it should take is the real question:
         WebService::SOAP::Options (honest - Action is SOAP with UPnP conventions on top - but callers then juggle
         two types); or UPnP::Options mapped onto it (tidier, but fValidateReadAgainst would silently not apply,
         since a DESCRIPTION schema is not an ENVELOPE schema); or a third UPnP::SOAP::Options naming both. First
         preference is the first, weakly held.
       - **UPnP::Options and WebService::SOAP::Options are structurally identical and semantically different** - same
         two fields, but fValidateReadAgainst means the description schema in one and the envelope schema in the
         other. Nothing says so at either declaration.
       - **GENA::Publisher::Options and GENA::Subscriber::Options are the same type written twice** - one
         fTimeout{1800.0} each, with near-identical doc comments.
       - The six SSDP ones are probably fine - genuinely different knobs per class - but nobody has checked whether
         they share fields that ought to be one type.
     The field that actually crosses module boundaries is fXMLProvider, and it has no shared convention: each Options
     redeclares it, and each DeSerialize has to remember to thread it (one did not - a3eb3ff6f3).
- v3.0d26x - at the start
   - **PRIORITY: threaded-code bugs, and Synchronized's flawed design choices** -
     https://github.com/SophistSolutions/Stroika/issues/1205 (from IntervalTimer's five removal bugs, fixed 2026-10-05).
     Synchronized: its recursive_mutex default hides calling out while holding a lock (#1206); it cannot wait on its own
     lock, so handshakes need a second one (#1207); per-expression locking reads like an atomic check-then-act (#1208). Then
     write the patterns down in Thread-Safety.md and audit Stroika for them - callback registries' unregister guarantee
     (Execution::CallbackRegistry has it), blocking under a lock (ThreadPool, Logger), thread members declared last (#1209).
     Also: **copy-on-write's "not shared, so write in place" check is a data race** (by the C++ memory model; harmless on x86
     and ARM in practice, so it waited for 3.0d26 - LGP 2026-10-07): Memory::SharedByValue::AssureNOrFewerReferences tests
     shared_ptr::use_count () - a relaxed load - so its write is not ordered after another thread's read through a copy it has
     since dropped. ThreadSanitizer reports it (3.0d25x's Ubuntu 26.04 regression run: Tests/53 - now 54 - SSDP_CachingListener_, its main
     thread polling told.load ().size () as the listener's thread appends). The fix needs an acquire there: a fence (free on
     x86, but ThreadSanitizer does not model fences), or an acquire RMW on the count (two atomics per write: measure with
     Tests/52). Until then, Tests/54's SSDP_CachingListener_ and SSDP_CachingListener_Search_ read told under its lock (a BWA,
     so 3.0d25's ThreadSanitizer runs are quiet): restore their told.load () - the reproducer - with the fix.
   - **SSDP: a shared transport `Ptr`** owning port 1900's sockets, an optional argument to every SSDP class -
     https://github.com/SophistSolutions/Stroika/issues/1220 (after #1205: its dispatch thread is the risk).
   - **Concepts - the rest of 2026-10-08's follow-ups** (the rules: Design-Overview.md, "Rules for writing concepts"):
       - **audit Stroika's concepts** (108 in Library) against those rules.
       - **StringBuilder's `sb << x` constraint cannot say no**: Characters::Private_::IUnoverloadedToString_ asks
         UnoverloadedToString, which takes any type - make it ask IToString, with the same care (never before ToString.inl's
         overloads are declared).
       - **constrain Characters::ToString itself**, so requires { Characters::ToString (x) } answers truly - then the
         non-template inline functions in ToString.inl that call it (ToString (byte), ToString (chrono::duration<double>))
         call ToStringDefaults::ToString instead.
   - **WebServer::Router design** - https://github.com/SophistSolutions/Stroika/issues/1214 - from comparing it with other
     C++ HTTP servers: a route's methods listable (so 405's Allow and OPTIONS include custom methods and HEAD); named, typed
     path parameters, any number; mounting routes under a prefix; CORS as an Interceptor of its own.
   - **Replace Ubuntu 25.04 with 26.10** ("Stonking Stingray", released 2026-10-15) as the latest non-LTS. 25.04 has
     been unsupported since 2026-01, and so has 25.10. CI still has 25.04 entries in build-N-test-Matrix.json, plus
     the Build/Docker/Ubuntu2504-* images. Regenerate Documentation/SupportedPlatformsAndCompilers.md afterwards.


  - DO PLANNING for CMAKE change
    - discuss staging
    - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
    - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
    - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
