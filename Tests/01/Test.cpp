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
    namespace    Test1_Simple_ {
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





namespace {
    namespace   Test2_LRUCache_ObjWithNoArgCTORs_ {
        namespace Private_ {
            struct TNoCTOR_ {
                TNoCTOR_ (int) {}
                TNoCTOR_ () {} /*= delete*/;
                //TNoCTOR_&  operator= (TNoCTOR_&) = delete;    // not sure we care about this
                bool operator==(const TNoCTOR_& rhs) const { return true; }
            };
        }
        void    DoIt ()
        {
            using Private_::TNoCTOR_;
            LRUCache<TNoCTOR_, TNoCTOR_> test (10);
            test.Add (TNoCTOR_ (), TNoCTOR_ ());
            (void)test.Lookup (TNoCTOR_ ());
        }
    }
}








namespace {
    namespace   Test3_LRUCache_Elements {
        namespace Private_ {
            struct TNoCTOR_ {
                TNoCTOR_ (int) {}
                TNoCTOR_ () {} /*= delete*/;
                //TNoCTOR_&  operator= (TNoCTOR_&) = delete;    // not sure we care about this
                bool operator==(const TNoCTOR_& rhs) const { return true; }
            };
        }
        void    DoIt ()
        {
            LRUCache<string, string> tmp (3);
            tmp.Add("a", "1");
            tmp.Add("b", "2");
            tmp.Add("c", "3");
            tmp.Add("d", "4");
            Containers::Mapping<string, string> x = tmp.Elements ();
            VerifyTestResult (x.length () <= 4);
            for (auto i : tmp.Elements ()) {
                VerifyTestResult (i.fKey == "a" or i.fKey == "b" or i.fKey == "c" or i.fKey == "d");
                VerifyTestResult (i.fValue == "1" or i.fValue == "2" or i.fValue == "3" or i.fValue == "4");
            }
        }
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Simple_::DoIt ();
        Test2_LRUCache_ObjWithNoArgCTORs_::DoIt ();
        Test3_LRUCache_Elements::DoIt ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
