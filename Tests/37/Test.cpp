/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
//  TEST    Foundation::Execution::Signals
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Containers::Set;

namespace {
    void Test1_Basic_ ()
    {
        {
            Set<SignalHandler> saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            {
                // atomic CAN be used with SAFE signal handlers
                atomic<bool> called = false;
                SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called](SignalID signal) -> void { called = true; }, SignalHandler::eSafe));
                ::raise (SIGINT);
                Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
                VerifyTestResult (called);
            }
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
        }
        {
            Set<SignalHandler> saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            {
                // atomic CAN be used with direct signal handlers
                atomic<bool> called = false;
                SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called](SignalID signal) -> void { called = true; }, SignalHandler::eDirect));
                ::raise (SIGINT);
                Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
                VerifyTestResult (called);
            }
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
        }
        {
            Set<SignalHandler> saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            {
                // Synchronized CAN be used with SAFE signal handlers
                Synchronized<bool> called = false;
                SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called](SignalID signal) -> void { called = true; }, SignalHandler::eSafe));
                ::raise (SIGINT);
                Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
                VerifyTestResult (called);
            }
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
        }
    }
}

namespace {
    Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE ("thread") void Test2_Direct_ ()
    {
        Set<SignalHandler> saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        {
            bool called = false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called](SignalID signal) -> void { called = true; }, SignalHandler::Type::eDirect));
            Stroika_Foundation_Debug_ValgrindDisableHelgrind (called); // helgrind doesnt know signal handler must have returend by end of sleep.
            ::raise (SIGINT);
            VerifyTestResult (called);
        }
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
    }
}

namespace {
    void Test3_Safe_ ()
    {
        Set<SignalHandler> saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        {
            bool called = false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called](SignalID signal) -> void { called = true; }));
            Stroika_Foundation_Debug_ValgrindDisableHelgrind (called); // helgrind doesnt know signal handler must have returend by end of sleep.
            ::raise (SIGINT);
            Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
            VerifyTestResult (called);
        }
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test1_Basic_ ();
        Test2_Direct_ ();
        Test3_Safe_ ();
    }
}

int main (int argc, const char* argv[])
{
    SignalHandlerRegistry::SafeSignalsManager safeSignals;
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
