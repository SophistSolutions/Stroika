/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
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
    void Test1_Function_ ()
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
            // https://stroika.atlassian.net/browse/STK-960
            // In WTF, really in Execution::IntervalTime code - was getting two functions added with same function pointer.
            // Workaround for https://stroika.atlassian.net/browse/STK-960 addresses that. But not sure why this doesn't trigger
            // with old code?
            Function<int ()> f1 = [] () { return 1; };
            Function<int ()> f2 = [] () { return -1; };
            EXPECT_TRUE (f1 != f2);
        }
    }
}

namespace {
    void Test2_CommandLine_ ()
    {
        {
            String           cmdLine = "/bin/sh -c \"a b c\"";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            EXPECT_TRUE (l.size () == 3);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "-c");
            EXPECT_TRUE (l[2] == "a b c");
        }
        {
            String           cmdLine = "";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            EXPECT_TRUE (l.size () == 0);
        }
        {
            String           cmdLine = "/bin/sh -c \'a b c\'";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            EXPECT_TRUE (l.size () == 3);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "-c");
            EXPECT_TRUE (l[2] == "a b c");
        }
        {
            String           cmdLine = "/bin/sh\t b c     -d";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            EXPECT_TRUE (l.size () == 4);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "b");
            EXPECT_TRUE (l[2] == "c");
            EXPECT_TRUE (l[3] == "-d");
        }
    }
}

namespace {
    namespace Test3_ {
        void DoAll ()
        {
            {
                unsigned int cnt = 0;
                {
                    [[maybe_unused]] auto&& c = Finally ([&cnt] () noexcept { cnt--; });
                    ++cnt;
                }
                EXPECT_TRUE (cnt == 0);
            }
        }
    }
}

namespace {
    namespace Test4_ConstantProperty_ {
        namespace Private_ {
            namespace T1_ {
                static const String                    x{"3"};
                const Common::ConstantProperty<String> kX = [] () { return x; };
                void                                   DoIt ()
                {
                    const String a = kX;
                }
            }
            namespace T2_ {
                const Common::ConstantProperty<String> kX = [] () { return "6"; };
                void                                   DoIt ()
                {
                    const String a = kX;
                    EXPECT_TRUE (a == "6"); // Before Stroika 2.1b12 there was a bug that ConstantProperty stored teh constant in a static variable not data member!
                }
            }
            namespace T3_ {
                // @todo get constexpr working - see docs for Common::ConstantProperty
                //constexpr Common::ConstantProperty<int> kX = [] () { return 3; };
                const Common::ConstantProperty<int> kX = [] () { return 3; };
                void                                DoIt ()
                {
                    const int a [[maybe_unused]] = kX;
                }
            }
            namespace T4_ {
                const Common::ConstantProperty<int> kX = [] () { return 4; };
                void                                DoIt ()
                {
                    const int a [[maybe_unused]] = kX;
                    EXPECT_TRUE (a == 4); // Before Stroika 2.1b12 there was a bug that ConstantProperty stored teh constant in a static variable not data member!
                }
            }
        }
        void DoAll ()
        {
            Private_::T1_::DoIt ();
            Private_::T2_::DoIt ();
            Private_::T3_::DoIt ();
        }
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
                                        mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                                            {"Enabled", StructFieldMetaInfo{&MyData_::fEnabled}},
                                            {"Last-Synchronized-At", StructFieldMetaInfo{&MyData_::fLastSynchronizedAt}},
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
        void DoAll ()
        {
            PRIVATE_::TestUse1_ ();
            PRIVATE_::TestUse2_ ();
            PRIVATE_::TestUse3_ ();
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Execution::Logger::Activator logMgrActivator; // needed for OptionsFile test
        Test1_Function_ ();
        Test2_CommandLine_ ();
        Test3_::DoAll ();
        Test4_ConstantProperty_::DoAll ();
        Test5_ModuleGetterSetter_::DoAll ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
