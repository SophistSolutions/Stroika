/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Execution::Signals
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

using Containers::Set;

#if qHasFeature_GoogleTest
namespace {
    void Test1_Direct_ ()
    {
        Set<SignalHandler>      saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup =
            Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
        {
            bool called = false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (
                SIGINT, SignalHandler{[&called] ([[maybe_unused]] SignalID signal) noexcept -> void { called = true; }, SignalHandler::eDirect});
            [[maybe_unused]] auto&& cleanup2 =
                Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
            ::raise (SIGINT);
            DISABLE_COMPILER_MSC_WARNING_START (4127) // conditional expression is constant - WRONG - CAN be constant - but if qCompiler_ValgrindDirectSignalHandler_Buggy, depends on non constexpr function
            if (qCompiler_ValgrindDirectSignalHandler_Buggy and Debug::IsRunningUnderValgrind ()) {
                VerifyTestResultWarning (called);
            }
            else {
                EXPECT_TRUE (called);
            }
            DISABLE_COMPILER_MSC_WARNING_END (4127)
        }
    }
}

namespace {
    void Test2_Safe_ ()
    {
        // safe signal handlers all run through another thread, so this amounts to thread sync
        {
            Set<SignalHandler>      saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            [[maybe_unused]] auto&& cleanup =
                Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
            {
                atomic<bool> called{false};
                SignalHandlerRegistry::Get ().SetSignalHandlers (
                    SIGINT, SignalHandler{[&called] ([[maybe_unused]] SignalID signal) -> void { called = true; }});
                ::raise (SIGINT);
                Execution::Sleep (0.5s); // delivery could be delayed because signal is pushed to another thread
                EXPECT_TRUE (called);
            }
        }
        {
            Set<SignalHandler>      saved = SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
            [[maybe_unused]] auto&& cleanup =
                Execution::Finally ([&] () noexcept { SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved); });
            {
                Execution::Synchronized<bool> called = false;
                SignalHandlerRegistry::Get ().SetSignalHandlers (
                    SIGINT, SignalHandler ([&called] ([[maybe_unused]] SignalID signal) -> void { called = true; }));
                ::raise (SIGINT);
                Execution::Sleep (0.5s); // delivery could be delayed because signal is pushed to another thread
                EXPECT_TRUE (called);
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test1_Direct_ ();
        Test2_Safe_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    SignalHandlerRegistry::SafeSignalsManager safeSignals;
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
