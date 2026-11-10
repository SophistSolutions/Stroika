/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Debugger.h"
#include "Stroika/Foundation/Debug/Fatal.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"

#include "TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;

using namespace Stroika::Foundation::Characters::Literals;

using namespace Stroika::Frameworks::Test;

namespace {
    // These handlers replace Stroika's defaults - why, and how they differ: Setup's docs (TestHarness.h)

    // A failed assertion or fatal error ends the test here. On POSIX by abort (), as assert does - so a core file, where the
    // environment allows one, shows the state at the failure. On Windows by _Exit (1): no core files there (a crash dump comes from
    // Windows Error Reporting), and a Debug build's abort () would stop an unattended run with a dialog
    [[noreturn]] void FailTest_ () noexcept
    {
#if qStroika_Platform_POSIX
        abort ();
#else
        _Exit (EXIT_FAILURE);
#endif
    }
    void ASSERT_HANDLER_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum, const wchar_t* functionName) noexcept
    {
        if (assertCategory == nullptr) {
            assertCategory = L"Unknown assertion";
        }
        if (assertionText == nullptr) {
            assertionText = L"";
        }
        if (fileName == nullptr) {
            fileName = L"";
        }
        if (functionName == nullptr) {
            functionName = L"";
        }
        wcerr << "FAILED: " << assertCategory << "; " << assertionText << ";" << functionName << ";" << fileName << ": " << lineNum << endl;
        DbgTrace ("FAILED: {}; {}; {}; {}; {}"_f, assertCategory, assertionText, functionName, fileName, lineNum);

        Debug::DropIntoDebuggerIfPresent ();

        FailTest_ ();
    }
    void WEAK_ASSERT_HANDLER_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName, int lineNum,
                               const wchar_t* functionName) noexcept
    {
        if (assertCategory == nullptr) {
            assertCategory = L"Unknown assertion";
        }
        if (assertionText == nullptr) {
            assertionText = L"";
        }
        if (fileName == nullptr) {
            fileName = L"";
        }
        if (functionName == nullptr) {
            functionName = L"";
        }
        wcerr << "WARNING: weak assertion  " << assertCategory << "; " << assertionText << ";" << functionName << ";" << fileName << ": "
              << lineNum << endl;
        DbgTrace ("WARNING: weak assertion  {}; {}; {}; {}; {}"_f, assertCategory, assertionText, functionName, fileName, lineNum);
    }
    void FatalErrorHandler_ (const Characters::SDKChar* msg) noexcept
    {
#if qStroika_Foundation_Characters_SDKUseswchar_t
        cerr << "FAILED: " << Characters::String::FromSDKString (msg) << endl;
#else
        cerr << "FAILED: " << msg << endl;
#endif
        Debug::DropIntoDebuggerIfPresent ();
        FailTest_ ();
    }
    void FatalSignalHandler_ (Execution::SignalID signal) noexcept
    {
        cerr << "FAILED: SIGNAL= " << Execution::SignalToName (signal) << endl;
        DbgTrace ("FAILED: SIGNAL= {}"_f, Execution::SignalToName (signal));
        Debug::DropIntoDebuggerIfPresent ();
#if qStroika_Platform_POSIX
        // Die of the signal, as with no handler, so a core file - where the environment allows one - shows the crash itself: the
        // signal stays blocked while this runs, so the raise arrives as this returns
        (void)::signal (signal, SIG_DFL);
        (void)::raise (signal);
#else
        FailTest_ ();
#endif
    }
}

vector<string> Test::Setup ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
#if qStroika_Foundation_Debug_AssertionsChecked
    Stroika::Foundation::Debug::SetAssertionHandler (ASSERT_HANDLER_);
    Stroika::Foundation::Debug::SetWeakAssertionHandler (WEAK_ASSERT_HANDLER_);
#endif
    Debug::RegisterDefaultFatalErrorHandlers (FatalErrorHandler_);
    using namespace Execution;
    SignalHandlerRegistry::sThe.SetStandardCrashHandlerSignals (SignalHandler{FatalSignalHandler_, SignalHandler::Type::eDirect});
#if qStroika_Platform_POSIX
    SignalHandlerRegistry::InstallAlternateSignalStack (); // so a stack overflow in a test (on its main thread) still reports
#endif
#if qStroika_HasComponent_googletest
    // @todo fix to COPY so safe...
    testing::InitGoogleTest (&argc, const_cast<char**> (argv));
#endif
    vector<string> v;
    for (int i = 0; i < argc; ++i) {
#if not qStroika_HasComponent_googletest
        // googletest's own, which InitGoogleTest would have taken (@see Setup's docs)
        if (string_view{argv[i]}.starts_with ("--gtest_")) {
            continue;
        }
#endif
        v.push_back (argv[i]);
    }
    return v;
}

int Test::PrintPassOrFail (void (*regressionTest) ())
{
    try {
        (*regressionTest) ();
        cout << "Succeeded" << endl;
        DbgTrace ("Succeeded"_f);
    }
    catch (...) {
        auto exc = current_exception ();
        cerr << "FAILED: REGRESSION TEST DUE TO EXCEPTION: '" << Characters::ToString (exc) << "'" << endl;
        cout << "Failed" << endl;
        DbgTrace ("FAILED: REGRESSION TEST (Exception): '{}"_f, Characters::ToString (exc));
        Debug::DropIntoDebuggerIfPresent ();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Test::Private_::Test_ (bool failIfFalse, bool isFailureElseWarning, const wchar_t* regressionTestText, const wchar_t* fileName, int lineNum)
{
    if (not failIfFalse) {
        if (isFailureElseWarning) {
            ASSERT_HANDLER_ (L"RegressionTestFailure", regressionTestText, fileName, lineNum, nullptr);
        }
        else {
            wcerr << "WARNING: REGRESSION TEST ISSUE: " << regressionTestText << ";" << fileName << ": " << lineNum << endl;
            DbgTrace ("WARNING: REGRESSION TEST ISSUE: ; {}; {}; {}"_f, regressionTestText, fileName, lineNum);
            // OK to continue
        }
    }
}

void Test::Private_::VerifyTestResultWarning_ (bool failIfFalse, bool isFailureElseWarning, const wchar_t* regressionTestText,
                                               const wchar_t* fileName, int lineNum)
{
    if (not failIfFalse) {
        if (isFailureElseWarning) {
            ASSERT_HANDLER_ (L"RegressionTestFailure", regressionTestText, fileName, lineNum, nullptr);
        }
        else {
            wcerr << "WARNING: REGRESSION TEST ISSUE: " << regressionTestText << ";" << fileName << ": " << lineNum << endl;
            DbgTrace ("WARNING: REGRESSION TEST ISSUE: ; {}; {}; {}"_f, regressionTestText, fileName, lineNum);
            // OK to continue
        }
    }
}

void Test::WarnTestIssue (const char* issue)
{
    using namespace Characters;
    WarnTestIssue (NarrowSDK2Wide (issue).c_str ());
}

void Test::WarnTestIssue (const wchar_t* issue)
{
    wcerr << "WARNING: REGRESSION TEST ISSUE: '" << issue << "'" << endl;
    DbgTrace ("WARNING: REGRESSION TEST ISSUE: '{}"_f, issue);
}

void Test::WarnTestIssue (const Foundation::Characters::String& issue)
{
    WarnTestIssue (issue.As<wstring> ().c_str ());
}

/*
 ********************************************************************************
 ******************* Test::ClockContinuitySampler *******************************
 ********************************************************************************
 */
Test::ClockContinuitySampler::ClockContinuitySampler (Time::DurationSeconds sampleEvery)
    : fSampler_{[this, sampleEvery] () {
        Time::TimePointSeconds prev = Time::GetTickCount ();
        while (not fDone_.load ()) {
            this_thread::sleep_for (sampleEvery);
            Time::TimePointSeconds now = Time::GetTickCount ();
            double                 gap = (now - prev).count ();
            prev                       = now;
            // no compare_exchange loop needed - single writer
            if (gap > fMaxGapSeconds_.load ()) {
                fMaxGapSeconds_.store (gap);
            }
        }
    }}
{
}

Test::ClockContinuitySampler::~ClockContinuitySampler ()
{
    fDone_.store (true);
    fSampler_.join ();
}

Time::DurationSeconds Test::ClockContinuitySampler::GetMaxObservedGap () const
{
    return Time::DurationSeconds{fMaxGapSeconds_.load ()};
}
