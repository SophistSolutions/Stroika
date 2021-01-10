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
#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/Configuration/Endian.h"

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
    void DoRegressionTests_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_"};
        Test_1_SpaceshipAutoGenForOpEqualsForCommonGUIDBug_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
