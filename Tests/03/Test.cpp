/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Configuration
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Configuration/Enumeration.h"
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
    }
}



namespace {
    void    Test2_EnumNames_ ()
    {
        using   namespace Configuration;
        enum class fooEnum {
            eOne,
            eTwo,
        };
        const EnumNames<fooEnum>   Stroika_Enum_Names(fooEnum) = {
            { fooEnum::eOne, L"eOne" },
            { fooEnum::eTwo, L"eTwo" },
        };
        Verify (wstring (L"eOne") == Stroika_Enum_Names(fooEnum).GetName (fooEnum::eOne));
        Verify (wstring (L"eTwo") == Stroika_Enum_Names(fooEnum).GetName (fooEnum::eTwo));
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Version_ ();
        Test2_EnumNames_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



