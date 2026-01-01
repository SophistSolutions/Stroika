/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
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
    GTEST_TEST (Foundation_Execution, Test1_Function_)
    {
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
            // http://stroika-bugs.sophists.com/browse/STK-960
            // In WTF, really in Execution::IntervalTime code - was getting two functions added with same function pointer.
            // Workaround for http://stroika-bugs.sophists.com/browse/STK-960 addresses that. But not sure why this doesn't trigger
            // with old code?
            Function<int ()> f1 = [] () { return 1; };
            Function<int ()> f2 = [] () { return -1; };
            EXPECT_TRUE (f1 != f2);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, Test2_CommandLine_)
    {
        Debug::TraceContextBumper ctx{"Test2_CommandLine_"};
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
    GTEST_TEST (Foundation_Execution, Test4_ConstantProperty_)
    {
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
    GTEST_TEST (Foundation_Execution, ThrowIfNullCheck)
    {
        auto throwFailureCalls = [] () -> void {
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
        constexpr Execution::Activity kActivityOuter_{"OUTER"sv};
        Execution::DeclareActivity    declareActivity{&kActivityOuter_};
        constexpr Execution::Activity kActivityINNER_{"INNER"sv};
        Execution::DeclareActivity    declareActivity2{&kActivityINNER_};
        try {
            Execution::Throw (Execution::RuntimeErrorException{"oops"});
        }
        catch (...) {
            DbgTrace ("error={}"_f, current_exception ());
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
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
