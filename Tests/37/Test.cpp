/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Function.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
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



namespace {
    void    Test2_CommandLine_ ()
    {
        {
            String  cmdLine = L"/bin/sh -c \"a b c\"";
            Sequence<String>  l = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 3);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"-c");
            VerifyTestResult (l[2] == L"a b c");
        }
        {
            String  cmdLine = L"";
            Sequence<String>  l = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 0);
        }
        {
            String  cmdLine = L"/bin/sh -c \'a b c\'";
            Sequence<String>  l = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 3);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"-c");
            VerifyTestResult (l[2] == L"a b c");
        }
        {
            String  cmdLine = L"/bin/sh\t b c     -d";
            Sequence<String>  l = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 4);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"b");
            VerifyTestResult (l[2] == L"c");
            VerifyTestResult (l[3] == L"-d");
        }
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Function_ ();
        Test2_CommandLine_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



