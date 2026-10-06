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
       - **no random 0-100 ms wait before the initial set of NOTIFYs** - nor when a new address or interface appears: the UPnP
         Device Architecture's guard against network storms, when many devices start together (1.1, section 1.2.2).
       - **SearchResponder answers a multicast M-SEARCH at once**; the spec says after a random 0..MX seconds, so devices
         do not all answer together.
       - **PeriodicNotifier's NOTIFYs go out with the OS's default multicast TTL (1)**; Search and SearchResponder set 4
         (the UPnP Device Architecture says 4 in 1.0, 2 in 1.1).
       - **BasicServer advertises no services** - no NOTIFY or search answer per service type (BasicServer.h's
         "@todo Add serviceList support"); DeviceDescription already has fServices.
       - (optional) **re-announce at a random interval** under max-age/2, as the spec recommends -
         FrequencyInfo::fRepeatInterval is a fixed 180s.
       - **a network appearing drops what waits in the old sockets**: Listener switches to new sockets (a slow callOnFinds
         lost 2 NOTIFYs on the rig), and SearchResponder restarts its thread on new ones. Fix: join the new interfaces on the
         existing sockets ("already a member" counting as joined) - nothing to switch, so nothing lost or doubled.
       - **answer a search with the address of the interface it arrived on**, not the route lookup's - which a Tailscale
         subnet route covering the LAN turns into the Tailscale address (#1194's step-2 comment). UPnP-only: pick the
         interface by the asker's subnet (Interface::fBindings.fAddressRanges; an IPv6 link-local asker's scope id names
         it), else the route lookup. The exact way - IP_PKTINFO - is https://github.com/SophistSolutions/Stroika/issues/1202
         (UNLIKELY for v3.0).
       - **implement CachingListener** (an empty stub since the first UPnP draft, 2013) - AFTER the callOnFinds redesign
         below: a device cache fed by both Listener and Search, keyed by USN, refreshed by each alive or search answer,
         expired at max-age (Advertisement::fMaxAge), dropped on ssdp:byebye, with added/removed callbacks. Then WTF can use
         it, which also fixes its ignoring byebye (below). Its contents after a wait are also the synchronous search Search.h
         had an @todo for ("sends a certain number of times, and then returns all the answers") - decide there whether that
         needs an API of its own.
       - **Listener and Search call callOnFinds with their callback list's mutex held** (Search.cpp: "DEADLOCK CITY") - so
         AddOnFoundCallback from another thread waits for a slow callOnFinds. (LinkMonitor does too, deliberately: it is how
         its RemoveCallback waits for a callback running on another thread - #1205's general question.)
       - **no way to remove a callOnFinds** (Listener's and Search's AddOnFoundCallback had "@todo RETHINK!"). Solved
         elsewhere in Stroika: by value, with an Execution::Function (LinkMonitor's RemoveCallback - though comparable
         function objects go against modern C++'s function wrappers), or by a handle whose destruction removes it
         (IntervalTimer::Adder). Either way, once removal returns, the callback is not running (LinkMonitor's guarantee).
       - **a callOnFinds that throws is swallowed** (Listener and Search): the callbacks after it miss that packet, and the
         thread sleeps 1 s (its guard against an error storm), with nothing reported - where LinkMonitor logs it and calls
         the rest. Decide with the two above whether to add an OnError callback (both headers had "@todo Consider adding
         OnError callback?").
       - https://github.com/SophistSolutions/Stroika/issues/1194 - close, noting IP_PKTINFO (#1202) and the socket switch
         above.
       - https://github.com/SophistSolutions/Stroika/issues/715 ("-s / -l sometimes produce no results") - likely fixed by
         #1194: check with the SSDPClient sample on Windows and Linux, then close. Firewalls are the other suspect: rewrite
         Listener.h's "Firewall Note" (which says only that turning off firewalls, rebooting, and trying again often helps)
         from what that check finds.
       - https://github.com/SophistSolutions/Stroika/issues/1094 (server started with no network yet) - likely fixed by
         #1194 and following network changes: check on the rig (a container with no network, then one appearing), then close.
       - https://github.com/SophistSolutions/Stroika/issues/986 (IPv6 on the SSDP server, and Ping) - SSDP's part done by
         #1194 (verify); Ping's is not SSDP.
       - https://github.com/SophistSolutions/Stroika/issues/975 (SSDPServer sample: use the WebServer framework) - looks
         done (it uses WebServer::ConnectionManager); check its leftover IO/Network/Listener.h include, then close.
       - (WTF, not Stroika) **WTF ignores every ssdp:byebye**: it finds the device by its LOCATION's host, which a byebye
         does not carry (Debug: `WeakAssert (not locAddrs.empty ())`) - match by USN instead.
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
     write the patterns down in Thread-Safety.md and audit Stroika for them - callback registries' unregister guarantee,
     blocking under a lock (ThreadPool, Logger, SSDP Search), thread members declared last (#1209).
   - **Replace Ubuntu 25.04 with 26.10** ("Stonking Stingray", released 2026-10-15) as the latest non-LTS. 25.04 has
     been unsupported since 2026-01, and so has 25.10. CI still has 25.04 entries in build-N-test-Matrix.json, plus
     the Build/Docker/Ubuntu2504-* images. Regenerate Documentation/SupportedPlatformsAndCompilers.md afterwards.

  - DO PLANNING for CMAKE change
    - discuss staging
    - Maybe first step is the MACRO for the build root(discuss if that is done in a way to mirror fit with cmake)
    - MAYBE get all MY THIRDPARTYCOMPUNTENTS built using a single cmake build line. That seems doable, and a big step towards being able to USE conan (or similar).
    - then later can think about remaining stroika usage steps (using it internally to build/specify, and GENERATING making consumable from cmake, and skel/examples using it)

  - Review UTFConvert and CodeCvt APIs (advice, performance, API choice).
