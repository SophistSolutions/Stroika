/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Configuration
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Endian.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/SystemConfiguration.h"
#include "Stroika/Foundation/Configuration/Version.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;

namespace {
    void Test1_Version_ ()
    {
        using namespace Configuration;
        {
            constexpr Version kTestVersion_ = Version (1, 0, VersionStage::Alpha, 1, false);
            VerifyTestResult (kTestVersion_.AsPrettyVersionString () == L"1.0a1x");
            VerifyTestResult (kTestVersion_ == Configuration::Version::FromPrettyVersionString (L"1.0a1x"));
        }
        VerifyTestResult (Version (1, 0, VersionStage::Release, 0) == Version::FromPrettyVersionString (L"1.0"));
        VerifyTestResult (Version (1, 0, VersionStage::Release, 1) == Version::FromPrettyVersionString (L"1.0.1"));
        VerifyTestResult (Version (2, 0, VersionStage::Beta, 3) == Version::FromPrettyVersionString (L"2.0b3"));

        auto verifier = [](const Version& v, const String& prettyName, const String& win32VersionString) {
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
    struct DefaultNames<Test2_EnumNames_Private_::fooEnum> : EnumNames<Test2_EnumNames_Private_::fooEnum> {
        static constexpr EnumNames<Test2_EnumNames_Private_::fooEnum> k{
            EnumNames<Test2_EnumNames_Private_::fooEnum>::BasicArrayInitializer{
                {
                    {Test2_EnumNames_Private_::fooEnum::eOne, L"eOne"},
                    {Test2_EnumNames_Private_::fooEnum::eTwo, L"eTwo"},
                }}};
        DefaultNames ()
            : EnumNames<Test2_EnumNames_Private_::fooEnum> (k)
        {
        }
    };
    constexpr EnumNames<Test2_EnumNames_Private_::fooEnum> DefaultNames<Test2_EnumNames_Private_::fooEnum>::k;
}
namespace {
    void Test2_EnumNames_ ()
    {
        using namespace Test2_EnumNames_Private_;
        VerifyTestResult (wstring (L"eOne") == DefaultNames<fooEnum>::k.GetName (fooEnum::eOne));
        VerifyTestResult (wstring (L"eTwo") == DefaultNames<fooEnum>::k.GetName (fooEnum::eTwo));
        {
            VerifyTestResult (wstring (L"eOne") == DefaultNames<fooEnum> ().GetName (fooEnum::eOne));
            VerifyTestResult (wstring (L"eTwo") == DefaultNames<fooEnum> ().GetName (fooEnum::eTwo));
        }
    }
}

namespace {
    void Test3_Endian_ ()
    {
        using namespace Configuration;
        VerifyTestResult (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle) == 0xBBAA);
        VerifyTestResult (EndianConverter<uint32_t> (0xAABBCCDD, Endian::eBig, Endian::eLittle) == 0xDDCCBBAA);
    }
}

namespace {
    namespace Test4_SystemConfigruation_ {
        void DoAll ()
        {
            using namespace Configuration;
            SystemConfiguration sc = GetSystemConfiguration ();
            DbgTrace (L"systemConfig=%s", Characters::ToString (sc).c_str ());
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
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
