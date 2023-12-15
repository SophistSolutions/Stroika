/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Common
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/Configuration/Endian.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/ObjectFieldUtilities.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;

static_assert (is_same_v<Identity<double>::type, double>);

static_assert (is_same_v<DifferenceType<int>, int>);
static_assert (is_same_v<DifferenceType<double>, double>);

static_assert (is_same_v<UnsignedOfIf<int>, unsigned int>);
static_assert (is_same_v<UnsignedOfIf<double>, double>);
static_assert (is_same_v<UnsignedOfIf<string>, string>);

#if qHasFeature_GoogleTest
namespace {
    void Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ()"};
        {
            Common::GUID guidFromStr{L"61e4d49d-8c26-3480-f5c8-564e155c67a6"};
            Common::GUID guidFromArray{array<uint8_t, 16>{0x9d, 0xd4, 0xe4, 0x61, 0x26, 0x8c, 0x80, 0x34, 0xf5, 0xc8, 0x56, 0x4e, 0x15, 0x5c, 0x67, 0xa6}};
            if (Configuration ::GetEndianness () == Configuration::Endian::eX86) {
                EXPECT_TRUE (::memcmp (&guidFromStr, &guidFromArray, sizeof (Common::GUID)) == 0);
            }
            if (::memcmp (&guidFromStr, &guidFromArray, sizeof (Common::GUID)) == 0) {
                EXPECT_TRUE (guidFromStr == guidFromArray); // fails due to qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
            }
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
                , contentLength2{[] (const auto* property) {
                                     const Headers* thisObj = Memory::GetObjectOwningField (property, &Headers::contentLength2);
                                     return thisObj->fContentLength_;
                                 },
                                 [] (auto* property, auto contentLength) {
                                     Headers* thisObj         = Memory::GetObjectOwningField (property, &Headers::contentLength2);
                                     thisObj->fContentLength_ = contentLength;
                                 }}
                , contentLength3{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) {
                                     const Headers* headerObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
                                     return headerObj->fContentLength_;
                                 },
                                 [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, auto contentLength) {
                                     Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
                                     thisObj->fContentLength_ = contentLength;
                                 }}
                , contentLength4{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) {
                                     const Headers* headerObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength4);
                                     return headerObj->fContentLength_;
                                 },
                                 [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, auto contentLength) {
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
        void Run ()
        {
            Private_::Headers h;
            EXPECT_TRUE (h.contentLength1 == 0);
            h.contentLength1 = 2;
            EXPECT_TRUE (h.contentLength2 == 2);
            h.contentLength2 = 4;
            EXPECT_TRUE (h.contentLength1 == 4);
            Private_::Headers h2 = h;
            EXPECT_TRUE (h2.contentLength1 == 4);
            h.contentLength2 = 5;
            EXPECT_TRUE (h.contentLength1 == 5);
            EXPECT_TRUE (h2.contentLength1 == 4);

            {
                // event handlers
                EXPECT_TRUE (h2.contentLength4 == 4);
                h2.contentLength4 = 5;
                EXPECT_TRUE (h2.contentLength4 == 5);
                bool firstEventHanlderCalled{false};
                h2.contentLength4.rwPropertyChangedHandlers ().push_front ([&] ([[maybe_unused]] const auto& changes) {
                    DbgTrace ("first event handler called");
                    firstEventHanlderCalled = true;
                    return PropertyCommon::PropertyChangedEventResultType::eContinueProcessing;
                });
                h2.contentLength4 = 6;
                EXPECT_TRUE (h2.contentLength4 == 6);
                bool secondEventHanlderCalled{false};
                EXPECT_TRUE (firstEventHanlderCalled);
                h2.contentLength4.rwPropertyChangedHandlers ().push_front ([&] ([[maybe_unused]] const auto& changes) {
                    DbgTrace ("second event handler called");
                    secondEventHanlderCalled = true;
                    return PropertyCommon::PropertyChangedEventResultType::eSilentlyCutOffProcessing;
                });
                h2.contentLength4 = 7;
                EXPECT_TRUE (secondEventHanlderCalled);
                EXPECT_TRUE (h2.contentLength4 == 6); // because event handler returned PropertyChangedEventResultType::eSilentlyCutOffProcessing, this time NO
            }
        }
    }
}

namespace {
    namespace Test3_CommonGUID_ {
        void DoIt ()
        {
            using Common::GUID;
            {
                GUID g1 = GUID::GenerateNew ();
                GUID g2 = GUID::GenerateNew ();
                VerifyTestResultWarning (g1 != g2);
                VerifyTestResultWarning (g1 < g2 or g2 < g1);
            }
            {
                GUID g1 = GUID::GenerateNew ();
                EXPECT_TRUE (GUID{g1.As<Characters::String> ()} == g1);
                EXPECT_TRUE (GUID{g1.As<string> ()} == g1);
                EXPECT_TRUE (GUID{g1.As<Memory::BLOB> ()} == g1);
            }
        }
    }
}

namespace {
    namespace Test4_Compare_ {
        void DoIt ()
        {
            static_assert (IEqualsComparer<equal_to<int>, int>);
            static_assert (not IEqualsComparer<less<int>, int>);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Common, all)
    {
        Debug::TraceContextBumper ctx{"{}::DoRegressionTests_"};
        Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ();
        Test02_Properties_::Run ();
        Test3_CommonGUID_::DoIt ();
        Test4_Compare_::DoIt ();
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
