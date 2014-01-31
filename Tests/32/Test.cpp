/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Execution/ProcessRunner.h"
#include    "Stroika/Foundation/Streams/BasicBinaryInputOutputStream.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"

#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Characters::String;




namespace   {
    void    RegressionTest1_ ()
    {
        Streams::BasicBinaryOutputStream myStdOut;
#if         qPlatform_Windows
        // quickie about to test..
        ProcessRunner pr (SDKSTR ("echo hi mom"), nullptr, myStdOut);
        pr.Run ();
#endif
    }
    void    RegressionTest2_ ()
    {
        Streams::BasicBinaryOutputStream myStdOut;
#if         qPlatform_Windows
        // quickie about to test..
        ProcessRunner pr (SDKSTR ("echo hi mom"));
        String out = pr.Run (L"");
        VerifyTestResult (out.Trim () == L"hi mom");
#endif
    }
    void    RegressionTest3_Pipe_ ()
    {
        Streams::BasicBinaryOutputStream myStdOut;
#if         qPlatform_Windows
        ProcessRunner pr1 (SDKSTR ("echo hi mom"));
        Streams::BasicBinaryInputOutputStream pipe;
        ProcessRunner pr2 (SDKSTR ("cat"));
        pr1.SetStdOut (pipe);
        pr2.SetStdIn (pipe);

        Streams::BasicBinaryOutputStream pr2Out;
        pr2.SetStdOut (pr2Out);

        pr1.Run ();
        pr2.Run ();

        String out = String::FromUTF8 (pr2Out.As<string> ());

        VerifyTestResult (out.Trim () == L"hi mom");
#endif
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        RegressionTest1_ ();
        RegressionTest2_ ();
        RegressionTest3_Pipe_ ();
    }

}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

