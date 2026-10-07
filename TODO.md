# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- Consider rewrite of remaining perl stuff - mostly configure - to use python?

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
   - **SSDP - every remaining item, as one list** (2026-10-05; dependencies, priorities and estimates to follow). The
     planned https://github.com/SophistSolutions/Stroika/issues/1194 work is all done.
       - (WTF, not Stroika) **WTF ignores every ssdp:byebye**: it finds the device by its LOCATION's host, which a byebye
         does not carry (Debug: `WeakAssert (not locAddrs.empty ())`) - use SSDP::Client::CachingListener: its callbacks get
         a Listener's Advertisement once per change - fAlive false (removed) with the LOCATION last heard - so WTF fetches
         each description once, not with each NOTIFY as now.
       - (mention SSDP, but not SSDP work) #1195 thread interruption (incl. ConnectionlessSocket ReceiveFrom), #1201 an
         IPv6 scope id in InternetAddress, #1059 threads -> IntervalTimer, #795 mDNS.
       - (optional) **CachingListener can re-add a device just withdrawn**: an answer to its search sent before the device's
         ssdp:byebye can arrive after it (UDP reorders) - ignore answers for a USN briefly after its byebye.
       - (this and the next: for 3.0d25, after the rest of this list - LGP 2026-10-07)
         **UPnP services as objects** (estimate 4-5 h, with the samples moved onto them): UPnP::ServiceDescription (a
         service's description, its SCPD) with Serialize; and SOAP control messages - an action's request, response and
         error as types, each with Serialize, plus a request parser (the XML DOM where the build has a parser, else as text) -
         so a device or control point writes no XML of its own. Their XML written as text, like DeviceDescription's
         Serialize. Then the SSDPServer and SSDPClient samples drop their hand-written SCPD and SOAP.
       - **ObjectVariantMapper to XML** (estimate 8-12 h, plus design calls): Variant::XML::Writer with namespaces,
         attributes and repeated elements - and element order, which ObjectVariantMapper does not keep for an object (a
         Mapping) though UPnP requires it (UPnP Device Architecture 1.1, section 2.5.4): an opt-in ordered representation, or
         objects as arrays. Then the UPnP objects above could Serialize through it. Variant::XML::Reader is still not
         implemented: another 6-10 h.
       - **the SSDP samples, to an A-** (multiple services, embedded devices, icons and security not needed - LGP 2026-10-07):
           - SSDPClient (estimate 2 h): use SSDP::Client::CachingListener - a line per device added or removed, not per
             NOTIFY and search answer; fetch each description once, off the SSDP thread (it does blocking HTTP inside the
             callback, under a lock); switch each light once; show its GetStatus (reading the answer: UPnP services as objects).
           - SSDPServer (estimate 1-2 h): a --port option; real description fields (not "model number"); refuse a control
             request that is not text/xml (415: UPnP Device Architecture 1.1, section 3.2.1); a small on/off page as its
             presentationURL.
       - **GENA eventing** (estimate a day, in the framework - Stroika has none): SUBSCRIBE, its renewal and UNSUBSCRIBE, and a
         NOTIFY with SEQ to each subscriber (UPnP Device Architecture 1.1, section 4) - so SSDPServer can tell subscribers each
         change of the light's Status, as SwitchPower:1 says it does.
   - **dynamic-analysis coverage - what is left.** Valgrind itself was settled 2026-09-29 (#1177): kept, memcheck
     only, Release builds, on 24.04 and 26.04 - see Documentation/Debugging.md. The audit's sanitizer and valgrind
     retests are in https://github.com/SophistSolutions/Stroika/issues/1185. Still open:
       - **GitHub Actions runs no sanitizer or valgrind job at all** - so dynamic analysis is entirely
         a local-release-run activity. Worth deciding if that is intentional.
       - msan is not usable with gcc (clang-only, and needs a specially rebuilt libc++) - see the note
         near the top of MakeRegressionTestConfigurations. So the realistic menu is asan/ubsan/leak,
         tsan, and valgrind - and valgrind does find what the first two cannot: its first 26.04 run caught
         libstdc++'s from_chars overread (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=127666), which ASan missed.
       - **every sanitizer configuration is a `g++-*` one - there is no clang asan/tsan/ubsan anywhere**
         (noted 2026-09-10 during the compiler-coverage audit). clang's sanitizers are the better-supported
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
   - **Replace Ubuntu 25.04 with 26.10** ("Stonking Stingray", released 2026-10-15) as the latest non-LTS. 25.04 has
     been unsupported since 2026-01, and so has 25.10. CI still has 25.04 entries in build-N-test-Matrix.json, plus
     the Build/Docker/Ubuntu2504-* images. Regenerate Documentation/SupportedPlatformsAndCompilers.md afterwards.

  - DO PLANNING for CMAKE change
    - discuss staging
    - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
    - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
    - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
