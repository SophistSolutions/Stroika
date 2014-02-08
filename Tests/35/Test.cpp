/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/Function.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;




namespace   {
    void    Test1_Function_()
    {
        // Make sure Function<> works as well as std::function
        {
            Function<int(bool)> f = [] (bool b) -> int { return 3;};
            VerifyTestResult (f(true) == 3);
            function<int(bool)> ff = f;
            VerifyTestResult (ff(true) == 3);
        }
        // Make sure Function<> serves its one purpose - being comparable
        {
            Function<int(bool)> f1 = [] (bool b) -> int { return 3;};
            Function<int(bool)> f2 = [] (bool b) -> int { return 3;};

            VerifyTestResult (f1 != f2);
            VerifyTestResult (f1 < f2 or f2 < f1);
            Function<int(bool)> f3 = f1;
            VerifyTestResult (f3 == f1);
            VerifyTestResult (f3 != f2);
        }

    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Function_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



