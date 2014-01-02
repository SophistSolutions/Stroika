/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
// TEST: Foundation::Execution::Signals
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/Signals.h"
#include    "Stroika/Foundation/Execution/Sleep.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



namespace   {
    static  bool    Test1_Basic_called_ =   false;
    static  void    Test1_Basic_DoIt_ (SignalIDType signal)
    {
        Test1_Basic_called_ = true;
    }
    void    Test1_Basic_ ()
    {
        set<SignalHandlerType> saved    =   SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
#if 1
        Test1_Basic_called_ =   false;
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, Test1_Basic_DoIt_);
        ::raise (SIGINT);
        VerifyTestResult (Test1_Basic_called_);
#else
        // VS (and maybe gcc) don't support the converion of lambda to plain function pointers yet
        bool    called  =   false;
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, [&called] (SignalIDType signal) -> void {called = true;});
        ::raise (SIGINT);
        VerifyTestResult (called);
#endif
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_Basic_ ();
    }

}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

