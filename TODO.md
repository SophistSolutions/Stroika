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
   - **SSDP - every remaining item, as one list** (2026-10-05), in the order to do them (2026-10-08). The planned
     https://github.com/SophistSolutions/Stroika/issues/1194 work is all done.
       - (for 3.0d25, after the rest of this list - LGP 2026-10-07)
         **ObjectVariantMapper to XML**: Variant::XML::Writer with namespaces,
         attributes and repeated elements - and element order, which ObjectVariantMapper does not keep for an object (a
         Mapping) though UPnP requires it (UPnP Device Architecture 1.1, section 2.5.4): an opt-in ordered representation, or
         objects as arrays. Then UPnP's descriptions and SOAP messages, written as text now, could Serialize through it.
         Variant::XML::Reader is still not implemented.
       - (WTF, not Stroika) **WTF ignores every ssdp:byebye**: it finds the device by its LOCATION's host, which a byebye
         does not carry (Debug: `WeakAssert (not locAddrs.empty ())`) - use SSDP::Client::CachingListener: its callbacks get
         a Listener's Advertisement once per change - fAlive false (removed) with the LOCATION last heard - so WTF fetches
         each description once, not with each NOTIFY as now.
       - (mention SSDP, but not SSDP work) #1195 thread interruption (incl. ConnectionlessSocket ReceiveFrom), #1201 an
         IPv6 scope id in InternetAddress, #1059 threads -> IntervalTimer, #795 mDNS.
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
   - **Concepts - the rest of 2026-10-08's follow-ups** (the rules: Design-Overview.md, "Rules for writing concepts"):
       - **audit Stroika's concepts** (108 in Library) against those rules - including IStdFormatterPredefinedFor_, a hand-kept
         list of what std formats: libstdc++ also formats __int128, unsigned __int128 and _Float128, which it omits (the g++
         build of the IToString fix, 2026-10-08).
       - **StringBuilder's `sb << x` constraint cannot say no**: Characters::Private_::IUnoverloadedToString_ asks
         UnoverloadedToString, which takes any type - make it ask IToString, with the same care (never before ToString.inl's
         overloads are declared).
       - **constrain Characters::ToString itself**, so requires { Characters::ToString (x) } answers truly - then the
         non-template inline functions in ToString.inl that call it (ToString (byte), ToString (chrono::duration<double>))
         call ToStringDefaults::ToString instead.
   - **Replace Ubuntu 25.04 with 26.10** ("Stonking Stingray", released 2026-10-15) as the latest non-LTS. 25.04 has
     been unsupported since 2026-01, and so has 25.10. CI still has 25.04 entries in build-N-test-Matrix.json, plus
     the Build/Docker/Ubuntu2504-* images. Regenerate Documentation/SupportedPlatformsAndCompilers.md afterwards.

  - DO PLANNING for CMAKE change
    - discuss staging
    - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
    - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
    - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
