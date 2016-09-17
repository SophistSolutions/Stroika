/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::IO::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/IO/FileSystem/DirectoryIterator.h"
#include    "Stroika/Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "Stroika/Foundation/IO/FileSystem/PathName.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO::FileSystem;


namespace   {
    void    Test1_DirectoryIterator_()
    {
        Debug::TraceContextBumper ctx ("Test1_DirectoryIterator_");
        {
            Debug::TraceContextBumper ctx ("simple test");
            for (DirectoryIterator i { WellKnownLocations::GetTemporary () }; not i.Done (); ++i) {
                DbgTrace (L"filename = %s", i->c_str ());
            }
        }
        {
            Debug::TraceContextBumper ctx ("t2");
            DirectoryIterator i { WellKnownLocations::GetTemporary () };
            for (DirectoryIterator i2 = i; not i2.Done (); ++i2) {
                DbgTrace (L"filename = %s", i2->c_str ());
            }
        }
    }
    void    Test2_DirectoryIterable_()
    {
        Debug::TraceContextBumper ctx ("Test2_DirectoryIterable_");
        for (String filename : DirectoryIterable (WellKnownLocations::GetTemporary ())) {
            DbgTrace (L"filename = %s", filename.c_str ());
        }
    }
}


namespace {
    namespace   Test3_Pathnames_ {
        void    Test_ExtractDirAndBaseName_ ()
        {
            // Tests from DOCS line in ExtractDirAndBaseName
#if     qPlatform_POSIX
            VerifyTestResult ((ExtractDirAndBaseName (L"/usr/lib") == pair<String, String> { L"/usr/", L"lib" }));
            VerifyTestResult ((ExtractDirAndBaseName (L"/usr/") == pair<String, String> { L"/", L"usr/" }));
            VerifyTestResult ((ExtractDirAndBaseName (L"usr") == pair<String, String> { L"./", L"usr" }));
            VerifyTestResult ((ExtractDirAndBaseName (L"/") == pair<String, String> { L"/", L"" }));
            VerifyTestResult ((ExtractDirAndBaseName (L".") == pair<String, String> { L"./", L"." }));
            VerifyTestResult ((ExtractDirAndBaseName (L"..") == pair<String, String> { L"./", L".." }));
#elif   qPlatform_Windows
            VerifyTestResult ((ExtractDirAndBaseName (L"\\usr\\lib") == pair<String, String> { L"\\usr\\", L"lib" }));
            VerifyTestResult ((ExtractDirAndBaseName (L"\\usr\\") == pair<String, String> { L"\\", L"usr\\" }));
            VerifyTestResult ((ExtractDirAndBaseName (L"usr") == pair<String, String> { L".\\", L"usr" }));
            VerifyTestResult ((ExtractDirAndBaseName (L"\\") == pair<String, String> { L"\\", L"" }));
            VerifyTestResult ((ExtractDirAndBaseName (L".") == pair<String, String> { L".\\", L"." }));
            VerifyTestResult ((ExtractDirAndBaseName (L"..") == pair<String, String> { L".\\", L".." }));
            VerifyTestResult ((ExtractDirAndBaseName (L"c:\\h\\m.t") == pair<String, String> { L"c:\\h\\", L"m.t" }));
#endif
        }
        void    Test_GetFileBaseName_ ()
        {
            VerifyTestResult (GetFileBaseName (L"foo") == L"foo");
            VerifyTestResult (GetFileBaseName (L"foo.cpp") == L"foo");
            VerifyTestResult (GetFileBaseName (L"foo.exe") == L"foo");
            VerifyTestResult (GetFileBaseName (L".exe") == L".exe");
#if     qPlatform_POSIX
            VerifyTestResult (GetFileBaseName (L"/tmp/.CPUBurner") == L".CPUBurner");
#elif   qPlatform_Windows
            VerifyTestResult (GetFileBaseName (L"c:\\tmp\\.CPUBurner") == L".CPUBurner");
#endif
        }

        void    DoTest ()
        {
            Test_ExtractDirAndBaseName_ ();
            Test_GetFileBaseName_ ();
        }
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_DirectoryIterator_ ();
        Test2_DirectoryIterable_ ();
        Test3_Pathnames_::DoTest ();
    }
}



int		main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
