/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
// TEST: Foundation::Execution::Signals
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#include    "Stroika/Foundation/Execution/Sleep.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Containers::Set;


namespace   {
    void    Test1_Basic_ ()
    {
        Set<SignalHandlerType> saved    =   SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        {
            bool    called  =   false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandlerType ([&called] (SignalIDType signal) -> void {called = true;}));
            ::raise (SIGINT);
            VerifyTestResult (called);
        }
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

