/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Configuration
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/Configuration/Endian.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Configuration/SystemConfiguration.h"
#include    "Stroika/Foundation/Configuration/Version.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;




namespace   {
    void    Test1_Version_ ()
    {
        const   Configuration::Version  kTestVersion_ =
            Configuration::Version (1, 0, Configuration::VersionStage::Alpha, 1, false
                                   )
            ;
        VerifyTestResult (kTestVersion_.AsPrettyVersionString () == L"1.0a1x");
        VerifyTestResult (kTestVersion_ == Configuration::Version::FromPrettyVersionString (L"1.0a1x"));
        VerifyTestResult (Configuration::Version (1, 0, Configuration::VersionStage::Release, 0) == Configuration::Version::FromPrettyVersionString (L"1.0"));
        VerifyTestResult (Configuration::Version (1, 0, Configuration::VersionStage::Release, 1) == Configuration::Version::FromPrettyVersionString (L"1.0.1"));
        VerifyTestResult (Configuration::Version (2, 0, Configuration::VersionStage::Beta, 3) == Configuration::Version::FromPrettyVersionString (L"2.0b3"));
    }
}



namespace {
    namespace    Test2_EnumNames_Private_ {
        using   namespace Configuration;
        enum class fooEnum {
            eOne,
            eTwo,
            Stroika_Define_Enum_Bounds(eOne, eTwo)
        };
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template<>
            const   EnumNames<Test2_EnumNames_Private_::fooEnum>    DefaultNames<Test2_EnumNames_Private_::fooEnum>::k = EnumNames<Test2_EnumNames_Private_::fooEnum>::BasicArrayInitializer {
                {
                    { Test2_EnumNames_Private_::fooEnum::eOne, L"eOne" },
                    { Test2_EnumNames_Private_::fooEnum::eTwo, L"eTwo" },
                }
            };
        }
    }
}
namespace {
    void    Test2_EnumNames_ ()
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






namespace   {
    void    Test3_Endian_ ()
    {
        using   namespace Configuration;
        VerifyTestResult (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle) == 0xBBAA);
        VerifyTestResult (EndianConverter<uint32_t> (0xAABBCCDD, Endian::eBig, Endian::eLittle) == 0xDDCCBBAA);
    }
}





namespace   {
    namespace Test4_SystemConfigruation_ {
        void    DoAll ()
        {
            using   namespace Configuration;
            SystemConfiguration sc = GetSystemConfiguration ();
            DbgTrace (L"systemConfig=%s", Characters::ToString (sc).c_str ());
        }
    }
}





namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Version_ ();
        Test2_EnumNames_ ();
        Test3_Endian_ ();
        Test4_SystemConfigruation_::DoAll ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



