/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Execution/ProcessRunner.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"

#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;






namespace   {
    void    RegressionTest1_ ()
    {
        Streams::BasicBinaryOutputStream myStdOut;
#if         qPlatform_Windows
        // quickie about to test..
        ProcessRunner pr (TSTR ("ls"), nullptr, myStdOut);
        pr.Run ();
#endif

        int a = 3;
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        RegressionTest1_ ();
    }

}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

