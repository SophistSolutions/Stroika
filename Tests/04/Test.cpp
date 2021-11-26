/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Configuration
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Configuration/Endian.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/SystemConfiguration.h"
#include "Stroika/Foundation/Configuration/Version.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika-Current-Version.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;

namespace {
    void Test1_Version_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test1_Version_"};
        using namespace Configuration;
        {
            constexpr Version kTestVersion_ = Version (1, 0, VersionStage::Alpha, 1, false);
            VerifyTestResult (kTestVersion_.AsPrettyVersionString () == L"1.0a1x");
            VerifyTestResult (kTestVersion_ == Configuration::Version::FromPrettyVersionString (L"1.0a1x"));
        }
        VerifyTestResult (Version (1, 0, VersionStage::Release, 0) == Version::FromPrettyVersionString (L"1.0"));
        VerifyTestResult (Version (1, 0, VersionStage::Release, 1) == Version::FromPrettyVersionString (L"1.0.1"));
        VerifyTestResult (Version (2, 0, VersionStage::Beta, 3) == Version::FromPrettyVersionString (L"2.0b3"));

        auto verifier = [] (const Version& v, const String& prettyName, const String& win32VersionString) {
            VerifyTestResult (Version::FromPrettyVersionString (prettyName).AsWin32Version4DotString () == win32VersionString);
            VerifyTestResult (Version::FromPrettyVersionString (prettyName) == v);
            VerifyTestResult (Version::FromPrettyVersionString (prettyName) == Version::FromWin32Version4DotString (win32VersionString));
        };
        /*
         *  FROM EXAMPLE TEXT
         *      So Release 1.2b4 would be (in decimal place separated octets):
         *          1.2.96.9 (in hex 0x1.0x2.0x60.0x9)
         *      So Release 3.0 would be (in decimal place separated octets):
         *          3.0.160.1  (in hex 0x3.0x0.0xa0.0x1)
         *      So Release 3.0.1 would be (in decimal place separated octets):
         *          3.0.160.3  (in hex 0x3.0x0.0xa0.0x3)
         */
        verifier (Version (1, 2, VersionStage::Beta, 4, true), L"1.2b4", L"1.2.96.9");
        verifier (Version (3, 0, VersionStage::Release, 0, true), L"3.0", L"3.0.160.1");
        verifier (Version (3, 0, VersionStage::Release, 1, true), L"3.0.1", L"3.0.160.3");
        {
            auto testRoundTrip = [] (uint32_t fullVer, uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint16_t verSubStage, bool finalBuild) {
                Version sv{fullVer};
                VerifyTestResult (sv.fMajorVer == majorVer);
                VerifyTestResult (sv.fMinorVer == minorVer);
                VerifyTestResult (sv.fVerStage == verStage);
                VerifyTestResult (sv.fVerSubStage == verSubStage);
                VerifyTestResult (sv.fFinalBuild == finalBuild);
                VerifyTestResult (sv.AsFullVersionNum () == fullVer);
            };
            // Could try a variety of these versions, but this should be enough...
            testRoundTrip (kStroika_Version_FullVersion, kStroika_Version_Major, kStroika_Version_Minor, static_cast<VersionStage> (kStroika_Version_Stage), kStroika_Version_SubStage, kStroika_Version_FinalBuild);
        }
    }
}

namespace {
    namespace Test2_EnumNames_Private_ {
        using namespace Configuration;
        enum class fooEnum {
            eOne,
            eTwo,
            Stroika_Define_Enum_Bounds (eOne, eTwo)
        };
    }
}
namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Test2_EnumNames_Private_::fooEnum> DefaultNames<Test2_EnumNames_Private_::fooEnum>::k{
        EnumNames<Test2_EnumNames_Private_::fooEnum>::BasicArrayInitializer{{
            {Test2_EnumNames_Private_::fooEnum::eOne, L"eOne"},
            {Test2_EnumNames_Private_::fooEnum::eTwo, L"eTwo"},
        }}};
}
namespace {
    void Test2_EnumNames_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test2_EnumNames_"};
        using namespace Test2_EnumNames_Private_;
        VerifyTestResult (wstring (L"eOne") == DefaultNames<fooEnum>{}.GetName (fooEnum::eOne));
        VerifyTestResult (wstring (L"eTwo") == DefaultNames<fooEnum>{}.GetName (fooEnum::eTwo));
        {
            VerifyTestResult (wstring (L"eOne") == DefaultNames<fooEnum>{}.GetName (fooEnum::eOne));
            VerifyTestResult (wstring (L"eTwo") == DefaultNames<fooEnum>{}.GetName (fooEnum::eTwo));
        }
        {
            VerifyTestResult ((DefaultNames<fooEnum>{}.GetValue (L"eTwo", Execution::Exception<> (L"OutOfRange")) == fooEnum::eTwo));
            try {
                DefaultNames<fooEnum>{}.GetValue (L"missing", Execution::Exception<> (L"OutOfRange"));
                VerifyTestResult (false); // above should throw
            }
            catch (const Execution::Exception<>&) {
                // good
            }
        }
    }
}

namespace {
    void Test3_Endian_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test3_Endian_"};
        using namespace Configuration;
        VerifyTestResult (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle) == 0xBBAA);
        VerifyTestResult (EndianConverter<uint32_t> (0xAABBCCDD, Endian::eBig, Endian::eLittle) == 0xDDCCBBAA);
    }
}

namespace {
    namespace Test4_SystemConfigruation_ {
        void DoAll ()
        {
            Debug::TraceContextBumper ctx{L"{}::Test4_SystemConfigruation_"};
            using namespace Configuration;
            SystemConfiguration sc = GetSystemConfiguration ();
            DbgTrace (L"systemConfig=%s", Characters::ToString (sc).c_str ());
            DbgTrace (L"systemConfig.actualOS=%s", Characters::ToString (sc.fActualOperatingSystem).c_str ());
            DbgTrace (L"systemConfig.apparentOS=%s", Characters::ToString (sc.fApparentOperatingSystem).c_str ());
        }
    }
}

namespace {
    namespace Test5_SFINAE_Concepts_ {
        void DoAll ()
        {
            Debug::TraceContextBumper ctx{L"{}::Test5_SFINAE_Concepts_"};
            using namespace Configuration;

            {
                static_assert (has_eq_v<int>);
                static_assert (not has_eq_v<SimpleClassWithoutComparisonOperators>);
                static_assert (has_neq_v<int>);
                static_assert (not has_neq_v<SimpleClassWithoutComparisonOperators>);
                static_assert (has_lt_v<int>);
                static_assert (not has_lt_v<SimpleClassWithoutComparisonOperators>);
                static_assert (has_minus_v<int>);
                static_assert (not has_minus_v<SimpleClassWithoutComparisonOperators>);
            }
            {
                // https://stroika.atlassian.net/browse/STK-749
                static_assert (is_detected_v<has_eq_t, std::pair<int, int>>);
                static_assert (HasUsableEqualToOptimization<int> ());
                static_assert (HasUsableEqualToOptimization<pair<int, int>> ());
                static_assert (HasUsableEqualToOptimization<SimpleClass> ());
                static_assert (HasUsableEqualToOptimization<pair<SimpleClass, SimpleClass>> ());
                static_assert (not HasUsableEqualToOptimization<SimpleClassWithoutComparisonOperators> ());
                static_assert (not HasUsableEqualToOptimization<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> ());

                {
                    using namespace Stroika::Foundation::Database::SQL::ORM;
                    static_assert (not has_eq_v<TableProvisioner>);
                    static_assert (not HasUsableEqualToOptimization<TableProvisioner> ());
                }

                // WARNING - see HasUsableEqualToOptimization - below - but this version of has_equal_to returns true, even when it should not, say for
                //  static_assert (!Configuration::has_eq<TableProvisioner>::value);
                //  static_assert (!Configuration::has_equal_to<TableProvisioner>::value);
                //      Seems same bug on VS2k19 and gcc, so almost certainly my bug....
                //      -- LGP 2021-11-22
                // SEE  https://stroika.atlassian.net/browse/STK-749
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_Version_ ();
        Test2_EnumNames_ ();
        Test3_Endian_ ();
        Test4_SystemConfigruation_::DoAll ();
        Test5_SFINAE_Concepts_::DoAll ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
