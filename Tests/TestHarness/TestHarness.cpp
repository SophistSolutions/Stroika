/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Debugger.h"
#include "Stroika/Foundation/Debug/Fatal.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"

#include "TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::TestHarness;

namespace {
    void ASSERT_HANDLER_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept
    {
        if (assertCategory == nullptr) {
            assertCategory = "Unknown assertion";
        }
        if (assertionText == nullptr) {
            assertionText = "";
        }
        if (functionName == nullptr) {
            functionName = "";
        }
        cerr << "FAILED: " << assertCategory << "; " << assertionText << ";" << functionName << ";" << fileName << ": " << lineNum << endl;
        DbgTrace ("FAILED: %s; %s; %s; %s; %d", assertCategory, assertionText, functionName, fileName, lineNum);

        Debug::DropIntoDebuggerIfPresent ();

        _Exit (EXIT_FAILURE); // skip
    }
    void FatalErrorHandler_ (const Characters::SDKChar* msg) noexcept
    {
#if qTargetPlatformSDKUseswchar_t
        cerr << "FAILED: " << Characters::WideStringToNarrowSDKString (msg) << endl;
#else
        cerr << "FAILED: " << msg << endl;
#endif
        Debug::DropIntoDebuggerIfPresent ();
        _Exit (EXIT_FAILURE); // skip
    }
    void FatalSignalHandler_ (Execution::SignalID signal) noexcept
    {
        cerr << "FAILED: SIGNAL= " << Execution::SignalToName (signal).AsNarrowSDKString () << endl;
        DbgTrace (L"FAILED: SIGNAL= %s", Execution::SignalToName (signal).c_str ());
        Debug::DropIntoDebuggerIfPresent ();
        _Exit (EXIT_FAILURE); // skip
    }
}

void TestHarness::Setup ()
{
#if qDebug
    Stroika::Foundation::Debug::SetAssertionHandler (ASSERT_HANDLER_);
#endif
    Debug::RegisterDefaultFatalErrorHandlers (FatalErrorHandler_);
    using namespace Execution;
    SignalHandlerRegistry::Get ().SetStandardCrashHandlerSignals (SignalHandler (FatalSignalHandler_, SignalHandler::Type::eDirect));
}

int TestHarness::PrintPassOrFail (void (*regressionTest) ())
{
    try {
        (*regressionTest) ();
        cout << "Succeeded" << endl;
        DbgTrace (L"Succeeded");
    }
    catch (...) {
        auto exc = current_exception ();
        cerr << "FAILED: REGRESSION TEST DUE TO EXCEPTION: '" << Characters::ToString (exc).AsNarrowSDKString () << "'" << endl;
        cout << "Failed" << endl;
        DbgTrace (L"FAILED: REGRESSION TEST (Exception): '%s", Characters::ToString (exc).c_str ());
        Debug::DropIntoDebuggerIfPresent ();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void TestHarness::Test_ (bool failIfFalse, bool isFailureElseWarning, const char* regressionTestText, const char* fileName, int lineNum)
{
    if (not failIfFalse) {
        if (isFailureElseWarning) {
            ASSERT_HANDLER_ ("RegressionTestFailure", regressionTestText, fileName, lineNum, "");
        }
        else {
            cerr << "WARNING: REGRESSION TEST ISSUE: " << regressionTestText << ";" << fileName << ": " << lineNum << endl;
            DbgTrace ("WARNING: REGRESSION TEST ISSUE: ; %s; %s; %d", regressionTestText, fileName, lineNum);
            // OK to continue
        }
    }
}

void TestHarness::WarnTestIssue (const char* issue)
{
    cerr << "WARNING: REGRESSION TEST ISSUE: '" << issue << "'" << endl;
    DbgTrace ("WARNING: REGRESSION TEST ISSUE: '%s", issue);
}
