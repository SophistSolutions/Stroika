/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Execution/ProcessRunner.h"
#if     qPlatform_POSIX
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
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
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom", nullptr, myStdOut);
        pr.Run ();
    }
    void    RegressionTest2_ ()
    {
        Streams::BasicBinaryOutputStream myStdOut;
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom");
        String out = pr.Run (L"");
        VerifyTestResult (out.Trim () == L"hi mom");
    }
    void    RegressionTest3_Pipe_ ()
    {
        Streams::BasicBinaryOutputStream myStdOut;
        ProcessRunner pr1 (L"echo hi mom");
        Streams::BasicBinaryInputOutputStream pipe;
        ProcessRunner pr2 (L"cat");
        pr1.SetStdOut (pipe);
        pr2.SetStdIn (pipe);

        Streams::BasicBinaryOutputStream pr2Out;
        pr2.SetStdOut (pr2Out);

        pr1.Run ();
        pr2.Run ();

        String out = String::FromUTF8 (pr2Out.As<string> ());

        VerifyTestResult (out.Trim () == L"hi mom");
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
#if     qPlatform_POSIX
        // Many performance instruments use pipes
        // @todo - REVIEW IF REALLY NEEDED AND WHY? SO LONG AS NO FAIL SHOULDNT BE?
        //  --LGP 2014-02-05
        Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
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

