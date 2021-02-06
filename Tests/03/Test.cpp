/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Common
#include "Stroika/Foundation/StroikaPreComp.h"
/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Traveral
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/Configuration/Endian.h"
#include "Stroika/Foundation/Memory/ObjectFieldUtilities.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Traversal;

static_assert (is_same_v<Identity<double>::type, double>);

static_assert (is_same_v<DifferenceType<int>, int>);
static_assert (is_same_v<DifferenceType<double>, double>);

static_assert (is_same_v<UnsignedOfIf<int>, unsigned int>);
static_assert (is_same_v<UnsignedOfIf<double>, double>);
static_assert (is_same_v<UnsignedOfIf<string>, string>);

namespace {
    void Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ()"};
        {
            Common::GUID guidFromStr{L"61e4d49d-8c26-3480-f5c8-564e155c67a6"};
            Common::GUID guidFromArray{array<uint8_t, 16>{0x9d, 0xd4, 0xe4, 0x61, 0x26, 0x8c, 0x80, 0x34, 0xf5, 0xc8, 0x56, 0x4e, 0x15, 0x5c, 0x67, 0xa6}};
            if (Configuration ::GetEndianness () == Configuration::Endian::eX86) {
                VerifyTestResult (memcmp (&guidFromStr, &guidFromArray, sizeof (Common::GUID)) == 0);
            }
            if (memcmp (&guidFromStr, &guidFromArray, sizeof (Common::GUID)) == 0) {
                VerifyTestResult (guidFromStr == guidFromArray); // fails due to qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
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
                Headers (Headers&& src);
                nonvirtual Headers& operator= (const Headers& rhs) = default; // properties are assignable, so this is OK
                nonvirtual Headers& operator                       = (Headers&& rhs);

                Property<unsigned int> contentLength1; // both refer to the private fContentLength_ field
                Property<unsigned int> contentLength2;
                Property<unsigned int> contentLength3;

            private:
                unsigned int fContentLength_{0};
            };
            Headers::Headers ()
                // Can implement getter/setters with this capture (wastes a bit of space)
                : contentLength1{
                      [this] ([[maybe_unused]] const auto* property) {
                          return fContentLength_;
                      },
                      [this] ([[maybe_unused]] auto* property, const auto& contentLength) {
                          fContentLength_ = contentLength;
                      }}
                // Can implement getter/setters with Memory::GetObjectOwningField - to save space, but counts on exact
                // storage layout and not totally legal with non- is_standard_layout<> - see Memory::GetObjectOwningField
                , contentLength2{
                      [] (const auto* property) {
                          const Headers* headerObj = Memory::GetObjectOwningField (property, &Headers::contentLength2);
                          return headerObj->fContentLength_;
                      },
                      [] (auto* property, auto contentLength) {
                          Headers* headerObj         = Memory::GetObjectOwningField (property, &Headers::contentLength2);
                          headerObj->fContentLength_ = contentLength;
                      }}
                , contentLength3{
                      [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) {
                          const Headers* headerObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
                          return headerObj->fContentLength_;
                      },
                      [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, auto contentLength) {
                          Headers* headerObj         = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
                          headerObj->fContentLength_ = contentLength;
                      }}
            {
            }
            Headers::Headers (const Headers& src)
                : Headers () // do default initialization of properties
            {
                // NOTE - cannot INITIALIZE properties with src.Properties values since they are not copy constructible
                // but they are assignable, so do that
                contentLength1 = src.contentLength1;
                contentLength2 = src.contentLength2;
                // COULD EITHER initialize fContentLength_ or pContentLength1/pContentLength2 - but no need to do both
            }
            Headers::Headers (Headers&& src)
                : Headers () // do default initialization of properties
            {
                // NOTE - cannot MOVE properties with src.Properties values since they are not copy constructible
                // but they are assignable, so do that
                contentLength1 = src.contentLength1;
                contentLength2 = src.contentLength2;
                // COULD EITHER initialize fContentLength_ or pContentLength1/pContentLength2 - but no need to do both
            }
            Headers& Headers::operator= (Headers&& rhs)
            {
                // Could copy either properties or underlying field - no matter which
                fContentLength_ = rhs.fContentLength_;
                return *this;
            }
        }
        void Run ()
        {
            Private_::Headers h;
            VerifyTestResult (h.contentLength1 == 0);
            h.pContentLength1 = 2;
            VerifyTestResult (h.contentLength2 == 2);
            h.pContentLength2 = 4;
            VerifyTestResult (h.contentLength1 == 4);
            Private_::Headers h2 = h;
            VerifyTestResult (h2.contentLength1 == 4);
            h.pContentLength2 = 5;
            VerifyTestResult (h.contentLength1 == 5);
            VerifyTestResult (h2.contentLength1 == 4);
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_"};
        Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ();
        Test02_Properties_::Run ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
