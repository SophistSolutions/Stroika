/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Common
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Endian.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/Common/Version.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/ObjectFieldUtilities.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "Stroika/Current-Version.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

static_assert (same_as<DifferenceType<int>, int>);
static_assert (same_as<DifferenceType<double>, double>);

static_assert (same_as<UnsignedOfIf<int>, unsigned int>);
static_assert (same_as<UnsignedOfIf<double>, double>);
static_assert (same_as<UnsignedOfIf<string>, string>);

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Common, SpaceshipAutoGenForOpEqualsForCommonGUIDBug_)
    {
        Debug::TraceContextBumper ctx{"{}::SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ()"};
        {
            Common::GUID guidFromStr{"61e4d49d-8c26-3480-f5c8-564e155c67a6"};
            DbgTrace ("GUID={}"_f, guidFromStr);
            Common::GUID guidFromArray{array<uint8_t, 16>{0x9d, 0xd4, 0xe4, 0x61, 0x26, 0x8c, 0x80, 0x34, 0xf5, 0xc8, 0x56, 0x4e, 0x15, 0x5c, 0x67, 0xa6}};
            if (Common ::GetEndianness () == Common::Endian::eX86) {
                EXPECT_EQ (::memcmp (&guidFromStr, &guidFromArray, sizeof (Common::GUID)), 0);
            }
            if (::memcmp (&guidFromStr, &guidFromArray, sizeof (Common::GUID)) == 0) {
                EXPECT_EQ (guidFromStr, guidFromArray); // fails due to qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
            }
        }
        {
            using namespace Characters;
            Common::GUID guidFromStr{"61e4d49d-8c26-3480-f5c8-564e155c67a6"};
            EXPECT_EQ ("{}"_f(guidFromStr), "61e4d49d-8c26-3480-f5c8-564e155c67a6");
        }
    }
}

namespace {
    namespace Test02_Properties_ {
        namespace Private_ {
            struct Headers {
            public:
                Headers ();
                Headers (const Headers& src);
                Headers (Headers&& src) noexcept;
                nonvirtual Headers& operator= (const Headers& rhs) = default; // properties are assignable, so this is OK
                nonvirtual Headers& operator= (Headers&& rhs) noexcept;

                Property<unsigned int>           contentLength1; // both refer to the private fContentLength_ field
                Property<unsigned int>           contentLength2;
                Property<unsigned int>           contentLength3;
                ExtendableProperty<unsigned int> contentLength4;

            private:
                unsigned int fContentLength_{0};
            };
            Headers::Headers ()
                // Can implement getter/setters with this capture (wastes a bit of space)
                : contentLength1{[this] ([[maybe_unused]] const auto* property) { return fContentLength_; },
                                 [this] ([[maybe_unused]] auto* property, const auto& contentLength) { fContentLength_ = contentLength; }}
                // Can implement getter/setters with Memory::GetObjectOwningField - to save space, but counts on exact
                // storage layout and not totally legal with non- is_standard_layout<> - see Memory::GetObjectOwningField
                , contentLength2{[] ([[maybe_unused]] const auto* property) {
                                     const Headers* thisObj = Memory::GetObjectOwningField (property, &Headers::contentLength2);
                                     return thisObj->fContentLength_;
                                 },
                                 [] ([[maybe_unused]] auto* property, auto contentLength) {
                                     Headers* thisObj         = Memory::GetObjectOwningField (property, &Headers::contentLength2);
                                     thisObj->fContentLength_ = contentLength;
                                 }}
                , contentLength3{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                                     const Headers* headerObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
                                     return headerObj->fContentLength_;
                                 },
                                 [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto contentLength) {
                                     Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
                                     thisObj->fContentLength_ = contentLength;
                                 }}
                , contentLength4{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                                     const Headers* headerObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength4);
                                     return headerObj->fContentLength_;
                                 },
                                 [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto contentLength) {
                                     Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength4);
                                     thisObj->fContentLength_ = contentLength;
                                 }}
            {
            }
            Headers::Headers (const Headers& src)
                : Headers{} // do default initialization of properties
            {
                // NOTE - cannot INITIALIZE properties with src.Properties values since they are not copy constructible
                // but they are assignable, so do that
                contentLength1 = src.contentLength1;
                contentLength2 = src.contentLength2;
                // COULD EITHER initialize fContentLength_ or pContentLength1/pContentLength2 - but no need to do both
            }
            Headers::Headers (Headers&& src) noexcept
                : Headers{} // do default initialization of properties
            {
                // NOTE - cannot MOVE properties with src.Properties values since they are not copy constructible
                // but they are assignable, so do that
                contentLength1 = src.contentLength1;
                contentLength2 = src.contentLength2;
                // COULD EITHER initialize fContentLength_ or pContentLength1/pContentLength2 - but no need to do both
            }
            Headers& Headers::operator= (Headers&& rhs) noexcept
            {
                // Could copy either properties or underlying field - no matter which
                fContentLength_ = rhs.fContentLength_;
                return *this;
            }
        }
    }
    GTEST_TEST (Foundation_Common, Properties_)
    {
        Debug::TraceContextBumper ctx{"{}::Properties_"};
        using namespace Test02_Properties_;
        Private_::Headers h;
        EXPECT_EQ (h.contentLength1, 0u);
        h.contentLength1 = 2;
        EXPECT_EQ (h.contentLength2, 2u);
        h.contentLength2 = 4;
        EXPECT_EQ (h.contentLength1, 4u);
        Private_::Headers h2 = h;
        EXPECT_EQ (h2.contentLength1, 4u);
        h.contentLength2 = 5;
        EXPECT_EQ (h.contentLength1, 5u);
        EXPECT_EQ (h2.contentLength1, 4u);

        {
            // event handlers
            EXPECT_EQ (h2.contentLength4, 4u);
            h2.contentLength4 = 5;
            EXPECT_EQ (h2.contentLength4, 5u);
            bool firstEventHandlerCalled{false};
            h2.contentLength4.rwPropertyChangedHandlers ().push_front ([&] ([[maybe_unused]] const auto& changes) {
                DbgTrace ("first event handler called"_f);
                firstEventHandlerCalled = true;
                return PropertyCommon::PropertyChangedEventResultType::eContinueProcessing;
            });
            h2.contentLength4 = 6;
            EXPECT_EQ (h2.contentLength4, 6u);
            bool secondEventHanlderCalled{false};
            EXPECT_TRUE (firstEventHandlerCalled);
            h2.contentLength4.rwPropertyChangedHandlers ().push_front ([&] ([[maybe_unused]] const auto& changes) {
                DbgTrace ("second event handler called"_f);
                secondEventHanlderCalled = true;
                return PropertyCommon::PropertyChangedEventResultType::eSilentlyCutOffProcessing;
            });
            h2.contentLength4 = 7;
            EXPECT_TRUE (secondEventHanlderCalled);
            EXPECT_EQ (h2.contentLength4, 6u); // because event handler returned PropertyChangedEventResultType::eSilentlyCutOffProcessing, this time NO
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Common, CommonGUID_)
    {
        Debug::TraceContextBumper ctx{"{}::CommonGUID_"};
        using Common::GUID;
        {
            GUID g1 = GUID::GenerateNew ();
            GUID g2 = GUID::GenerateNew ();
            VerifyTestResultWarning (g1 != g2);
            VerifyTestResultWarning (g1 < g2 or g2 < g1);
        }
        {
            GUID g1 = GUID::GenerateNew ();
            EXPECT_EQ (GUID{g1.As<Characters::String> ()}, g1);
            EXPECT_EQ (GUID{g1.As<string> ()}, g1);
            EXPECT_EQ (GUID{g1.As<Memory::BLOB> ()}, g1);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Common, Compare_)
    {
        Debug::TraceContextBumper ctx{"{}::Compare_"};
        static_assert (IEqualsComparer<equal_to<int>, int>);
        static_assert (not IEqualsComparer<less<int>, int>);
    }
}

namespace {
    GTEST_TEST (Foundation_Common, Version_)
    {
        Debug::TraceContextBumper ctx{"{}::Version_"};
        using namespace Characters;
        using namespace Common;
        {
            constexpr Version kTestVersion_ = Version (1, 0, VersionStage::Alpha, 1, false);
            EXPECT_TRUE (kTestVersion_.AsPrettyVersionString () == "1.0a1x");
            EXPECT_TRUE (kTestVersion_ == Common::Version::FromPrettyVersionString ("1.0a1x"));
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
        using namespace Common;
        enum class fooEnum {
            eOne,
            eTwo,
            Stroika_Define_Enum_Bounds (eOne, eTwo)
        };
    }
}
namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<Test2_EnumNames_Private_::fooEnum> DefaultNames<Test2_EnumNames_Private_::fooEnum>::k{{{
        {Test2_EnumNames_Private_::fooEnum::eOne, L"eOne"},
        {Test2_EnumNames_Private_::fooEnum::eTwo, L"eTwo"},
    }}};
}
namespace {
    GTEST_TEST (Foundation_Common, EnumNames_)
    {
        Debug::TraceContextBumper ctx{"{}::EnumNames_"};
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
    GTEST_TEST (Foundation_Common, Endian_)
    {
        Debug::TraceContextBumper ctx{"{}::Endian_"};
        using namespace Common;
        EXPECT_EQ (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle), 0xBBAA);
        EXPECT_EQ (EndianConverter<uint32_t> (0xAABBCCDD, Endian::eBig, Endian::eLittle), 0xDDCCBBAA);
    }
}

namespace {
    GTEST_TEST (Foundation_Common, SystemConfigruation_)
    {
        using namespace Characters::Literals;
        Debug::TraceContextBumper ctx{"{}::SystemConfigruation_"};
        using namespace Common;
        SystemConfiguration sc = GetSystemConfiguration ();
        DbgTrace ("systemConfig={}"_f, sc);
        DbgTrace ("systemConfig.actualOS={}"_f, sc.fActualOperatingSystem);
        DbgTrace ("systemConfig.apparentOS={}"_f, sc.fApparentOperatingSystem);
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
    }
    GTEST_TEST (Foundation_Common, SFINAE_Concept)
    {
        Debug::TraceContextBumper ctx{"{}::SFINAE_Concept"};
        using namespace Test5_SFINAE_Concepts_;

        {
            static_assert (totally_ordered<int>);
            static_assert (totally_ordered<pair<int, int>>);
            //@todo - NOT SURE WHY FAILING - LGP 2024-01-28 - static_assert (totally_ordered<OnlyCopyableMoveableAndTotallyOrdered>);
            static_assert (totally_ordered<pair<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>>);
            static_assert (not totally_ordered<OnlyCopyableMoveable>);
            static_assert (not totally_ordered<OnlyCopyableMoveable>);
            static_assert (not totally_ordered<pair<OnlyCopyableMoveable, OnlyCopyableMoveable>>);
        }
        {
            static_assert (IEqualToOptimizable<int>);
            static_assert (IEqualToOptimizable<pair<int, int>>);
            static_assert (IEqualToOptimizable<OnlyCopyableMoveableAndTotallyOrdered>);
            static_assert (IEqualToOptimizable<pair<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>>);
            static_assert (not IEqualToOptimizable<OnlyCopyableMoveable>);
            static_assert (not equality_comparable<OnlyCopyableMoveable>);
            //@todo - NOT SURE WHY FAILING - LGP 2024-01-28 - static_assert (not equality_comparable<pair<OnlyCopyableMoveable, OnlyCopyableMoveable>>);

            //tmphack static_assert (not IEqualToOptimizable<pair<OnlyCopyableMoveable, OnlyCopyableMoveable>>);
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
            struct X_ {};
            static_assert (equality_comparable<int>);
            static_assert (not equality_comparable<X_>);
            static_assert (equality_comparable<pair<int, int>>); // makes sense
            //???     static_assert (equality_comparable<pair<X_, X_>>);     // makes NO sense - but appears to be defect in definition of pair<>
            // - fixed in C++26 according to https://en.cppreference.com/w/cpp/utility/pair/operator_cmp - undefined in C++14 and later so dont check
            static_assert (equality_comparable<tuple<int, string>>);
            //??      static_assert (equality_comparable<tuple<int, X_>>);                    // similarly defect with tuple it appears
            static_assert (not equality_comparable<Common::KeyValuePair<int, X_>>); // works cuz conditionally defined operator==
        }
        {
            using Traversal::Iterator;
            static_assert (input_iterator<Iterator<int>>);
            static_assert (input_iterator<Iterator<Characters::Character>>);
            static_assert (input_iterator<Iterator<int>>); // @todo figure out why forward_iterator doesn't work here, but maybe OK
        }
        {
            static_assert (Traversal::IIterableOfTo<vector<int>, int>);
            static_assert (not Traversal::IIterableOfTo<vector<int>, char*>);
            static_assert (not Traversal::IIterableOfTo<vector<int>, String>);
            static_assert (not Traversal::IIterableOfTo<char, String>);
        }
        {
            // Test from https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
            auto lambda  = [] (int i) { return long (i * 10); };
            using traits = FunctionTraits<decltype (lambda)>;
            static_assert (traits::kArity == 1);
            static_assert (same_as<long, traits::result_type>);
            static_assert (same_as<int, traits::arg<0>::type>);
        }
        {
            static_assert (Traversal::IIterable<vector<int>, Private_::CONTAINER<int>::IsAddable_t>);
            static_assert (not Traversal::IIterable<vector<char*>, Private_::CONTAINER<int>::IsAddable_t>);
            Private_::TEST<int> ();
        }
        {
            // verify Configuration::ExtractValueType_t works right
            static_assert (same_as<Common::ExtractValueType_t<vector<int>>, int>);
            static_assert (same_as<Common::ExtractValueType_t<int>, void>);
            static_assert (same_as<Common::ExtractValueType_t<Containers::Collection<char>>, char>);
            static_assert (same_as<Common::ExtractValueType_t<Traversal::Iterator<string>>, string>);
            static_assert (same_as<Common::ExtractValueType_t<vector<int>&>, int>);
            static_assert (same_as<Common::ExtractValueType_t<const vector<int>&>, int>);
            static_assert (same_as<Common::ExtractValueType_t<vector<int>&&>, int>);
            static_assert (same_as<Common::ExtractValueType_t<int*>, int>);
            static_assert (same_as<Common::ExtractValueType_t<const int*>, int>);
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
