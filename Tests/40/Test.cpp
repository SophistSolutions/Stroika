/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::IO::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO::FileSystem;


namespace   {
    void    Test1_()
    {
        for (String filename : DirectoryIterable (WellKnownLocations::GetTemporary ())) {
            DbgTrace (L"filename = %s", filename.c_str ());
        }
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



