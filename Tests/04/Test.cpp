/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika-Current-Version.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;

using namespace Stroika::TestHarness;

namespace {
    void Test1_Version_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test1_Version_"};
        using namespace Configuration;
        {
            constexpr Version kTestVersion_ = Version (1, 0, VersionStage::Alpha, 1, false);
            VerifyTestResult (kTestVersion_.AsPrettyVersionString () == "1.0a1x");
            VerifyTestResult (kTestVersion_ == Configuration::Version::FromPrettyVersionString ("1.0a1x"));
        }
        VerifyTestResult (Version (1, 0, VersionStage::Release, 0) == Version::FromPrettyVersionString ("1.0"));
        VerifyTestResult (Version (1, 0, VersionStage::Release, 1) == Version::FromPrettyVersionString ("1.0.1"));
        VerifyTestResult (Version (2, 0, VersionStage::Beta, 3) == Version::FromPrettyVersionString ("2.0b3"));

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
        verifier (Version{1, 2, VersionStage::Beta, 4, true}, "1.2b4", "1.2.96.9");
        verifier (Version{3, 0, VersionStage::Release, 0, true}, "3.0", "3.0.160.1");
        verifier (Version{3, 0, VersionStage::Release, 1, true}, "3.0.1", "3.0.160.3");
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
            testRoundTrip (kStroika_Version_FullVersion, kStroika_Version_Major, kStroika_Version_Minor,
                           static_cast<VersionStage> (kStroika_Version_Stage), kStroika_Version_SubStage, kStroika_Version_FinalBuild);
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
    template <>
    constexpr EnumNames<Test2_EnumNames_Private_::fooEnum> DefaultNames<Test2_EnumNames_Private_::fooEnum>::k{
        EnumNames<Test2_EnumNames_Private_::fooEnum>::BasicArrayInitializer{{
            {Test2_EnumNames_Private_::fooEnum::eOne, L"eOne"},
            {Test2_EnumNames_Private_::fooEnum::eTwo, L"eTwo"},
        }}};
}
namespace {
    void Test2_EnumNames_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test2_EnumNames_"};
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
        Debug::TraceContextBumper ctx{"{}::Test3_Endian_"};
        using namespace Configuration;
        VerifyTestResult (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle) == 0xBBAA);
        VerifyTestResult (EndianConverter<uint32_t> (0xAABBCCDD, Endian::eBig, Endian::eLittle) == 0xDDCCBBAA);
    }
}

namespace {
    namespace Test4_SystemConfigruation_ {
        void DoAll ()
        {
            Debug::TraceContextBumper ctx{"{}::Test4_SystemConfigruation_"};
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
        namespace Private_ {
            template <typename T>
            struct CONTAINER {
                using value_type = T;
                template <typename POTENTIALLY_ADDABLE_T>
                class IsAddable_t : public is_convertible<POTENTIALLY_ADDABLE_T, value_type> {};
            };
            template <typename TT>
            void TEST ()
            {
                static_assert (Traversal::IIterable<vector<TT>, Private_::CONTAINER<TT>::template IsAddable_t>);
            }
        }
        void DoAll ()
        {
            Debug::TraceContextBumper ctx{"{}::Test5_SFINAE_Concepts_"};
            using namespace Configuration;

            {
                static_assert (IOperatorLt<int>);
                static_assert (IOperatorLt<pair<int, int>>);
                static_assert (IOperatorLt<SimpleClass>);
                static_assert (IOperatorLt<pair<SimpleClass, SimpleClass>>);
                static_assert (not IOperatorLt<SimpleClassWithoutComparisonOperators>);
                static_assert (not IOperatorLt<SimpleClassWithoutComparisonOperators>);
                static_assert (not IOperatorLt<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>>);
            }
            {
                static_assert (IEqualToOptimizable<int>);
                static_assert (IEqualToOptimizable<pair<int, int>>);
                static_assert (IEqualToOptimizable<SimpleClass>);
                static_assert (IEqualToOptimizable<pair<SimpleClass, SimpleClass>>);
                static_assert (not IEqualToOptimizable<SimpleClassWithoutComparisonOperators>);
                static_assert (not IOperatorEq<SimpleClassWithoutComparisonOperators>);
                static_assert (not IOperatorEq<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>>);

                //tmphack static_assert (not IEqualToOptimizable<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>>);
                {
                    using namespace Stroika::Foundation::Database::SQL::ORM;
                    static_assert (not IOperatorEq<TableProvisioner>);
                    static_assert (not IEqualToOptimizable<TableProvisioner>);
                }
            }
            {
                struct X {};
                static_assert (IOperatorLt<int>);
                static_assert (not IOperatorLt<X>);
                static_assert (IOperatorLt<pair<int, int>>);
                static_assert (IOperatorLt<tuple<int, string>>);
                static_assert (not IOperatorLt<pair<int, X>>);
                static_assert (not IOperatorLt<tuple<X, int>>);
            }
            {
                struct X {};
                static_assert (IOperatorEq<int>);
                static_assert (not IOperatorEq<X>);
                static_assert (IOperatorEq<pair<int, int>>);
                static_assert (IOperatorEq<tuple<int, string>>);
                static_assert (not IOperatorEq<pair<int, X>>);
                static_assert (not IOperatorEq<tuple<X, int>>);
            }
            {
                using Traversal::Iterator;
                static_assert (input_iterator<Iterator<int>>);
                static_assert (input_iterator<Iterator<Characters::Character>>);
                static_assert (input_iterator<Iterator<int>>); // @todo figure out why forward_iterator doesn't work here, but maybe OK
            }
            {
                static_assert (Traversal::IIterableOf<vector<int>, int>);
                static_assert (not Traversal::IIterableOf<vector<int>, char*>);
                static_assert (not Traversal::IIterableOf<vector<int>, String>);
                static_assert (not Traversal::IIterableOf<char, String>);
            }
            {
                // Test from https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
                auto lambda  = [] (int i) { return long (i * 10); };
                using traits = FunctionTraits<decltype (lambda)>;
                static_assert (traits::kArity == 1);
                static_assert (std::is_same<long, traits::result_type>::value);
                static_assert (std::is_same<int, traits::arg<0>::type>::value);
            }
            {
                static_assert (Traversal::IIterable<vector<int>, Private_::CONTAINER<int>::IsAddable_t>);
                static_assert (not Traversal::IIterable<vector<char*>, Private_::CONTAINER<int>::IsAddable_t>);
                Private_::TEST<int> ();
            }
            {
                // verify Configuration::ExtractValueType_t works right
                static_assert (is_same_v<Configuration::ExtractValueType_t<vector<int>>, int>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<int>, void>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<Containers::Collection<char>>, char>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<Traversal::Iterator<string>>, string>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<vector<int>&>, int>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<const vector<int>&>, int>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<vector<int>&&>, int>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<int*>, int>);
                static_assert (is_same_v<Configuration::ExtractValueType_t<const int*>, int>);
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
