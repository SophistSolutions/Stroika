/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CPUAffinity_h_
#define _Stroika_Foundation_Execution_CPUAffinity_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Set.h"

/**
 *  \file
 *      Control which logical CPU cores THIS PROCESS is permitted to run on.
 *
 *  Companion to Common/SystemConfiguration.h, which DESCRIBES the machine (how many sockets, how many
 *  logical cores, model names). This header does not describe anything - it constrains where the caller
 *  runs. Core numbering here is the same 0-based logical core numbering the OS uses, and the count comes
 *  from there, not from here.
 *
 *  @see Common::GetNumberOfLogicalCPUCores - how many logical cores exist (cheap, mildly stale)
 *  @see Common::GetSystemConfiguration_CPU - fuller topology, sockets and per-core details
 *
 *  The motivating use is measurement. A benchmark that migrates between cores mid-run measures the
 *  migration as well as the code: pinning Tests/52 to one core took its run-to-run spread from 8.3%
 *  median (39% worst) to 2.6% (17% worst) - about 3x tighter - while moving the scores themselves not at
 *  all (median ratio 0.992x across 34 tests). So this buys precision, not speed. Pinning ordinary
 *  application code is usually a pessimization, since it denies the scheduler every other core.
 *
 *  \note   PROCESS SCOPE - and that means subtly different things per platform. A real platform
 *          difference, not an implementation gap:
 *              o   Windows: SetProcessAffinityMask () is genuinely process-wide - it applies to every
 *                  thread, including ones already running.
 *              o   Linux: there is no process-wide call. sched_setaffinity (0, ...) sets the CALLING
 *                  THREAD. Threads created later inherit the mask; threads already running keep the old
 *                  one. Changing those would mean walking /proc/self/task, which this does not do.
 *          For the intended use - pin early, from a still-single-threaded program - the two coincide.
 *
 *  \note   NOT AVAILABLE EVERYWHERE - see kCPUAffinitySupported. macOS has no equivalent of
 *          sched_setaffinity: thread_policy_set (THREAD_AFFINITY_POLICY) is an advisory hint about which
 *          threads want to share a cache, not a pinning primitive, and does nothing whatever on Apple
 *          Silicon. Reported honestly here rather than faked.
 *
 *  TODO:
 *      @todo   Thread::Ptr::SetCPUAffinity () - PER-THREAD affinity, so a caller could pin a thread other
 *              than its own (ThreadPool workers, say). Deliberately not here. It belongs on Thread::Ptr as
 *              a sibling of SetThreadPriority (), which is the established shape for a portable, coarse,
 *              set-only thread knob - including having no getter, which happens to suit affinity too since
 *              Win32 has no GetThreadAffinityMask (and reading it back by calling SetThreadAffinityMask
 *              twice perturbs the thing it claims to report).
 *
 *              Why wait: SetThreadPriority () copes with being called BEFORE Start () by stashing
 *              fRep_->fInitialPriority_ and applying it at launch, because that is a common sequence.
 *              Affinity has the same usage pattern, so a member that silently did nothing before Start ()
 *              would be a trap manufactured by matching that precedent only halfway - and honoring it
 *              properly needs a new field on the Thread rep plus a hook at thread start. Worth doing when
 *              some caller actually needs to pin another thread; until then this process-scope API covers
 *              the measurement use, and is also where the portable GETTER has to live regardless.
 *
 *      @todo   Windows confines a process to one processor GROUP, so its mask covers at most 64 logical
 *              cores. A >64-core Windows machine needs SetThreadGroupAffinity and a group number threaded
 *              through this API. Not done - no such machine here to test against.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \brief  A set of 0-based logical CPU core numbers, as used by Get/SetCPUAffinity.
     *
     *  Deliberately a Set<> of core numbers and not a bitmask: it says what it means, it copies by value
     *  like the rest of Stroika, and it does not silently cap at 64 the way a uint64_t would.
     *
     *  Numbering matches the OS's logical core numbering, so an element is always
     *  < Common::GetNumberOfLogicalCPUCores ().
     */
    using LogicalCPUCoreSet = Containers::Set<unsigned int>;

    /**
     *  \brief  Can CPU affinity be controlled on this platform at all?
     *
     *  When false: SetCPUAffinity () throws FeatureNotSupportedException, SetCPUAffinityQuietly () returns
     *  false without throwing (the check is if constexpr, so it costs nothing), GetCPUAffinity ()
     *  returns nullopt, and PinToOneLogicalCPUCoreQuietly () returns nullopt having changed nothing.
     *  Test this, or just use a Quietly form - affinity is a tuning knob, and code that *requires* it
     *  cannot run on macOS.
     */
    constexpr bool kCPUAffinitySupported =
#if qStroika_Foundation_Common_Platform_Windows or qStroika_Foundation_Common_Platform_Linux
        true
#else
        false
#endif
        ;

    /**
     *  \brief  Which logical CPU cores this process may currently run on; nullopt if not knowable.
     *
     *  This is a LIVE query every time, deliberately unlike Common::GetNumberOfLogicalCPUCores () which
     *  caches with an allowedStaleness. The core count barely changes; an affinity mask can be rewritten
     *  underneath a running process at any moment (a cgroup cpuset being edited, or a launcher applying
     *  taskset), and a stale answer here would be used to pick a core that is no longer permitted.
     *
     *  Returns nullopt when kCPUAffinitySupported is false, or when the query fails.
     *
     *  \par Example Usage
     *      \code
     *          // Pin to a core we are actually permitted to use. Never just name core 0: inside
     *          // 'docker run --cpuset-cpus=2,3' it is not in our set, and asking for it fails outright.
     *          if (optional<LogicalCPUCoreSet> allowed = GetCPUAffinity (); allowed and not allowed->empty ()) {
     *              (void)SetCPUAffinityQuietly (LogicalCPUCoreSet{*allowed->Min ()});
     *          }
     *          // ... or just say what you mean, which does exactly that:
     *          (void)PinToOneLogicalCPUCoreQuietly ();
     *      \endcode
     */
    optional<LogicalCPUCoreSet> GetCPUAffinity ();

    /**
     *  \brief  Restrict this process to the given logical CPU cores.
     *
     *  \req not cores.empty ()
     *
     *  \note   Deliberately NOT a precondition that each core exists, or that it is currently permitted.
     *          Both are environment rather than program invariant, and both can change between checking
     *          and calling - a cgroup cpuset can be rewritten, cores can be hotplugged - so asserting
     *          would be a race that reports a programmer error for something that is not one. The OS
     *          checks regardless, and reports it through the right channel: an exception here, or false
     *          from SetCPUAffinityQuietly ().
     *
     *  \note   Naming a core outside the currently permitted set fails: the OS will probably not widen a
     *          restriction imposed from outside it (a cgroup cpuset, or a taskset wrapper).
     *
     *  @see SetCPUAffinityQuietly - usually the one you want, since failing to pin is not an error.
     */
    void SetCPUAffinity (const LogicalCPUCoreSet& cores);

    /**
     *  \brief  Like SetCPUAffinity (), but returns false rather than throwing.
     *
     *  False means unsupported on this platform, or the OS refused; either way nothing changed. Prefer
     *  this form: something that would merely *like* to be pinned should not fail because it happens to
     *  be running somewhere that will not allow it.
     */
    bool SetCPUAffinityQuietly (const LogicalCPUCoreSet& cores) noexcept;

    /**
     *  \brief  Pin to a single logical CPU core chosen from those currently permitted; returns which, or nullopt.
     *
     *  The measurement helper. It encodes the thing that is easy to get wrong: it chooses from the mask
     *  the caller ALREADY holds rather than naming a core. Hardcoding core 0 is what breaks under
     *  'docker run --cpuset-cpus=2,3', where taskset reports
     *  "failed to set pid's affinity: Invalid argument" - and a launcher that ignores that failure while
     *  having already redirected its output leaves a zero-byte result file behind.
     *
     *  Returns nullopt if affinity is unsupported or could not be set, having changed nothing, so the
     *  caller can carry on unpinned - just noisier.
     */
    optional<unsigned int> PinToOneLogicalCPUCoreQuietly () noexcept;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CPUAffinity.inl"

#endif /*_Stroika_Foundation_Execution_CPUAffinity_h_*/
