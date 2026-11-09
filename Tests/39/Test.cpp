/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Execution::Signals
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

using Containers::Mapping;
using Containers::Set;

#if qStroika_HasComponent_googletest
namespace {
    void Test1_Direct_ ()
    {
        Set<SignalHandler>      saved   = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
        {
            // a direct handler is a noexcept function pointer - so a lambda without captures, and what it sets a lock-free atomic
            static atomic<bool> sCalled;
            sCalled = false;
            SignalHandlerRegistry::sThe.SetSignalHandlers (
                SIGINT, SignalHandler{+[] ([[maybe_unused]] SignalID signal) noexcept -> void { sCalled = true; }, SignalHandler::eDirect});
            [[maybe_unused]] auto&& cleanup2 = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
            ::raise (SIGINT);
            EXPECT_TRUE (sCalled);
        }
    }
}

namespace {
    void Test2_Safe_ ()
    {
        SignalHandlerRegistry::SafeSignalsManager safeSignals; // first, so last to go
        // safe signal handlers all run through another thread, so this amounts to thread sync
        {
            Set<SignalHandler>      saved   = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
            [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
            {
                atomic<bool> called{false};
                SignalHandlerRegistry::sThe.SetSignalHandlers (
                    SIGINT, SignalHandler{[&called] ([[maybe_unused]] SignalID signal) -> void { called = true; }});
                ::raise (SIGINT);
                Execution::Sleep (0.5s); // delivery could be delayed because signal is pushed to another thread
                EXPECT_TRUE (called);
            }
        }
        {
            Set<SignalHandler>      saved   = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
            [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
            {
                Synchronized<bool> called = false;
                SignalHandlerRegistry::sThe.SetSignalHandlers (
                    SIGINT, SignalHandler ([&called] ([[maybe_unused]] SignalID signal) -> void { called = true; }));
                ::raise (SIGINT);
                Execution::Sleep (0.5s); // delivery could be delayed because signal is pushed to another thread
                EXPECT_TRUE (called);
            }
        }
    }
}

namespace {
    void Test3_StandardCrashHandlersByDefault_ ()
    {
        // SetStandardCrashHandlerSignals () with its defaults: DefaultCrashSignalHandler, a direct handler, on each crash signal
        Set<SignalID> crashSignals = SignalHandlerRegistry::GetStandardCrashSignals ();
        crashSignals.Remove (SIGABRT); // SetStandardCrashHandlerSignals leaves it alone
        Mapping<SignalID, Set<SignalHandler>> saved;
        for (SignalID s : crashSignals) {
            saved.Add (s, SignalHandlerRegistry::sThe.GetSignalHandlers (s));
        }
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept {
            for (const auto& [s, handlers] : saved) {
                SignalHandlerRegistry::sThe.SetSignalHandlers (s, handlers);
            }
        });
        SignalHandlerRegistry::sThe.SetStandardCrashHandlerSignals ();
        for (SignalID s : crashSignals) {
            Set<SignalHandler> handlers = SignalHandlerRegistry::sThe.GetSignalHandlers (s);
            EXPECT_EQ (handlers.size (), 1u);
            EXPECT_TRUE (handlers.All ([] (const SignalHandler& h) { return h.GetType () == SignalHandler::Type::eDirect; }));
        }
    }
}

namespace {
    void Test4_SafeHandlersGoWithTheirManager_ ()
    {
        // destroying the SafeSignalsManager removes the safe handlers it runs, and a signal left with no handler reverts to its
        // default - rather than keep a handler whose signals would be lost, with no thread to run them
        Set<SignalHandler>      saved   = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
        SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT);
        {
            SignalHandlerRegistry::SafeSignalsManager safeSignals;
            SignalHandlerRegistry::sThe.AddSignalHandler (SIGINT, SignalHandler{function<void (SignalID)>{[] (SignalID) {}}});
            EXPECT_EQ (SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT).size (), 1u);
        }
        EXPECT_TRUE (SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT).empty ());
        auto installed = ::signal (SIGINT, SIG_DFL); // reads what the OS calls on SIGINT (setting what it should already be)
        EXPECT_EQ (installed, SIG_DFL);
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_Signals, all)
    {
        Test1_Direct_ ();
        Test2_Safe_ ();
        Test3_StandardCrashHandlersByDefault_ ();
        Test4_SafeHandlersGoWithTheirManager_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
