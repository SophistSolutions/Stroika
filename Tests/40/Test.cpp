/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::IO::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/IO/FileSystem/DirectoryIterator.h"
#include    "Stroika/Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO::FileSystem;


namespace   {
    void    Test1_DirectoryIterator_()
    {
        Debug::TraceContextBumper ctx (SDKSTR ("Test1_DirectoryIterator_"));
        {
            Debug::TraceContextBumper ctx (SDKSTR ("simple test"));
            for (DirectoryIterator i { WellKnownLocations::GetTemporary () }; not i.Done (); ++i) {
                DbgTrace (L"filename = %s", i->c_str ());
            }
        }
        {
            Debug::TraceContextBumper ctx (SDKSTR ("t2"));
            DirectoryIterator i { WellKnownLocations::GetTemporary () };
            for (DirectoryIterator i2 = i; not i2.Done (); ++i2) {
                DbgTrace (L"filename = %s", i2->c_str ());
            }
        }
    }
    void    Test2_DirectoryIterable_()
    {
        Debug::TraceContextBumper ctx (SDKSTR ("Test2_DirectoryIterable_"));
        for (String filename : DirectoryIterable (WellKnownLocations::GetTemporary ())) {
            DbgTrace (L"filename = %s", filename.c_str ());
        }
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_DirectoryIterator_ ();
        Test2_DirectoryIterable_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



