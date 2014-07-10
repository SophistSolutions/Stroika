/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/DataExchange/Atom.h"
#include    "Stroika/Foundation/DataExchange/OptionsFile.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;






namespace   {
    void    Test1_Atom_()
    {
        {
            Atom<> a = L"d";
            Atom<> b = L"d";
            VerifyTestResult (a == b);
            VerifyTestResult (a.GetPrintName () == L"d");
            VerifyTestResult (a.As<String> () == L"d");
            VerifyTestResult (a.As<wstring> () == L"d");
            VerifyTestResult (not a.empty ());
        }
        {
            VerifyTestResult (Atom<> ().empty ());
        }
        {
            Atom<> a = L"d";
            Atom<> b = L"e";
            VerifyTestResult (a != b);
            VerifyTestResult (not a.empty ());
            Atom<>  c   =   a;
            VerifyTestResult (c == a);
        }
    }
}


namespace {
    void    Test2_OptionsFile_ ()
    {
        struct  MyData_ {
            bool                fEnabled = false;
            DateTime            fLastSynchronizedAt;
        };
        OptionsFile of {
            L"MyModule",
            [] () -> ObjectVariantMapper {
                ObjectVariantMapper mapper;
                mapper.AddClass<MyData_> ({
                    ObjectVariantMapper_StructureFieldInfo_Construction_Helper (MyData_, fEnabled, L"Enabled"),
                    ObjectVariantMapper_StructureFieldInfo_Construction_Helper (MyData_, fLastSynchronizedAt, L"Last-Synchronized-At"),
                });
                return mapper;
            } (),
            OptionsFile::kDefaultUpgrader,
            [] (const String & moduleName, const String & fileSuffix) -> String {
                return  IO::FileSystem::WellKnownLocations::GetTemporary () + moduleName;
            }
        };
        MyData_ m = of.Read<MyData_> (MyData_ ());  // will return default values if file not present
        of.Write (m);                               // test writing
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Atom_ ();
        Test2_OptionsFile_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



