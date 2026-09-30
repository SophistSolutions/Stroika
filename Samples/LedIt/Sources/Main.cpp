/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_MacOS
#include <new.h>

#include <UDrawingState.h> // for class UQDGlobals
#include <UMemoryMgr.h>    // for InitializeHeap
#elif defined(WIN32)
#include <afxwin.h>
#endif

#include "Stroika/Foundation/Debug/Visualizations.h"

#include "LedItApplication.h"

/*
 *  Config/Defines
 */
#if qStroika_Platform_MacOS
#if __profile__
#define qProfile 1
#endif

#ifndef qProfile
#define qProfile 0
#endif
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

/*
 *  Profiling/Mac memory Management.
 */
#if qStroika_Platform_MacOS

#if qProfile && defined(__MWERKS__)
#include "profiler.h"
#endif

#if qUseMacTmpMemForAllocs
inline char* DoSysAlloc (size_t n)
{
#if DEBUG_NEW
    gDebugNewFlags &= ~dnCheckBlocksInApplZone;
#endif
    OSErr  err;                           // ignored...
    Handle h = ::TempNewHandle (n, &err); // try temp mem, and use our local mem if no temp mem left
    if (h == NULL) {
        h = ::NewHandle (n);
        // Don't use up the last few K of heap-memory cuz mac toolbox doesn't like it! -LGP 961021
        if (h != NULL) {
            Handle x = ::NewHandle (4 * 1024);
            if (x == NULL) {
                ::DisposeHandle (h);
                h = NULL;
            }
            else {
                ::DisposeHandle (x);
            }
        }
    }
    if (h == NULL) {
        return NULL;
    }
    ::HLock (h);
    return *h;
}
inline void DoSysFree (void* p)
{
    Handle h = ::RecoverHandle (reinterpret_cast<Ptr> (p));
    ::HUnlock (h);
    ::DisposeHandle (h);
}
inline size_t DoGetPtrSize (void* p)
{
    Handle h = ::RecoverHandle (reinterpret_cast<Ptr> (p));
    return ::GetHandleSize (h);
}
#endif

#if qUseMacTmpMemForAllocs
/*
 *  In CW5Pro and earlier - I hooked into the Metrowerks memory allocation system via #including "New.cpp" and
 *  doing #defines of NewPtr and DisposePtr () etc. But that no longer works with CWPro7 (I'm not sure about
 *  CWPro6). As of CWPro7 - there are several malloc implementations. This below code seems to work for all of
 *  them (from purusing the code in alloc.c, pool_alloc.c, pool_alloc.h, pool_alloc.mac.c). And empiricly it
 *  works fine for the default allocation algorithm -- LGP 2001-09-19
 */
#include "pool_alloc.h"
extern "C" {
void* __sys_alloc (std::size_t n)
{
    return DoSysAlloc (n);
}
void __sys_free (void* p)
{
    DoSysFree (p);
}
std::size_t __sys_pointer_size (void* p)
{
    return ::DoGetPtrSize (p);
}
}
#endif

#if DEBUG_NEW
#define NEWMODE NEWMODE_MALLOC
#include "DebugNew.cp"
#endif

#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

#if qStroika_Platform_MacOS
int main ([[maybe_unused]]int argc, [maybe_unused]]char** argv)
{
#if qStroika_Platform_MacOS
    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        // Set Debugging options
        SetDebugThrow_ (debugAction_Alert);
        SetDebugSignal_ (debugAction_Alert);
    }

#if !TARGET_CARBON
    const long kMinStack = 32l * 1024l; // reserve  32K for stack
    ::SetApplLimit ((Ptr)((long)(::GetApplLimit ()) - kMinStack));
#endif

    /*
     *  Initialize Memory Manager Parameter is number of Master Pointer blocks to allocate
     */
    ::InitializeHeap (3);

    // Initialize standard Toolbox managers
    UQDGlobals::InitializeToolbox ();

#if qProfile && defined(__MWERKS__)
    OSErr proErr = ::ProfilerInit (collectDetailed, bestTimeBase, 10000, 1000);
#endif

    LedItApplication theApp; // Create instance of your Application
    theApp.Run ();           //   class and run it

#if qProfile && defined(__MWERKS__)
    ::ProfilerDump ("\pLedProfile.prof");
    ::ProfilerTerm ();
#endif

#if DEBUG_NEW > 0 && DEBUG_NEW >= DEBUG_NEW_LEAKS
    // Hmm. produces link error doing this on CW6Pro? But seems to work OK without... LGP 2001-07-17
    ::DebugNewValidateAllBlocks ();
#endif
#endif
    return (0);
}
#endif
