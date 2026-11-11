/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Execution::Signals
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#if qStroika_Platform_POSIX
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"

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
    void Test5_HandlerStaysInstalled_ ()
    {
        // a handler stays installed once called - the Windows CRT resets a signal to SIG_DFL before calling its handler, so
        // without Stroika putting it back, SIGINT's second arrival ends the program
        Set<SignalHandler>      saved   = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
        static atomic<int>      sCalls;
        sCalls = 0;
        SignalHandlerRegistry::sThe.SetSignalHandlers (
            SIGINT, SignalHandler{+[] ([[maybe_unused]] SignalID signal) noexcept -> void { ++sCalls; }, SignalHandler::eDirect});
        ::raise (SIGINT);
        ::raise (SIGINT);
        EXPECT_EQ (sCalls, 2);
    }
}

namespace {
    void Test6_ConcurrentAddsAllKept_ ()
    {
        // AddSignalHandler from several threads at once keeps every handler added
        SignalHandlerRegistry::SafeSignalsManager safeSignals; // first, so last to go
        Set<SignalHandler>                        saved = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
        SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT);
        static constexpr unsigned int kThreads_       = 4;
        static constexpr unsigned int kAddsPerThread_ = 50;
        Thread::Ptr                   threads[kThreads_];
        for (Thread::Ptr& t : threads) {
            t = Thread::New (
                [] () {
                    for (unsigned int i = 0; i < kAddsPerThread_; ++i) {
                        SignalHandlerRegistry::sThe.AddSignalHandler (SIGINT, SignalHandler{function<void (SignalID)>{[] (SignalID) {}}});
                    }
                },
                Thread::eAutoStart);
        }
        for (const Thread::Ptr& t : threads) {
            t.Join ();
        }
        EXPECT_EQ (SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT).size (), kThreads_ * kAddsPerThread_);
    }
}

namespace {
    void Test7_RemovedSafeHandlerNotRunningNorCalled_ ()
    {
        // Once RemoveSignalHandler returns, the safe handler removed is not running, and is not called again - even for a signal
        // that arrived before - so what it uses may then go
        SignalHandlerRegistry::SafeSignalsManager safeSignals; // first, so last to go
        Set<SignalHandler>                        saved = SignalHandlerRegistry::sThe.GetSignalHandlers (SIGINT);
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, saved); });
        // Two handlers, called in an order not promised: the first called takes a while, so the other waits its turn
        static atomic<int>  sCalls[2];
        static atomic<int>  sFirst;
        static atomic<bool> sFirstRunning;
        static atomic<bool> sMarkerCalled;
        sCalls[0] = sCalls[1] = 0;
        sFirst                = -1;
        sFirstRunning = sMarkerCalled = false;
        auto handler                  = [] (int me) {
            return SignalHandler{function<void (SignalID)>{[me] (SignalID) {
                ++sCalls[me];
                if (int none = -1; sFirst.compare_exchange_strong (none, me)) {
                    sFirstRunning = true;
                    Execution::Sleep (1s);
                    sFirstRunning = false;
                }
            }}};
        };
        SignalHandler handlers[2] = {handler (0), handler (1)};
        SignalHandlerRegistry::sThe.SetSignalHandlers (SIGINT, Set<SignalHandler>{handlers[0], handlers[1]});
        ::raise (SIGINT);
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; not sFirstRunning and Time::GetTickCount () < giveUpAt;) {
            Execution::Sleep (10ms);
        }
        ASSERT_TRUE (sFirstRunning);
        int first = sFirst;
        int other = 1 - first;
        SignalHandlerRegistry::sThe.RemoveSignalHandler (SIGINT, handlers[other]); // its call is still to come, for that SIGINT
        // a marker, so the next SIGINT is handled, and when it is called that first SIGINT is done with
        SignalHandler marker{function<void (SignalID)>{[] (SignalID) { sMarkerCalled = true; }}};
        SignalHandlerRegistry::sThe.AddSignalHandler (SIGINT, marker);
        SignalHandlerRegistry::sThe.RemoveSignalHandler (SIGINT, handlers[first]); // it is running now
        EXPECT_FALSE (sFirstRunning);
        ::raise (SIGINT);
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; not sMarkerCalled and Time::GetTickCount () < giveUpAt;) {
            Execution::Sleep (10ms);
        }
        ASSERT_TRUE (sMarkerCalled);
        EXPECT_EQ (sCalls[other], 0);
        EXPECT_EQ (sCalls[first], 1);
    }
}

#if qStroika_Platform_POSIX
namespace {
    // Overflows any stack: each call's volatile frame stays on it, and the limit - never reached - stops the compiler from
    // treating this as endless recursion
    volatile int sRecursionLimit_ = numeric_limits<int>::max ();
    int          Recurse_ (int depth)
    {
        volatile char frame[1024];
        frame[0] = static_cast<char> (depth);
        return depth < sRecursionLimit_ ? Recurse_ (depth + 1) + frame[0] : frame[0];
    }
    void Test8_CrashHandlerRunsAfterStackOverflow_ ()
    {
        // A thread opts in to an alternate signal stack (InstallAlternateSignalStack) - none is made for it otherwise - and its
        // signal handlers then run there, so a crash handler runs after a stack overflow too
        auto hasAlternateStack = [] (bool install) {
            stack_t     current{};
            Thread::Ptr t = Thread::New (
                [&] () {
                    if (install) {
                        SignalHandlerRegistry::InstallAlternateSignalStack ();
                    }
                    Verify (::sigaltstack (nullptr, &current) == 0);
                },
                Thread::eAutoStart);
            t.Join ();
            return (current.ss_flags & SS_DISABLE) == 0;
        };
        EXPECT_TRUE (hasAlternateStack (true));
        if (Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer or Debug::IsRunningUnderValgrind ()) {
            return; // each may give a thread its own alternate stack, and handles a stack overflow itself
        }
        EXPECT_FALSE (hasAlternateStack (false)); // a thread that does not ask pays nothing
#if qStroika_Platform_MacOS
        // The overflow below happens in a fork child, and on macOS a fork child does not keep its parent's alternate signal stack
        // (Linux's does): with no stack for the handler, the kernel kills the child with SIGILL
        return;
#endif
        Set<SignalID>                         overflowSignals{SIGSEGV, SIGBUS}; // SIGBUS on macOS
        Mapping<SignalID, Set<SignalHandler>> saved;
        for (SignalID s : overflowSignals) {
            saved.Add (s, SignalHandlerRegistry::sThe.GetSignalHandlers (s));
        }
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept {
            for (const auto& [s, handlers] : saved) {
                SignalHandlerRegistry::sThe.SetSignalHandlers (s, handlers);
            }
        });
        SignalHandler           exit42{+[] ([[maybe_unused]] SignalID signal) noexcept -> void { ::_exit (42); }, SignalHandler::eDirect};
        SignalHandlerRegistry::sThe.SetStandardCrashHandlerSignals (exit42, overflowSignals);
        // A stack overflow ends the process it happens in, so it happens in a forked copy of this one: fork copies only the calling
        // thread - this, the main thread - with its alternate signal stack, so that is the thread that opts in
        SignalHandlerRegistry::InstallAlternateSignalStack ();
        pid_t child = ::fork ();
        ASSERT_NE (child, -1);
        if (child == 0) {
            // the child overflows its (main thread's) stack - at most 1MB, so the overflow comes quickly even with no stack limit
            rlimit stackLimit{};
            (void)::getrlimit (RLIMIT_STACK, &stackLimit);
            stackLimit.rlim_cur = min<rlim_t> (stackLimit.rlim_cur, 1024 * 1024);
            (void)::setrlimit (RLIMIT_STACK, &stackLimit);
            (void)Recurse_ (0);
            ::_exit (1);
        }
        int status{};
        Verify (::waitpid (child, &status, 0) == child);
        EXPECT_TRUE (WIFEXITED (status) and WEXITSTATUS (status) == 42)
            << "wait status " << status << " (killed by signal " << (WIFSIGNALED (status) ? WTERMSIG (status) : 0) << ")";
    }
}
#endif

namespace {
    GTEST_TEST (Foundation_Execution_Signals, all)
    {
        Test1_Direct_ ();
        Test2_Safe_ ();
        Test3_StandardCrashHandlersByDefault_ ();
        Test4_SafeHandlersGoWithTheirManager_ ();
        Test5_HandlerStaysInstalled_ ();
        Test6_ConcurrentAddsAllKept_ ();
        Test7_RemovedSafeHandlerNotRunningNorCalled_ ();
#if qStroika_Platform_POSIX
        Test8_CrashHandlerRunsAfterStackOverflow_ ();
#endif
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
