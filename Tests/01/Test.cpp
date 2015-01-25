/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
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
    namespace    Test2_Simple_ {
        namespace Private_ {
            void    T1_ ()
            {
                LRUCache<string, string> tmp (3);
                tmp.Add("a", "1");
                tmp.Add("b", "2");
                tmp.Add("c", "3");
                tmp.Add("d", "4");
                VerifyTestResult (tmp.Lookup ("a").IsMissing ());
                VerifyTestResult (tmp.Lookup ("b") == "2");
                VerifyTestResult (tmp.Lookup ("d") == "4");

                LRUCache<string, string> tmp2 = tmp;
                VerifyTestResult (tmp2.Lookup ("a").IsMissing ());
                VerifyTestResult (tmp2.Lookup ("b") == "2");
                VerifyTestResult (tmp2.Lookup ("d") == "4");
            }
            void    T2_ ()
            {
                using   CACHE = LRUCache<string, string, LRUCacheSupport::DefaultTraits<string, 10>>;
                CACHE tmp (3);
                tmp.Add("a", "1");
                tmp.Add("b", "2");
                tmp.Add("c", "3");
                tmp.Add("d", "4");
                VerifyTestResult (tmp.Lookup ("a").IsMissing () or * tmp.Lookup ("a") == "1");  // could be missing or found but if found same value
                VerifyTestResult (tmp.Lookup ("b") == "2");
                VerifyTestResult (tmp.Lookup ("d") == "4");

                CACHE tmp2 = tmp;
                VerifyTestResult (tmp2.Lookup ("a").IsMissing () or * tmp2.Lookup ("a") == "1"); // could be missing or found but if found same value
                VerifyTestResult (tmp2.Lookup ("b") == "2");
                VerifyTestResult (tmp2.Lookup ("d") == "4");
            }

        }
        void    DoIt ()
        {
            Private_::T1_ ();
            Private_::T2_ ();
        }
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test2_Simple_::DoIt ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



