/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::Execution::Signals
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Containers::Set;

namespace {
    void Test1_Direct_ ()
    {
        Set<SignalHandler>      saved   = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
        {
            bool called = false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called] ([[maybe_unused]] SignalID signal) noexcept -> void { Lambda_Arg_Unused_BWA (signal); called = true; }, SignalHandler::eDirect));
            [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
            ::raise (SIGINT);
            VerifyTestResult (called);
        }
    }
}

namespace {
    void Test2_Safe_ ()
    {
        // safe signal handlers all run through another thread, so this amounts to thread sync
        {
            Set<SignalHandler>      saved   = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
            {
                atomic<bool> called{false};
                SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called] ([[maybe_unused]] SignalID signal) -> void { Lambda_Arg_Unused_BWA (signal); called = true; }));
                // @todo - as of 2018-02-18 - helgrind still doesn't understand that atomic<bool> is threadsafe
                Stroika_Foundation_Debug_ValgrindDisableHelgrind (called);
                ::raise (SIGINT);
                Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
                VerifyTestResult (called);
            }
        }
        {
            Set<SignalHandler>      saved   = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
            {
                Execution::Synchronized<bool> called = false;
                SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called] ([[maybe_unused]] SignalID signal) -> void { Lambda_Arg_Unused_BWA (signal); called = true; }));
                ::raise (SIGINT);
                Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
                VerifyTestResult (called);
            }
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test1_Direct_ ();
        Test2_Safe_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    SignalHandlerRegistry::SafeSignalsManager safeSignals;
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
