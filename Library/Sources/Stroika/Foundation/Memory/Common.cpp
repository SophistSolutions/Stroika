/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>

#include <Psapi.h>
#endif

#include "../Debug/Trace.h"

#include "Common.h"

using namespace Stroika;
using namespace Stroika::Foundation;

/*
 ********************************************************************************
 ******************* Memory::GetGlobalAllocationStatistics **********************
 ********************************************************************************
 */
Memory::GlobalAllocationStatistics Memory::GetGlobalAllocationStatistics ()
{
    GlobalAllocationStatistics s;
#if qPlatform_Windows
    HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ::GetCurrentProcessId ());
    if (hProcess != nullptr) {
#pragma comment(lib, "Psapi.lib")
        PROCESS_MEMORY_COUNTERS pmc{};
        pmc.cb = sizeof (pmc);
        if (::GetProcessMemoryInfo (hProcess, &pmc, sizeof (pmc))) {
            s.fPageFaultCount = pmc.PageFaultCount;
            s.fPagefileUsage  = pmc.PagefileUsage;
            s.fWorkingSetSize = pmc.WorkingSetSize;
#if qOverrideOpNewDeleteForAccounting
            s.fTotalOutstandingAllocations    = GetAllocator_ ().GetNetAllocationCount ();
            s.fTotalOutstandingBytesAllocated = GetAllocator_ ().GetNetAllocatedByteCount ();
#elif qDebug
            _CrtMemState memState;
            _CrtMemCheckpoint (&memState);
            s.fTotalOutstandingAllocations    = memState.lCounts[_NORMAL_BLOCK] + memState.lCounts[_CLIENT_BLOCK];
            s.fTotalOutstandingBytesAllocated = memState.lSizes[_NORMAL_BLOCK] + memState.lSizes[_CLIENT_BLOCK];
#endif
        }
        ::CloseHandle (hProcess);
    }
#endif
    return s;
}
