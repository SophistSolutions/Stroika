/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <Windows.h>
#elif qStroika_Foundation_Common_Platform_Linux
#include <sched.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/FeatureNotSupportedException.h"

#include "CPUAffinity.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Execution;

namespace {
    /*
     *  Deliberately OUTSIDE the platform #if blocks, though only the unsupported branch calls it. A body
     *  inside '#else' is compiled on no machine this is developed on - so a typo there would first appear
     *  on macOS, which is the platform least able to check it. Declaring it unconditionally means every
     *  compiler sees it.
     */
    [[noreturn]] void ThrowCPUAffinityUnsupported_ ()
    {
        Throw (FeatureNotSupportedException{"CPU affinity"sv});
    }
}

namespace {
#if qStroika_Foundation_Common_Platform_Windows
    /*
     *  Windows affinity is a DWORD_PTR bitmask over the process's single processor GROUP, so it tops out at
     *  64 logical cores (32 in a 32-bit build). Beyond that needs SetThreadGroupAffinity and a group
     *  number, which this API does not model - see the @todo in CPUAffinity.h.
     */
    constexpr unsigned int kMaxCoreInAMask_ = static_cast<unsigned int> (sizeof (DWORD_PTR) * 8);

    DWORD_PTR mkMask_ (const LogicalCPUCoreSet& cores)
    {
        DWORD_PTR mask{};
        for (unsigned int c : cores) {
            Require (c < kMaxCoreInAMask_); // else silently unrepresentable, which would be worse than failing
            mask |= (static_cast<DWORD_PTR> (1) << c);
        }
        return mask;
    }
    LogicalCPUCoreSet mkSet_ (DWORD_PTR mask)
    {
        LogicalCPUCoreSet cores;
        for (unsigned int c = 0; c < kMaxCoreInAMask_; ++c) {
            if (mask & (static_cast<DWORD_PTR> (1) << c)) {
                cores.Add (c);
            }
        }
        return cores;
    }
#elif qStroika_Foundation_Common_Platform_Linux
    cpu_set_t mkMask_ (const LogicalCPUCoreSet& cores)
    {
        cpu_set_t cpuSet;
        CPU_ZERO (&cpuSet);
        for (unsigned int c : cores) {
            Require (c < CPU_SETSIZE);
            CPU_SET (c, &cpuSet);
        }
        return cpuSet;
    }
    LogicalCPUCoreSet mkSet_ (const cpu_set_t& cpuSet)
    {
        LogicalCPUCoreSet cores;
        for (unsigned int c = 0; c < CPU_SETSIZE; ++c) {
            if (CPU_ISSET (c, &cpuSet)) {
                cores.Add (c);
            }
        }
        return cores;
    }
#endif
}

/*
 ********************************************************************************
 ********************** Execution::GetCPUAffinity *******************************
 ********************************************************************************
 */
optional<LogicalCPUCoreSet> Execution::GetCPUAffinity ()
{
#if qStroika_Foundation_Common_Platform_Windows
    DWORD_PTR processMask{};
    DWORD_PTR systemMask{};
    if (::GetProcessAffinityMask (::GetCurrentProcess (), &processMask, &systemMask) == 0) {
        return nullopt;
    }
    return mkSet_ (processMask);
#elif qStroika_Foundation_Common_Platform_Linux
    // pid 0 == the calling thread. See the PROCESS SCOPE note in CPUAffinity.h for why that is the right
    // answer here even though it is not literally process-wide on Linux.
    cpu_set_t cpuSet;
    CPU_ZERO (&cpuSet);
    if (::sched_getaffinity (0, sizeof (cpuSet), &cpuSet) != 0) {
        return nullopt;
    }
    return mkSet_ (cpuSet);
#else
    return nullopt; // macOS and anything else: no pinning primitive, so nothing to report
#endif
}

/*
 ********************************************************************************
 ********************** Execution::SetCPUAffinity *******************************
 ********************************************************************************
 */
void Execution::SetCPUAffinity ([[maybe_unused]] const LogicalCPUCoreSet& cores)
{
    Require (not cores.empty ());
    // NB: deliberately NO Require () that each core is < GetNumberOfLogicalCPUCores (), or that it is in
    // the currently permitted set. Both are environment, not program invariant, and both can change
    // between the check and the call - a cgroup cpuset can be rewritten, cores can be hotplugged. Checking
    // would be a race that reports a programmer error for something that is not one. The OS checks anyway,
    // and reports it as an exception (or false, from the Quietly form), which is the right channel for it.
#if qStroika_Foundation_Common_Platform_Windows
    if (::SetProcessAffinityMask (::GetCurrentProcess (), mkMask_ (cores)) == 0) {
        ThrowSystemErrNo ();
    }
#elif qStroika_Foundation_Common_Platform_Linux
    cpu_set_t cpuSet = mkMask_ (cores);
    ThrowPOSIXErrNoIfNegative (::sched_setaffinity (0, sizeof (cpuSet), &cpuSet));
#else
    // NB: a throw, not AssertNotImplemented () - that compiles out in a release build, which would leave
    // this returning normally having done nothing, while the header promises it throws. kCPUAffinitySupported
    // is false here, so callers who do not want the throw should use SetCPUAffinityQuietly ().
    ThrowCPUAffinityUnsupported_ ();
#endif
}

/*
 ********************************************************************************
 *************** Execution::PinToOneLogicalCPUCoreQuietly ***********************
 ********************************************************************************
 */
optional<unsigned int> Execution::PinToOneLogicalCPUCoreQuietly () noexcept
{
    if constexpr (not kCPUAffinitySupported) {
        return nullopt;
    }
    else {
        try {
            // Ask what we HOLD and choose from that. Naming a core outright is the bug this exists to
            // avoid: under 'docker run --cpuset-cpus=2,3' core 0 is not ours and the call fails outright.
            optional<LogicalCPUCoreSet> allowed = GetCPUAffinity ();
            if (not allowed or allowed->empty ()) {
                return nullopt;
            }
            unsigned int pinTo = *allowed->Min ();
            if (SetCPUAffinityQuietly (LogicalCPUCoreSet{pinTo})) {
                return pinTo;
            }
            return nullopt;
        }
        catch (...) {
            return nullopt; // noexcept, and failing to pin is never worth propagating
        }
    }
}
