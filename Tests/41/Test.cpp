/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Math
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Math/Angle.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Math/Overlap.h"
#include    "Stroika/Foundation/Math/ReBin.h"
#include    "Stroika/Foundation/Math/Statistics.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Math;







namespace {
    // test helper to assure answer for (A,B) is same as (B,A) - commutative
    template    <typename T>
    bool    VerifyOverlapIsCommutative_ (const pair<T, T>& p1, const pair<T, T>& p2)
    {
        bool r  = Overlaps<T> (p1, p2);
        VerifyTestResult (r == Overlaps<T> (p2, p1));
        return r;
    }
}


namespace   {
    void    Test1_Overlap_ ()
    {
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (2, 2)));
        VerifyTestResult (not VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (3, 4)));
        VerifyTestResult (not VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (0, 1)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (1, 1)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 10), pair<int, int> (3, 4)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 10), pair<int, int> (3, 3)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (5, 10), pair<int, int> (3, 7)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (5, 10), pair<int, int> (5, 5)));
    }
    void    Test2_Round_ ()
    {
        // really could use more cases!!!
        VerifyTestResult (RoundUpTo (2, 10) == 10);
        VerifyTestResult (RoundDownTo (2, 10) == 0);
        VerifyTestResult (RoundUpTo (2, 2) == 2);
        VerifyTestResult (RoundDownTo (2, 2) == 2);
    }
    void    Test3_Angle_ ()
    {
        // really could use more cases!!!
        VerifyTestResult (Angle (1.1) + Angle (1.1) < Angle (2.3));
        VerifyTestResult (Angle (1.1) + Angle (1.1) < Angle (360, Angle::AngleFormat::eDegrees));
        VerifyTestResult (Angle (1.1) + Angle (1.1) < Angle (180, Angle::AngleFormat::eDegrees));
        VerifyTestResult (Angle (1.1) + Angle (1.1) > Angle (120, Angle::AngleFormat::eDegrees));
    }
    void    Test4_OddEvenPrime_ ()
    {
        VerifyTestResult (IsPrime (2));
        VerifyTestResult (IsOdd (3));
        VerifyTestResult (IsEven (4));
        VerifyTestResult (IsPrime (5));
        for (int i = 1; i < 1000; ++i) {
            VerifyTestResult (IsOdd (i) != IsEven (i));
            if (IsPrime (i)) {
                VerifyTestResult (i == 2 or IsOdd (i));
            }
            if (IsEven (i)) {
                VerifyTestResult (i == 2 or not IsPrime (i));
            }
        }
    }
}


namespace {
    void    Test5_ReBin_ ()
    {
        using   ReBin::ReBin;
        {
            uint32_t srcBinData[] = { 3, 5, 19, 2 };
            double  resultData[4];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            for (int i = 0; i < NEltsOf(srcBinData); ++i) {
                VerifyTestResult (srcBinData[i] == resultData[i]);
            }
        }
        {
            uint32_t srcBinData[] = { 3, 5, 19, 2 };
            double  resultData[2];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            VerifyTestResult (8 == resultData[0]);
            VerifyTestResult (21 == resultData[1]);
        }
        {
            uint32_t srcBinData[] = { 3, 5, 19, 2, 0, 0, 0 };
            double  resultData[4];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            VerifyTestResult (NearlyEquals ((3 + (5 * ((7.0 / 4.0) - 1))), resultData[0]));
            VerifyTestResult (0 == resultData[3]);
        }
        {
            uint32_t srcBinData[] = { 3, 5, 19, 2 };
            double  resultData[8];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            VerifyTestResult (NearlyEquals (1.5, resultData[0]));
            VerifyTestResult (NearlyEquals (1.5, resultData[1]));
            VerifyTestResult (NearlyEquals (2.5, resultData[2]));
            VerifyTestResult (NearlyEquals (2.5, resultData[3]));
        }
        {
            uint32_t srcBinData[] = { 3, 5, 19, 2 };
            double  resultData[4];
            using   SRC_DATA_DESCRIPTOR     =   ReBin::BasicDataDescriptor<double, uint32_t>;
            using   TRG_DATA_DESCRIPTOR     =   ReBin::UpdatableDataDescriptor<double, double>;
            SRC_DATA_DESCRIPTOR srcData (begin (srcBinData), end (srcBinData), 0, 10);
            TRG_DATA_DESCRIPTOR trgData (begin (resultData), end (resultData), 1, 11);
            ReBin (srcData, &trgData);
            VerifyTestResult (NearlyEquals (3.8, resultData[0]));

        }
    }
}


namespace {
    void    Test6_Statistics_ ()
    {
        VerifyTestResult (Math::Mean (vector<int> ({1, 3, 5})) == 3);
        VerifyTestResult (Math::Mean (vector<int> ({5, 3, 1})) == 3);
        VerifyTestResult (Math::Median (vector<int> ({1, 3, 5})) == 3);
        VerifyTestResult (Math::Median (vector<int> ({5, 3, 1})) == 3);
    }

}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Overlap_ ();
        Test2_Round_ ();
        Test3_Angle_ ();
        Test4_OddEvenPrime_ ();
        Test5_ReBin_ ();
        Test6_Statistics_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



