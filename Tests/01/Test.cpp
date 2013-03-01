/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::Caching
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Cache/LRUCache.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Cache;


namespace   {
    void    Test1_Simple_ ()
    {
        LRUCache<string> tmp (3);
        *tmp.AddNew ("x") = "x";
        *tmp.AddNew ("y") = "y";
        *tmp.AddNew ("z") = "z";
        *tmp.AddNew ("a") = "a";
        VerifyTestResult (tmp.LookupElement ("a") != nullptr);
        VerifyTestResult (tmp.LookupElement ("z") != nullptr);
        VerifyTestResult (tmp.LookupElement ("x") == nullptr);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Simple_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



