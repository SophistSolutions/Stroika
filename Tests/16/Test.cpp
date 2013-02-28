/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//	TEST	Foundation::Math
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Math/Overlap.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Math;




namespace   {
	void	Test1_Overlap_ ()
	{
		VerifyTestResult (OverlapOrStrictlyContains<int> (1, 3, 2, 2));
		VerifyTestResult (not OverlapOrStrictlyContains<int> (1, 3, 3, 4));
		VerifyTestResult (not OverlapOrStrictlyContains<int> (1, 10, 3, 4));
		VerifyTestResult (not OverlapOrStrictlyContains<int> (1, 10, 3, 3));
	}
}


namespace   {

    void    DoRegressionTests_ ()
    {
		Test1_Overlap_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



