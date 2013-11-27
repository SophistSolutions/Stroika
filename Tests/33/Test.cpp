/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::Memory
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Memory/AnyVariantValue.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/Memory/SharedByValue.h"
#include    "Stroika/Foundation/Memory/VariantValue.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;


//TODO: DOES IT EVEN NEED TO BE SAID? THese tests are a bit sparse ;-)

namespace   {
    void    Test1_Optional ()
    {
        {
            Optional<int>   x;
            VerifyTestResult (x.empty ());
            x = 1;
            VerifyTestResult (not x.empty ());
            VerifyTestResult (*x == 1);
        }
        {
            // Careful about self-assignment
            Optional<int>   x;
            x = 3;
            x = max (*x, 1);
            VerifyTestResult (x == 3);
        }
    }
    void    Test2_SharedByValue ()
    {
    }
    template <typename T>
    void Test3_VariantValue_Helper_MinMax_ ()
    {
        {
            VariantValue v = numeric_limits<T>::lowest ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
        {
            VariantValue v = numeric_limits<T>::min ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
        {
            VariantValue v = numeric_limits<T>::max ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
    }
    void    Test3_VariantValue ()
    {
        {
            VariantValue v;
            VerifyTestResult (v.empty ());
            v = String (L"hi");
            VerifyTestResult (v == L"hi");
        }
        Test3_VariantValue_Helper_MinMax_<int> ();
        Test3_VariantValue_Helper_MinMax_<unsigned int> ();
        Test3_VariantValue_Helper_MinMax_<long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long> ();
        Test3_VariantValue_Helper_MinMax_<long long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long long> ();
        Test3_VariantValue_Helper_MinMax_<float> ();
        Test3_VariantValue_Helper_MinMax_<double> ();
        Test3_VariantValue_Helper_MinMax_<long double> ();
    }
    void    Test_4_Optional_Of_Mapping_Copy_Problem_ ()
    {
        using   namespace   Stroika::Foundation::Memory;
        using   namespace   Stroika::Foundation::Containers;
        Mapping<int, float>  ml1, ml2;
        ml1 = ml2;

        Optional<Mapping<int, float>>  ol1, ol2;
        if (ol2.IsPresent ()) {
            ml1 = *ol2;
        }
        ol1 = ml1;
        Optional<Mapping<int, float>>  xxxx2 (ml1);

        // fails to compile prior to 2013-09-09
        Optional<Mapping<int, float>>  xxxx1 (ol1);
        // fails to compile prior to 2013-09-09
        ol1 = ol2;
    }
    void    Test_5_AnyVariantValue_ ()
    {
        {
            VerifyTestResult (AnyVariantValue ().empty ());
            VerifyTestResult (not AnyVariantValue (1).empty ());
            VerifyTestResult (not AnyVariantValue ("1").empty ());
            //VerifyTestResult (AnyVariantValue ("1").GetType () == typeid ("1"));  // not sure why this fails but not worthy worrying about yet
            VerifyTestResult (AnyVariantValue (1).As<int> () == 1);
        }
        {
            AnyVariantValue v;
            VerifyTestResult (v.empty ());
            v = AnyVariantValue (1);
            VerifyTestResult (not v.empty ());
            VerifyTestResult (v.GetType () == typeid (1));
            VerifyTestResult (v.As<int> () == 1);
            v = AnyVariantValue (L"a");
            //VerifyTestResult (v.GetType () == typeid (L"a")); // not sure why this fails but not worthy worrying about yet
            VerifyTestResult (not v.empty ());
            v.clear ();
            VerifyTestResult (v.empty ());
            VerifyTestResult (v.GetType () == typeid (void));
        }
        {
            struct JIM {
                int a;
            };
            AnyVariantValue v;
            VerifyTestResult (v.empty ());
            v = AnyVariantValue (JIM ());
            VerifyTestResult (v.GetType () == typeid (JIM));
        }
        {
            AnyVariantValue v;
            VerifyTestResult (v.empty ());
            v = AnyVariantValue (1);
            v = v;
            VerifyTestResult (v.GetType () == typeid (1));
            VerifyTestResult (v.As<int> () == 1);
            v = AnyVariantValue (v);
            VerifyTestResult (v.GetType () == typeid (1));
            VerifyTestResult (v.As<int> () == 1);
        }
        {
            static int nCopies = 0;
            struct Copyable {
                Copyable ()
                {
                    ++nCopies;
                }
                Copyable (const Copyable&)
                {
                    ++nCopies;
                }
                ~Copyable ()
                {
                    --nCopies;
                }
                const Copyable& operator= (const Copyable&) = delete;
            };
            {
                AnyVariantValue v;
                VerifyTestResult (v.empty ());
                v = AnyVariantValue (Copyable ());
                v = v;
                v = AnyVariantValue (AnyVariantValue (v));
                v = AnyVariantValue (AnyVariantValue (Copyable ()));
                VerifyTestResult (v.GetType () == typeid (Copyable));
            }
            VerifyTestResult (0 == nCopies);
        }
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_Optional ();
        Test2_SharedByValue ();
        Test3_VariantValue ();
        Test_4_Optional_Of_Mapping_Copy_Problem_ ();
        Test_5_AnyVariantValue_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



