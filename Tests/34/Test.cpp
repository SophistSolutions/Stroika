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
        Set<SignalHandler> saved    =   SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        {
            bool    called  =   false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called] (SignalID signal) -> void {called = true;}));
            ::raise (SIGINT);
            Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
            VerifyTestResult (called);
        }
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
    }
}


namespace   {
    void    Test2_Direct_ ()
    {
        Set<SignalHandler> saved    =   SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        {
            bool    called  =   false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called] (SignalID signal) -> void {called = true;}, SignalHandler::Type::eDirect));
            ::raise (SIGINT);
            VerifyTestResult (called);
        }
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
    }
}



namespace   {
    void    Test3_Safe_ ()
    {
        Set<SignalHandler> saved    =   SignalHandlerRegistry::Get ().GetSignalHandlers (SIGINT);
        {
            bool    called  =   false;
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler ([&called] (SignalID signal) -> void {called = true;}));
            ::raise (SIGINT);
            Execution::Sleep (0.5); // delivery could be delayed because signal is pushed to another thread
            VerifyTestResult (called);
        }
        SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, saved);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_Basic_ ();
        Test2_Direct_ ();
        Test3_Safe_ ();
    }

}



int     main (int argc, const char* argv[])
{
    SignalHandlerRegistry::SafeSignalsManager   safeSignals;
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

