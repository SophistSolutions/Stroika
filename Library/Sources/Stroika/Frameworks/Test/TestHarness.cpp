/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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

        _Exit (EXIT_FAILURE); // skip
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
#if qTargetPlatformSDKUseswchar_t
        cerr << "FAILED: " << Characters::String::FromSDKString (msg) << endl;
#else
        cerr << "FAILED: " << msg << endl;
#endif
        Debug::DropIntoDebuggerIfPresent ();
        _Exit (EXIT_FAILURE); // skip
    }
    void FatalSignalHandler_ (Execution::SignalID signal) noexcept
    {
        cerr << "FAILED: SIGNAL= " << Execution::SignalToName (signal) << endl;
        DbgTrace ("FAILED: SIGNAL= {}"_f, Execution::SignalToName (signal));
        Debug::DropIntoDebuggerIfPresent ();
        _Exit (EXIT_FAILURE); // skip
    }
}

vector<string> Test::Setup ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
#if qDebug
    Stroika::Foundation::Debug::SetAssertionHandler (ASSERT_HANDLER_);
    Stroika::Foundation::Debug::SetWeakAssertionHandler (WEAK_ASSERT_HANDLER_);
#endif
    Debug::RegisterDefaultFatalErrorHandlers (FatalErrorHandler_);
    using namespace Execution;
    SignalHandlerRegistry::Get ().SetStandardCrashHandlerSignals (SignalHandler{FatalSignalHandler_, SignalHandler::Type::eDirect});
#if qHasFeature_GoogleTest
    // @todo fix to COPY so safe...
    testing::InitGoogleTest (&argc, const_cast<char**> (argv));
#endif
    vector<string> v;
    for (int i = 0; i < argc; ++i) {
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
