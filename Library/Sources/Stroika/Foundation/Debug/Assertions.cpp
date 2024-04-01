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
    void DefaultAssertionHandler_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum,
                                   const wchar_t* functionName) noexcept
    {
        try {
            DbgTrace ("{} ({}) failed in '{}'; {}:{}"_f, assertCategory == nullptr ? L"Unknown assertion" : assertCategory,
                      assertionText == nullptr ? L"" : assertionText, functionName == nullptr ? L"" : functionName,
                      fileName == nullptr ? L"" : fileName, lineNum);
#if qPlatform_POSIX
            fwprintf (stderr, L"%s (%s) failed in '%s'; %s:%d\n", assertCategory == nullptr ? L"Unknown assertion" : assertCategory,
                      assertionText == nullptr ? L"" : assertionText, functionName == nullptr ? L"" : functionName,
                      fileName == nullptr ? L"" : fileName, lineNum);
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
    void DefaultWeakAssertionHandler_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum,
                                       const wchar_t* functionName) noexcept
    {
        DbgTrace ("{} ({}) failed in '{}'; {}:{}"_f, assertCategory == nullptr ? L"Unknown assertion" : assertCategory,
                  assertionText == nullptr ? L"" : assertionText, functionName == nullptr ? L"" : functionName,
                  fileName == nullptr ? L"" : fileName, lineNum);
#if qPlatform_POSIX
        fwprintf (stderr, L"%s (%s) failed in '%s'; %s:%d\n", assertCategory == nullptr ? L"Unknown assertion" : assertCategory,
                 assertionText == nullptr ? L"" : assertionText, functionName == nullptr ? L"" : functionName,
                 fileName == nullptr ? L"" : fileName, lineNum);
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
    atomic<AssertionHandlerType> sAssertFailureHandler_{DefaultAssertionHandler_};
    atomic<AssertionHandlerType> sWeakAssertFailureHandler_{DefaultWeakAssertionHandler_};
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
 ***************************** Debug::SetAssertionHandler ***********************
 ********************************************************************************
 */
void Stroika::Foundation::Debug::SetAssertionHandler (AssertionHandlerType assertionHandler)
{
    sAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultAssertionHandler_ : assertionHandler;
}

namespace {
    void (*sLegacyHandlerDelegate2_) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum,
                                      const char* functionName) noexcept;
}
void Stroika::Foundation::Debug::SetAssertionHandler (void (*legacyHandler) (const char* assertCategory, const char* assertionText,
                                                                             const char* fileName, int lineNum, const char* functionName) noexcept)
{
    sLegacyHandlerDelegate2_ = legacyHandler;
    auto wrapper             = [] (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum,
                       const wchar_t* functionName) noexcept {
        try {
            string narrowAssertCategory = String{assertCategory}.AsNarrowSDKString ();
            string narrowAssert         = String{assertionText}.AsNarrowSDKString ();
            string narrowFile           = String{fileName}.AsNarrowSDKString ();
            string narrowFunctionName   = String{functionName}.AsNarrowSDKString ();
            sLegacyHandlerDelegate2_ (narrowAssertCategory.c_str (), narrowAssert.c_str (), narrowFile.c_str (), lineNum, narrowFunctionName.c_str ());
        }
        catch (...) {
            abort (); // ooops
        }
    };
    SetAssertionHandler (wrapper);
}

/*
 ********************************************************************************
 ************************** Debug::GetWeakAssertionHandler **********************
 ********************************************************************************
 */
AssertionHandlerType Stroika::Foundation::Debug::GetWeakAssertionHandler ()
{
    return sWeakAssertFailureHandler_;
}

/*
 ********************************************************************************
 ********************** Debug::GetDefaultWeakAssertionHandler *******************
 ********************************************************************************
 */
AssertionHandlerType Stroika::Foundation::Debug::GetDefaultWeakAssertionHandler ()
{
    return DefaultWeakAssertionHandler_;
}

/*
 ********************************************************************************
 ************************** Debug::SetWeakAssertionHandler **********************
 ********************************************************************************
 */
void Stroika::Foundation::Debug::SetWeakAssertionHandler (AssertionHandlerType assertionHandler)
{
    sWeakAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultWeakAssertionHandler_ : assertionHandler;
}

namespace {
    void (*sLegacyWeakHandlerDelegate2_) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept;
}
void Stroika::Foundation::Debug::SetWeakAssertionHandler (void (*legacyHandler) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept)
{
    sLegacyWeakHandlerDelegate2_ = legacyHandler;
    auto wrapper                 = [] (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum,
                       const wchar_t* functionName) noexcept {
        try {
            string narrowAssertCategory = String{assertCategory}.AsNarrowSDKString ();
            string narrowAssert         = String{assertionText}.AsNarrowSDKString ();
            string narrowFile           = String{fileName}.AsNarrowSDKString ();
            string narrowFunctionName   = String{functionName}.AsNarrowSDKString ();
            sLegacyHandlerDelegate2_ (narrowAssertCategory.c_str (), narrowAssert.c_str (), narrowFile.c_str (), lineNum, narrowFunctionName.c_str ());
        }
        catch (...) {
            abort (); // ooops
        }
    };
    SetWeakAssertionHandler (wrapper);
}


[[noreturn]] void Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum, const char* functionName) noexcept
{
    static bool s_InTrap = false;
    if (s_InTrap) {
        // prevent infinite looping if we get an assertion triggered while processing an assertion.
        // And ignore threading issues, because we are pragmatically aborting at this stage anyhow...
        abort ();
    }
    s_InTrap = true;
    
    (sAssertFailureHandler_.load ()) (assertCategory, assertionText, fileName, lineNum, Characters::NarrowSDK2Wide(functionName).c_str ());
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

void Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ (const wchar_t* assertCategory, const wchar_t* assertionText,
                                                                            const wchar_t* fileName, int lineNum, const char* functionName) noexcept
{
    (sWeakAssertFailureHandler_.load ()) (assertCategory, assertionText, fileName, lineNum, Characters::NarrowSDK2Wide(functionName).c_str ());
}
#endif
