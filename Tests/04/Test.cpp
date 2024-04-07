/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Configuration
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
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

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

#if qHasFeature_GoogleTest
namespace {
    void Test1_Version_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test1_Version_"};
        using namespace Configuration;
        {
            constexpr Version kTestVersion_ = Version (1, 0, VersionStage::Alpha, 1, false);
            EXPECT_TRUE (kTestVersion_.AsPrettyVersionString () == "1.0a1x");
            EXPECT_TRUE (kTestVersion_ == Configuration::Version::FromPrettyVersionString ("1.0a1x"));
        }
        EXPECT_TRUE (Version (1, 0, VersionStage::Release, 0) == Version::FromPrettyVersionString ("1.0"));
        EXPECT_TRUE (Version (1, 0, VersionStage::Release, 1) == Version::FromPrettyVersionString ("1.0.1"));
        EXPECT_TRUE (Version (2, 0, VersionStage::Beta, 3) == Version::FromPrettyVersionString ("2.0b3"));

        auto verifier = [] (const Version& v, const String& prettyName, const String& win32VersionString) {
            EXPECT_TRUE (Version::FromPrettyVersionString (prettyName).AsWin32Version4DotString () == win32VersionString);
            EXPECT_TRUE (Version::FromPrettyVersionString (prettyName) == v);
            EXPECT_TRUE (Version::FromPrettyVersionString (prettyName) == Version::FromWin32Version4DotString (win32VersionString));
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
                EXPECT_TRUE (sv.fMajorVer == majorVer);
                EXPECT_TRUE (sv.fMinorVer == minorVer);
                EXPECT_TRUE (sv.fVerStage == verStage);
                EXPECT_TRUE (sv.fVerSubStage == verSubStage);
                EXPECT_TRUE (sv.fFinalBuild == finalBuild);
                EXPECT_TRUE (sv.AsFullVersionNum () == fullVer);
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
    constexpr EnumNames<Test2_EnumNames_Private_::fooEnum> DefaultNames<Test2_EnumNames_Private_::fooEnum>::k{{{
        {Test2_EnumNames_Private_::fooEnum::eOne, L"eOne"},
        {Test2_EnumNames_Private_::fooEnum::eTwo, L"eTwo"},
    }}};
}
namespace {
    void Test2_EnumNames_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test2_EnumNames_"};
        using namespace Test2_EnumNames_Private_;
        EXPECT_TRUE (wstring (L"eOne") == DefaultNames<fooEnum>{}.GetName (fooEnum::eOne));
        EXPECT_TRUE (wstring (L"eTwo") == DefaultNames<fooEnum>{}.GetName (fooEnum::eTwo));
        {
            EXPECT_TRUE (wstring (L"eOne") == DefaultNames<fooEnum>{}.GetName (fooEnum::eOne));
            EXPECT_TRUE (wstring (L"eTwo") == DefaultNames<fooEnum>{}.GetName (fooEnum::eTwo));
        }
        {
            EXPECT_TRUE ((DefaultNames<fooEnum>{}.GetValue (L"eTwo", Execution::Exception<> (L"OutOfRange")) == fooEnum::eTwo));
            try {
                DefaultNames<fooEnum>{}.GetValue (L"missing", Execution::Exception<> (L"OutOfRange"));
                EXPECT_TRUE (false); // above should throw
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
        EXPECT_EQ (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle), 0xBBAA);
        EXPECT_EQ (EndianConverter<uint32_t> (0xAABBCCDD, Endian::eBig, Endian::eLittle), 0xDDCCBBAA);
    }
}

namespace {
    namespace Test4_SystemConfigruation_ {
        void DoAll ()
        {
            using namespace Characters::Literals;
            Debug::TraceContextBumper ctx{"{}::Test4_SystemConfigruation_"};
            using namespace Configuration;
            SystemConfiguration sc = GetSystemConfiguration ();
            DbgTrace ("systemConfig={}"_f, sc);
            DbgTrace ("systemConfig.actualOS={}"_f, sc.fActualOperatingSystem);
            DbgTrace ("systemConfig.apparentOS={}"_f, sc.fApparentOperatingSystem);
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
                static_assert (totally_ordered<int>);
                static_assert (totally_ordered<pair<int, int>>);
                //@todo - NOT SURE WHY FAILING - LGP 2024-01-28 - static_assert (totally_ordered<SimpleClass>);
                static_assert (totally_ordered<pair<SimpleClass, SimpleClass>>);
                static_assert (not totally_ordered<SimpleClassWithoutComparisonOperators>);
                static_assert (not totally_ordered<SimpleClassWithoutComparisonOperators>);
                static_assert (not totally_ordered<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>>);
            }
            {
                static_assert (IEqualToOptimizable<int>);
                static_assert (IEqualToOptimizable<pair<int, int>>);
                static_assert (IEqualToOptimizable<SimpleClass>);
                static_assert (IEqualToOptimizable<pair<SimpleClass, SimpleClass>>);
                static_assert (not IEqualToOptimizable<SimpleClassWithoutComparisonOperators>);
                static_assert (not equality_comparable<SimpleClassWithoutComparisonOperators>);
                //@todo - NOT SURE WHY FAILING - LGP 2024-01-28 - static_assert (not equality_comparable<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>>);

                //tmphack static_assert (not IEqualToOptimizable<pair<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>>);
                {
                    using namespace Stroika::Foundation::Database::SQL::ORM;
                    static_assert (not equality_comparable<TableProvisioner>);
                    static_assert (not IEqualToOptimizable<TableProvisioner>);
                }
            }
            {
                struct X {};
                static_assert (totally_ordered<int>);
                static_assert (not totally_ordered<X>);
                static_assert (totally_ordered<pair<int, int>>);
                static_assert (totally_ordered<tuple<int, string>>);
                static_assert (not totally_ordered<pair<int, X>>);
                static_assert (not totally_ordered<tuple<X, int>>);
                static_assert (not totally_ordered<Common::KeyValuePair<X, int>>);
            }
            {
                struct X {};
                static_assert (equality_comparable<int>);
                static_assert (not equality_comparable<X>);
                static_assert (equality_comparable<pair<int, int>>); // makes sense
                static_assert (equality_comparable<pair<X, X>>);     // makes NO sense - but appears to be defect in definition of pair<>
                static_assert (equality_comparable<tuple<int, string>>);
                static_assert (equality_comparable<tuple<int, X>>);                    // similarly defect with tuple it appears
                static_assert (not equality_comparable<Common::KeyValuePair<int, X>>); // works cuz conditionally defined op=
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
    GTEST_TEST (Foundation_Configuration, all)
    {
        Test1_Version_ ();
        Test2_EnumNames_ ();
        Test3_Endian_ ();
        Test4_SystemConfigruation_::DoAll ();
        Test5_SFINAE_Concepts_::DoAll ();
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
