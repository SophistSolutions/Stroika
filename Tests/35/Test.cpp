/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Execution/VirtualConstant.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

namespace {
    void Test1_Function_ ()
    {
        // Make sure Function<> works as well as std::function
        {
            Function<int(bool)> f = [](bool b) -> int { return 3; };
            VerifyTestResult (f (true) == 3);
            function<int(bool)> ff = f;
            VerifyTestResult (ff (true) == 3);
        }
        // Make sure Function<> serves its one purpose - being comparable
        {
            Function<int(bool)> f1 = [](bool b) -> int { return 3; };
            Function<int(bool)> f2 = [](bool b) -> int { return 3; };

            VerifyTestResult (f1 != f2);
            VerifyTestResult (f1 < f2 or f2 < f1);
            Function<int(bool)> f3 = f1;
            VerifyTestResult (f3 == f1);
            VerifyTestResult (f3 != f2);
        }
    }
}

namespace {
    void Test2_CommandLine_ ()
    {
        {
            String           cmdLine = L"/bin/sh -c \"a b c\"";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 3);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"-c");
            VerifyTestResult (l[2] == L"a b c");
        }
        {
            String           cmdLine = L"";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 0);
        }
        {
            String           cmdLine = L"/bin/sh -c \'a b c\'";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 3);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"-c");
            VerifyTestResult (l[2] == L"a b c");
        }
        {
            String           cmdLine = L"/bin/sh\t b c     -d";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 4);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"b");
            VerifyTestResult (l[2] == L"c");
            VerifyTestResult (l[3] == L"-d");
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
                    auto&& c = Finally (
                        [&cnt]() noexcept {
                            cnt--;
                        });
                    cnt++;
                }
                VerifyTestResult (cnt == 0);
            }
        }
    }
}

namespace {
    namespace Test4_VirtualConstant_ {
        namespace Private_ {
            namespace T1_ {
                static const String                            x{L"3"};
                inline const String&                           kX_ () { return x; }
                const Execution::VirtualConstant<String, &kX_> kX;
                void                                           DoIt ()
                {
                    const String a = kX;
                }
            }
            namespace T2_ {
                inline const String& kX_ ()
                {
                    static const String x{L"6"};
                    return x;
                }
                const Execution::VirtualConstant<String, &kX_> kX;
                void                                           DoIt ()
                {
                    const String a = kX;
                }
            }
        }
        void DoAll ()
        {
            Private_::T1_::DoIt ();
            Private_::T2_::DoIt ();
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
                bool     fEnabled = false;
                DateTime fLastSynchronizedAt;
            };
            struct ModuleGetterSetter_Implementation_MyData_ {
                ModuleGetterSetter_Implementation_MyData_ ()
                    : fOptionsFile_{
                          L"MyModule",
                          []() -> ObjectVariantMapper {
                              ObjectVariantMapper mapper;
                              mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                                  {L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fEnabled)},
                                  {L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fLastSynchronizedAt)},
                              });
                              return mapper;
                          }(),
                          OptionsFile::kDefaultUpgrader, OptionsFile::mkFilenameMapper (L"Put-Your-App-Name-Here")}
                    , fActualCurrentConfigData_ (fOptionsFile_.Read<MyData_> (MyData_ ()))
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
#if qCompilerAndStdLib_new_MSFT_optimizer_modulegettersetter_lambda_dates_Buggy
#pragma optimize("", off)
#endif
            void TestUse2_ ()
            {
                sModuleConfiguration_.Update ([](MyData_ data) { MyData_ result = data; if (result.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) { result.fLastSynchronizedAt = DateTime::Now (); } return result; });
            }
            void TestUse3_ ()
            {
                if (sModuleConfiguration_.Update ([](const MyData_& data) -> Optional<MyData_> {  if (data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) { MyData_ result = data; result.fLastSynchronizedAt = DateTime::Now (); return result; } return {}; })) {
                    // e.g. trigger someone to wakeup and used changes?
                }
            }
#if qCompilerAndStdLib_new_MSFT_optimizer_modulegettersetter_lambda_dates_Buggy
#pragma optimize("", on)
#endif
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
    void DoRegressionTests_ ()
    {
        Test1_Function_ ();
        Test2_CommandLine_ ();
        Test3_::DoAll ();
        Test4_VirtualConstant_::DoAll ();
        Test5_ModuleGetterSetter_::DoAll ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
