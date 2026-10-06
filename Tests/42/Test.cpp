/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <optional>

#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Async.h"
#include "Stroika/Foundation/Execution/CPUAffinity.h"
#include "Stroika/Foundation/Execution/CallbackRegistry.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest
// must be tested before main, so cannot call directly below
namespace {
    int TestAtomicInitializedCoorectly_ ();
    static int sIgnoredTestValue_ = TestAtomicInitializedCoorectly_ (); // if using static constructors, this will be called before sAtomicBoolNotInitializedTilAfterStaticInitizers_

    atomic<bool> sAtomicBoolNotInitializedTilAfterStaticInitizers_{true}; // for calls before start of or after end of main ()
    int          TestAtomicInitializedCoorectly_ ()
    {
        EXPECT_TRUE (sAtomicBoolNotInitializedTilAfterStaticInitizers_);
        return 1;
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, AsyncRunAll_)
    {
        Debug::TraceContextBumper ctx{"AsyncRunAll_"};
#if !qCompilerAndStdLib_tsubst_pack_expansion_Buggy
        {
            auto results = RunAll ([] () { return 1; }, [] () { return 2; }, [] () { return 3; });
            EXPECT_EQ (results, make_tuple (1, 2, 3));
        }
        {
            tuple<int> results = RunAll ([] () -> void {}, [] () { return 3; });
            EXPECT_EQ (results, make_tuple (3));
        }
        {
            int a = 0;
            int b = 1;
            int c = 2;
            RunAll ([&] () { a = 3; }, [&] () { b = 4; }, [&] () { c = 5; });
            EXPECT_EQ (a, 3);
            EXPECT_EQ (b, 4);
            EXPECT_EQ (c, 5);
        }
        {
            static const auto kExcept_ = Execution::Exception{"Test exception"sv};
            auto              thrower  = [] () { Execution::Throw (kExcept_); };
            EXPECT_THROW (RunAll (thrower, [] () { return 3; }), Execution::Exception<>);
        }
#else
        GTEST_SKIP () << "RunAll not tested with this compiler (qCompilerAndStdLib_tsubst_pack_expansion_Buggy)";
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, Function_)
    {
        Debug::TraceContextBumper ctx{"Function_"};
        // Make sure Function<> works as well as std::function
        {
            Function<int (bool)> f = [] ([[maybe_unused]] bool b) -> int { return 3; };
            EXPECT_TRUE (f (true) == 3);
            function<int (bool)> ff = f;
            EXPECT_TRUE (ff (true) == 3);
        }
        // Make sure Function<> serves its one purpose - being comparable
        {
            Function<int (bool)> f1 = [] ([[maybe_unused]] bool b) -> int { return 3; };
            Function<int (bool)> f2 = [] ([[maybe_unused]] bool b) -> int { return 3; };

            EXPECT_TRUE (f1 != f2);
            EXPECT_TRUE (f1 < f2 or f2 < f1); // SEE qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy
            Function<int (bool)> f3 = f1;
            EXPECT_TRUE (f3 == f1);
            EXPECT_TRUE (f3 != f2);
        }
        {
            // https://github.com/SophistSolutions/Stroika/issues/1092 (STK-960)
            // In WTF, really in Execution::IntervalTime code - was getting two functions added with same function pointer.
            // Workaround for https://github.com/SophistSolutions/Stroika/issues/1092 (STK-960) addresses that. But not sure why this doesn't trigger
            // with old code?
            Function<int ()> f1 = [] () { return 1; };
            Function<int ()> f2 = [] () { return -1; };
            EXPECT_TRUE (f1 != f2);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, CommandLine_)
    {
        Debug::TraceContextBumper ctx{"CommandLine_"};
        {
            String           cmdLine = "/bin/sh -c \"a b c\"";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_TRUE (l.size () == 3);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "-c");
            EXPECT_TRUE (l[2] == "a b c");
        }
        {
            String           cmdLine = "";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_TRUE (l.size () == 0);
        }
        {
            String           cmdLine = "/bin/sh -c \'a b c\'";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_TRUE (l.size () == 3);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "-c");
            EXPECT_TRUE (l[2] == "a b c");
        }
        {
            String           cmdLine = "/bin/sh\t b c     -d";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_EQ (l.size (), 4u);
            EXPECT_EQ (l[0], "/bin/sh");
            EXPECT_EQ (l[1], "b");
            EXPECT_EQ (l[2], "c");
            EXPECT_EQ (l[3], "-d");
        }

        {
            const CommandLine::Option kMongoConnectionStringOpt_{.fLongName = "mongoConnectionString"sv, .fSupportsArgument = true};
            CommandLine               cl{"test --mongoConnectionString b"};
            EXPECT_EQ (cl.GetArgument (kMongoConnectionStringOpt_), "b"sv);
            EXPECT_EQ ((cl.ValidateQuietly ({kMongoConnectionStringOpt_})), nullopt);
            CommandLine clBadName{"test --mongoXXX b"};
            EXPECT_EQ (clBadName.GetArgument (kMongoConnectionStringOpt_), nullopt);
            CommandLine clBadName2{"test --mongoConnectionStringXXX b"};
            EXPECT_EQ (clBadName2.GetArgument (kMongoConnectionStringOpt_), nullopt);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, Finally)
    {
        Debug::TraceContextBumper ctx{"Finally"};
        {
            unsigned int cnt = 0;
            {
                [[maybe_unused]] auto&& c = Finally ([&cnt] () noexcept { cnt--; });
                ++cnt;
            }
            EXPECT_EQ (cnt, 0u);
        }
    }
}

namespace {
    namespace Test4_ConstantProperty_ {
        namespace Private_ {
            namespace T1_ {
                static const String                      x{"3"};
                const Execution::LazyInitialized<String> kX = [] () { return x; };
                void                                     DoIt ()
                {
                    const String a = kX;
                }
            }
            namespace T2_ {
                const Execution::LazyInitialized<String> kX = [] () { return "6"; };
                void                                     DoIt ()
                {
                    const String a = kX;
                    EXPECT_TRUE (a == "6"); // Before Stroika 2.1b12 there was a bug that ConstantProperty stored teh constant in a static variable not data member!
                }
            }
            namespace T3_ {
                // @todo get constexpr working - see docs for Execution::LazyInitialized
                //constexpr Execution::LazyInitialized<int> kX = [] () { return 3; };
                const Execution::LazyInitialized<int> kX = [] () { return 3; };
                void                                  DoIt ()
                {
                    const int a [[maybe_unused]] = kX;
                }
            }
            namespace T4_ {
                const Execution::LazyInitialized<int> kX = [] () { return 4; };
                void                                  DoIt ()
                {
                    const int a [[maybe_unused]] = kX;
                    EXPECT_TRUE (a == 4); // Before Stroika 2.1b12 there was a bug that ConstantProperty stored teh constant in a static variable not data member!
                }
            }
        }

    }
    GTEST_TEST (Foundation_Execution, ConstantProperty_)
    {
        Debug::TraceContextBumper ctx{"ConstantProperty_"};
        using namespace Test4_ConstantProperty_;
        Private_::T1_::DoIt ();
        Private_::T2_::DoIt ();
        Private_::T3_::DoIt ();
    }
}

namespace {
    namespace Test5_ModuleGetterSetter_ {
        namespace PRIVATE_ {
            using namespace DataExchange;
            using namespace Execution;
            using namespace Time;
            static const Duration kMinTime_ = 1s;
            struct MyData_ {
                bool               fEnabled = false;
                optional<DateTime> fLastSynchronizedAt;
            };
            struct ModuleGetterSetter_Implementation_MyData_ {
                ModuleGetterSetter_Implementation_MyData_ ()
                    : fOptionsFile_{"MyModule",
                                    [] () -> ObjectVariantMapper {
                                        ObjectVariantMapper mapper;
                                        mapper.AddClass<MyData_> ({
                                            {"Enabled", &MyData_::fEnabled},
                                            {"Last-Synchronized-At", &MyData_::fLastSynchronizedAt},
                                        });
                                        return mapper;
                                    }(),
                                    OptionsFile::kDefaultUpgrader, OptionsFile::mkFilenameMapper ("Put-Your-App-Name-Here")}
                    , fActualCurrentConfigData_{fOptionsFile_.Read<MyData_> (MyData_{})}
                {
                    Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
                }
                MyData_ Get () const
                {
                    return fActualCurrentConfigData_;
                }
                void Set (const MyData_& v)
                {
                    fActualCurrentConfigData_ = v;
                    fOptionsFile_.Write (v);
                }

            private:
                OptionsFile fOptionsFile_;
                MyData_     fActualCurrentConfigData_; // automatically initialized just in time, and externally synchronized
            };

            using Execution::ModuleGetterSetter;
            ModuleGetterSetter<MyData_, ModuleGetterSetter_Implementation_MyData_> sModuleConfiguration_;

            void TestUse1_ ()
            {
                if (sModuleConfiguration_.Get ().fEnabled) {
                    auto n     = sModuleConfiguration_.Get ();
                    n.fEnabled = false;
                    sModuleConfiguration_.Set (n);
                }
            }
            void TestUse2_ ()
            {
                sModuleConfiguration_.Update ([] (MyData_ data) {
                    MyData_ result = data;
                    if (result.fLastSynchronizedAt.has_value () and *result.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) {
                        result.fLastSynchronizedAt = DateTime::Now ();
                    }
                    return result;
                });
            }
            void TestUse3_ ()
            {
                if (sModuleConfiguration_.Update ([] (const MyData_& data) -> optional<MyData_> {
                        if (data.fLastSynchronizedAt.has_value () and *data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) {
                            MyData_ result             = data;
                            result.fLastSynchronizedAt = DateTime::Now ();
                            return result;
                        }
                        return {};
                    })) {
                    // e.g. trigger someone to wakeup and used changes?
                }
            }
        }
    }
    GTEST_TEST (Foundation_Execution, ModuleGetterSetter_)
    {
        Debug::TraceContextBumper ctx{"ModuleGetterSetter_"};
        using namespace Test5_ModuleGetterSetter_;
        Execution::Logger::Activator logMgrActivator; // needed for OptionsFile test
        PRIVATE_::TestUse1_ ();
        PRIVATE_::TestUse2_ ();
        PRIVATE_::TestUse3_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, Environment)
    {
        Debug::TraceContextBumper ctx{"Environment"};
        Mapping<String, String>   env = Execution::kEnvironment;
        EXPECT_TRUE (env.ContainsKey ("PATH"));
        DbgTrace ("env={}"_f, env);
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, CPUAffinity)
    {
        Debug::TraceContextBumper ctx{"CPUAffinity"};
        /*
         *  NB: this test MUTATES the affinity of the running test process, so everything below is wrapped
         *  in a Finally that puts it back. Leaving the process pinned would not fail anything here - it
         *  would quietly serialize every test that runs after this one, which is a far worse failure mode
         *  than an assertion.
         */
        optional<LogicalCPUCoreSet> originally = GetCPUAffinity ();
        [[maybe_unused]] auto&&     cleanup    = Execution::Finally ([&] () noexcept {
            if (originally and not originally->empty ()) {
                (void)SetCPUAffinityQuietly (*originally);
            }
        });

        if constexpr (kCPUAffinitySupported) {
            // we should be able to see what we are allowed to run on, and it must be self-consistent
            EXPECT_TRUE (originally.has_value ());
            if (originally) {
                DbgTrace ("initial affinity={}, logical cores={}"_f, *originally, Common::GetNumberOfLogicalCPUCores ());
                EXPECT_FALSE (originally->empty ()); // we are running, so at least one core is permitted
                for (unsigned int c : *originally) {
                    EXPECT_TRUE (c < Common::GetNumberOfLogicalCPUCores ());
                }
            }

            // pin to one core, and check that the mask we read back is exactly that one core
            optional<unsigned int> pinnedTo = PinToOneLogicalCPUCoreQuietly ();
            EXPECT_TRUE (pinnedTo.has_value ());
            if (pinnedTo) {
                EXPECT_TRUE (originally->Contains (*pinnedTo)); // must have chosen from what we held
                optional<LogicalCPUCoreSet> now = GetCPUAffinity ();
                EXPECT_TRUE (now.has_value ());
                if (now) {
                    EXPECT_TRUE (*now == LogicalCPUCoreSet{*pinnedTo});
                }
            }

            // A core number no platform could represent must fail cleanly - false, not an abort. Deliberately
            // an absurd value rather than 'cores + 1000': the mask ceiling is 32 on Windows x86, 64 on x64
            // and 1024 with glibc, so a modest overshoot passes on some platforms and not others. The first
            // version of this test used +1000, which slipped under glibc's 1024 and tripped an assertion on
            // Windows x86 debug - green on 4 of 5 CI platforms.
            EXPECT_FALSE (SetCPUAffinityQuietly (LogicalCPUCoreSet{1u << 20}));
        }
        else {
            // macOS and friends: every entry point must be a well-behaved no-op, not a crash and not a lie
            EXPECT_FALSE (originally.has_value ());
            EXPECT_FALSE (PinToOneLogicalCPUCoreQuietly ().has_value ());
            EXPECT_FALSE (SetCPUAffinityQuietly (LogicalCPUCoreSet{0}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, ThrowIfNullCheck)
    {
        Debug::TraceContextBumper ctx{"ThrowIfNullCheck"};
        auto                      throwFailureCalls = [] () -> void {
            {
                void* p = nullptr;
                ThrowIfNull (p);
            }
            {
                //static_assert (equality_comparable_with<nullopt_t, optional<int>>);
                optional<int> p;
                ThrowIfNull (p);
            }
        };

        IgnoreExceptionsForCall (throwFailureCalls ());
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, kInnerOuterExceptionStackHandlingWhile)
    {
        Debug::TraceContextBumper     ctx{"kInnerOuterExceptionStackHandlingWhile"};
        constexpr Execution::Activity kActivityOuter_{"OUTER"sv};
        Execution::DeclareActivity    declareActivity{&kActivityOuter_};
        constexpr Execution::Activity kActivityINNER_{"INNER"sv};
        Execution::DeclareActivity    declareActivity2{&kActivityINNER_};
        try {
            Execution::Throw (Execution::Exception<runtime_error>{"oops"});
        }
        catch (...) {
            DbgTrace ("error={}"_f, current_exception ());
        }
    }
}

namespace {
    /*
     *  An IntervalTimer removed - its Adder destroyed - while its callback runs: once removed, the callback is not running, and
     *  it is never called again.
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_RemovedWhileRunning_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_RemovedWhileRunning_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        IntervalTimer::Adder keepRunning{[] () {}, Time::Duration{1h}}; // so ours is not the last timer (removing that also stops the timer thread)
        atomic<unsigned int>           calls{0};
        atomic<bool>                   running{false};
        WaitableEvent                  started;
        optional<IntervalTimer::Adder> adder{in_place,
                                             [&] () {
                                                 running = true;
                                                 ++calls;
                                                 started.Set ();
                                                 Execution::Sleep (500ms); // long enough to be removed while running
                                                 running = false;
                                             },
                                             Time::Duration{50ms}};
        started.Wait (10s);
        adder.reset (); // while its callback runs
        EXPECT_FALSE (running.load ()) << "the callback was still running after its timer was removed";
        unsigned int callsThen = calls;
        Execution::Sleep (1s); // twenty of its intervals
        EXPECT_EQ (calls.load (), callsThen) << "the callback was called again after its timer was removed";
    }

    /*
     *  A callback that removes its own timer - destroys its Adder - does not wait for itself to finish (which would be
     *  forever), and is not called again. Also when it is the last timer, whose removal stops the timer thread: the very
     *  thread removing it.
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_CallbackRemovesItself_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_CallbackRemovesItself_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        for (bool last : {false, true}) {
            optional<IntervalTimer::Adder> keepRunning;
            if (not last) {
                keepRunning.emplace ([] () {}, Time::Duration{1h});
            }
            atomic<unsigned int>           calls{0};
            WaitableEvent                  removed;
            mutex                          adderMutex; // this thread makes the Adder; the callback, on the timer's thread, destroys it
            optional<IntervalTimer::Adder> adder;
            {
                lock_guard lk{adderMutex};
                adder.emplace (
                    [&] () {
                        ++calls;
                        lock_guard lk{adderMutex};
                        adder.reset ();
                        removed.Set ();
                    },
                    Time::Duration{50ms});
            }
            EXPECT_TRUE (removed.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered) << (last ? "the last timer" : "not the last timer");
            Execution::Sleep (500ms); // ten of its intervals
            EXPECT_EQ (calls.load (), 1u) << (last ? "the last timer" : "not the last timer");
        }
    }

    /*
     *  An Adder given a Manager adds its timer to that one - not to Manager::sThe - and removes it from there.
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_AdderGivenManager_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_AdderGivenManager_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        IntervalTimer::Manager            other{Memory::MakeSharedPtr<IntervalTimer::Manager::DefaultRep> ()};
        {
            IntervalTimer::Adder adder{other, [] () {}, Time::Duration{1h}};
            EXPECT_EQ (other.GetAllRegisteredTasks ().size (), 1u);
            EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 0u);
        }
        EXPECT_EQ (other.GetAllRegisteredTasks ().size (), 0u);
    }

    /*
     *  An Adder moved into another: the one assigned to removes its own timer, and takes over the moved one's - not adding it
     *  again - which goes when the Adder now holding it does (the one moved from removes nothing).
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_AdderMoveAssign_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_AdderMoveAssign_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        {
            IntervalTimer::Adder a{[] () {}, Time::Duration{1h}};
            {
                IntervalTimer::Adder b{[] () {}, Time::Duration{2h}};
                EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 2u);
                a = move (b);
                EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 1u);
            }
            EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 1u) << "the Adder moved from removed the timer";
            optional<IntervalTimer::RegisteredTask> t = IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().First ();
            EXPECT_TRUE (t and t->fFrequency == Time::Duration{2h}) << "not the timer moved in"; // (b's: 2 hours, a's 1)
        }
        EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 0u);
    }
}

namespace {
    /*
     *  CallbackRegistry: each callback called in the order added; a removed one never again; one that throws stops none of
     *  the others; and an ID from another registry, or one already removed, removes nothing.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_)
    {
        Debug::TraceContextBumper    ctx{"CallbackRegistry_"};
        CallbackRegistry<void (int)> callbacks;
        vector<int>                  calls;
        auto                         first = callbacks.Add ([&] (int i) { calls.push_back (i); });
        callbacks.Add ([] (int) { Execution::Throw (Execution::Exception<>{"a callback, throwing"sv}); });
        auto last = callbacks.Add ([&] (int i) { calls.push_back (10 * i); });
        EXPECT_NE (first, last);
        callbacks.Call (1);
        EXPECT_EQ (calls, (vector<int>{1, 10}));
        callbacks.Remove (first);
        callbacks.Call (2);
        EXPECT_EQ (calls, (vector<int>{1, 10, 20}));
        CallbackRegistry<void (int)> other;
        callbacks.Remove (other.Add ([] (int) {})); // not one of callbacks'
        callbacks.Remove (first);                   // already removed
        callbacks.Remove ({});                      // names none
        callbacks.Call (3);
        EXPECT_EQ (calls, (vector<int>{1, 10, 20, 30}));
    }

    /*
     *  With a callback running on another thread: Add does not wait for it; Remove does - so once Remove returns, the callback
     *  is not running, and it is never called again.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_Threads_)
    {
        Debug::TraceContextBumper ctx{"CallbackRegistry_Threads_"};
        CallbackRegistry<void ()> callbacks;
        atomic<unsigned int>      calls{0};
        atomic<bool>              running{false};
        WaitableEvent             started;
        WaitableEvent             letGo;
        auto                      id     = callbacks.Add ([&] () {
            ++calls;
            running = true;
            started.Set ();
            letGo.WaitQuietly (10s);
            running = false;
        });
        Thread::Ptr               caller = Thread::New ([&] () { callbacks.Call (); }, Thread::eAutoStart);
        EXPECT_TRUE (started.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered);

        // the callback waits for this thread - so an Add waiting for it would take its 10 seconds
        Time::TimePointSeconds start = Time::GetTickCount ();
        callbacks.Add ([] () {});
        EXPECT_LT ((Time::GetTickCount () - start).count (), 1.0) << "Add waited for a running callback";

        atomic<bool> removed{false};
        Thread::Ptr  remover = Thread::New (
            [&] () {
                callbacks.Remove (id);
                removed = true;
            },
            Thread::eAutoStart);
        Execution::Sleep (200ms); // for Remove to be waiting
        EXPECT_FALSE (removed.load ()) << "Remove did not wait for the running callback";
        letGo.Set ();
        remover.Join ();
        EXPECT_FALSE (running.load ()) << "the callback was still running after Remove returned";
        caller.Join ();
        callbacks.Call ();
        EXPECT_EQ (calls.load (), 1u) << "the callback was called after Remove returned";
    }

    /*
     *  A callback that removes itself does not wait for itself to finish (which would be forever), and is not called again; nor
     *  is one it removes that comes after it, in the very Call that removed it.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_RemoveFromCallback_)
    {
        Debug::TraceContextBumper     ctx{"CallbackRegistry_RemoveFromCallback_"};
        CallbackRegistry<void ()>     callbacks;
        unsigned int                  selfCalls  = 0;
        unsigned int                  laterCalls = 0;
        CallbackRegistry<void ()>::ID self;
        CallbackRegistry<void ()>::ID later;
        self  = callbacks.Add ([&] () {
            ++selfCalls;
            callbacks.Remove (self);
            callbacks.Remove (later);
        });
        later = callbacks.Add ([&] () { ++laterCalls; });
        callbacks.Call ();
        callbacks.Call ();
        EXPECT_EQ (selfCalls, 1u);
        EXPECT_EQ (laterCalls, 0u) << "called in the very Call that removed it";
    }

    /*
     *  RemoveAll waits for a callback running on another thread, and the rest of that Call calls none; called from within a
     *  callback, it does not wait for that one, and none is called again.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_RemoveAll_)
    {
        Debug::TraceContextBumper ctx{"CallbackRegistry_RemoveAll_"};
        {
            CallbackRegistry<void ()> callbacks;
            atomic<unsigned int>      calls{0};
            atomic<bool>              running{false};
            WaitableEvent             started;
            callbacks.Add ([&] () {
                ++calls;
                running = true;
                started.Set ();
                Execution::Sleep (200ms); // so RemoveAll, on this test's thread, finds it running
                running = false;
            });
            callbacks.Add ([&] () { ++calls; });
            Thread::Ptr caller = Thread::New ([&] () { callbacks.Call (); }, Thread::eAutoStart);
            EXPECT_TRUE (started.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered);
            callbacks.RemoveAll ();
            EXPECT_FALSE (running.load ()) << "a callback was still running after RemoveAll returned";
            caller.Join ();
            callbacks.Call ();
            EXPECT_EQ (calls.load (), 1u) << "a callback was called after RemoveAll returned";
        }
        {
            CallbackRegistry<void ()> callbacks;
            unsigned int              calls = 0;
            callbacks.Add ([&] () {
                ++calls;
                callbacks.RemoveAll ();
            });
            callbacks.Add ([&] () { ++calls; });
            callbacks.Call ();
            callbacks.Call ();
            EXPECT_EQ (calls, 1u);
        }
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
