/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cassert>
#include <cstdlib>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/BackTrace.h"
#include "Stroika/Foundation/Debug/Debugger.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Assertions.h"

#if qPlatform_POSIX
#include <cstdio>
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::Debug, qDebug, qDebug);

#if qDebug

namespace {
    void DefaultAssertionHandler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept
    {
        try {
            DbgTrace ("{} ({}) failed in '{}'; {}:{}"_f, String{assertCategory == nullptr ? "Unknown assertion" : assertCategory},
                      String{assertionText == nullptr ? "" : assertionText}, String{functionName == nullptr ? "" : functionName},
                      String{fileName == nullptr ? "" : fileName}, lineNum);
#if qPlatform_POSIX
            fprintf (stderr, "%s (%s) failed in '%s'; %s:%d\n", assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                     assertionText == nullptr ? "" : assertionText, functionName == nullptr ? "" : functionName,
                     fileName == nullptr ? "" : fileName, lineNum);
#endif
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
            {
                wstring tmp{Debug::BackTrace::Capture ()};
                if (not tmp.empty ()) {
                    DbgTrace ("BackTrace: {}"_f, tmp);
                }
            }
#endif
            DropIntoDebuggerIfPresent ();
            DbgTrace ("ABORTING..."_f);
#if qPlatform_POSIX
            fprintf (stderr, "ABORTING...\n");
#endif
        }
        catch (...) {
        }
        abort (); // if we ever get that far...
    }
    void DefaultWeakAssertionHandler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept
    {
        DbgTrace ("{} ({}) failed in '{}'; {}:{}"_f, String{assertCategory == nullptr ? "Unknown assertion" : assertCategory},
                  String{assertionText == nullptr ? "" : assertionText}, String{functionName == nullptr ? "" : functionName},
                  String{fileName == nullptr ? "" : fileName}, lineNum);
#if qPlatform_POSIX
        fprintf (stderr, "%s (%s) failed in '%s'; %s:%d\n", assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                 assertionText == nullptr ? "" : assertionText, functionName == nullptr ? "" : functionName,
                 fileName == nullptr ? "" : fileName, lineNum);
#endif
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        {
            wstring tmp{Debug::BackTrace::Capture ()};
            if (not tmp.empty ()) {
                DbgTrace ("BackTrace: {}"_f, tmp);
            }
        }
#endif
    }
}

namespace {
    atomic<AssertionHandlerType>     sAssertFailureHandler_{DefaultAssertionHandler_};
    atomic<WeakAssertionHandlerType> sWeakAssertFailureHandler_{DefaultWeakAssertionHandler_};
}

/*
 ********************************************************************************
 **************************** Debug::GetAssertionHandler ************************
 ********************************************************************************
 */
AssertionHandlerType Stroika::Foundation::Debug::GetAssertionHandler ()
{
    return sAssertFailureHandler_;
}

/*
 ********************************************************************************
 ************************** Debug::GetDefaultAssertionHandler *******************
 ********************************************************************************
 */
AssertionHandlerType Stroika::Foundation::Debug::GetDefaultAssertionHandler ()
{
    return DefaultAssertionHandler_;
}

/*
 ********************************************************************************
 ********************************* Debug::SetAssertionHandler *******************
 ********************************************************************************
 */
void Stroika::Foundation::Debug::SetAssertionHandler (AssertionHandlerType assertionHandler)
{
    sAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultAssertionHandler_ : assertionHandler;
}

/*
 ********************************************************************************
 ************************** Debug::GetWeakAssertionHandler **********************
 ********************************************************************************
 */
WeakAssertionHandlerType Stroika::Foundation::Debug::GetWeakAssertionHandler ()
{
    return sWeakAssertFailureHandler_;
}

/*
 ********************************************************************************
 ********************** Debug::GetDefaultWeakAssertionHandler *******************
 ********************************************************************************
 */
WeakAssertionHandlerType Stroika::Foundation::Debug::GetDefaultWeakAssertionHandler ()
{
    return DefaultWeakAssertionHandler_;
}

/*
 ********************************************************************************
 ************************** Debug::SetWeakAssertionHandler **********************
 ********************************************************************************
 */
void Stroika::Foundation::Debug::SetWeakAssertionHandler (WeakAssertionHandlerType assertionHandler)
{
    sWeakAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultWeakAssertionHandler_ : assertionHandler;
}

void Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ (const char* assertCategory, const char* assertionText,
                                                                            const char* fileName, int lineNum, const char* functionName) noexcept
{
    (sWeakAssertFailureHandler_.load ()) (assertCategory, assertionText, fileName, lineNum, functionName);
}

[[noreturn]] void Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (const char* assertCategory, const char* assertionText,
                                                                                    const char* fileName, int lineNum, const char* functionName) noexcept
{
    static bool s_InTrap = false;
    if (s_InTrap) {
        // prevent infinite looping if we get an assertion triggered while processing an assertion.
        // And ignore threading issues, because we are pragmatically aborting at this stage anyhow...
        abort ();
    }
    s_InTrap = true;
    (sAssertFailureHandler_.load ()) (assertCategory, assertionText, fileName, lineNum, functionName);
    //  in case using some sort of assertion handler that allows for continuation
    //  (like under debugger manipulation of PC to go a little further in the code)
    s_InTrap = false;

    // Doesn't matter much what we do at this stage, but in visual studio debugger, you can skip this line
#if qCompilerAndStdLib_quick_exit_Buggy
    _Exit (1);
#else
    quick_exit (1);
#endif
}

#endif
