/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#include    "Stroika/Frameworks/Service/Main.h"


using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::Service;

using   Containers::Sequence;


/*
 *  Almost always would want to use logger with a service. But demarcate so clear what is 'service' demo
 *  and what is logger demo.
 */
#ifndef     qUseLogger
#define     qUseLogger 1
#endif

/*
 *  Most likely would want todo this. It has nothing todo with the service framework, but often desirable
 *  in service apps.
 */
#ifndef     qRegisterFatalErrorHandlers
#define     qRegisterFatalErrorHandlers 1
#endif





#if     qUseLogger
#include    "Stroika/Foundation/Execution/Logger.h"
using   Execution::Logger;
#endif


#if     qRegisterFatalErrorHandlers
#include    "Stroika/Foundation/Debug/Fatal.h"
#if     qPlatform_Windows
#include    "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#include    "Stroika/Foundation/Execution/Platform/Windows/StructuredException.h"
#endif
#endif







#if     qRegisterFatalErrorHandlers
namespace   {
    void    _FatalErorrHandler_ (const Characters::TChar* msg)
    {
        DbgTrace (TSTR ("Fatal Error %s encountered"), msg);
#if     qUseLogger
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Fatal Error: %s; Aborting...", Characters::TString2NarrowSDK (msg).c_str ());
#endif
    }
}
#endif






namespace {
    struct   AppRep_ : Main::IApplicationRep {
        AppRep_ () {
        }
        virtual ~AppRep_ () {
        }

    public:
        virtual void  MainLoop () override {
            Execution::Sleep (1 * 24 * 60 * 60);    // wait 1 day ... simple test....
        }
        virtual Main::ServiceDescription  GetServiceDescription () const override {
            Main::ServiceDescription    t;
            t.fPrettyName = L"Test Service";
            t.fRegistrationName = L"Test-Service";
            return t;
        }
    };
}





namespace {
    void    ShowUsage_ (const Execution::InvalidCommandLineArgument& e = Execution::InvalidCommandLineArgument ())
    {
        if (not e.fMessage.empty ()) {
            cerr << "Error: " << e.fMessage.AsUTF8 () << endl;
            cerr << endl;
        }
        cerr << "Usage: Sample-SimpleService [options] where options can be:\n";
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRunAsService) << "        /* Run this process as a service (doesn't exit until the serivce is done ...) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kStart) << "                 /* Service/Control Function: Start the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kStop) << "                  /* Service/Control Function: Stop the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kForcedStop) << "            /* Service/Control Function: Forced stop the service (after trying to normally stop) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRestart) << "	        /* Service/Control Function: Stop and then re-start the service (ok if already stopped) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kForcedRestart) << "         /* Service/Control Function: Stop (force if needed) and then re-start the service (ok if already stopped) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kReloadConfiguration) << "  /* kReloadConfiguration (@todo  TDB) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kPause) << "                 /* Service/Control Function: Pause the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kContinue) << "              /* Service/Control Function: Continue the paused service */" << endl;
        cerr << "\t--Status                /* Service/Control Function: Print status of running service */ " << endl;
        cerr << "\t--run2Idle              /* run2Idle (@todo  TDB) */ " << endl;
        cerr << "\t--help                  /* Print this help. */ " << endl;
        cerr << endl << "\tExtra unrecognized parameters for start/restart, and forcedrestart operations will be passed along to the actual service process" << endl;
        cerr << endl;
    }
}




int main (int argc, const char* argv[])
{
#if     qRegisterFatalErrorHandlers
#if qPlatform_Windows
    Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
    Execution::Platform::Windows::StructuredException::RegisterHandler ();
#endif
    Debug::RegisterDefaultFatalErrorHandlers (_FatalErorrHandler_);
#endif

#if     qUseLogger
#if     qHas_Syslog
    Logger::Get ().SetAppender (Logger::IAppenderRepPtr (new Logger::SysLogAppender (L"Stroika-Sample-SimpleService")));
#elif   qPlatform_Windows
    //  -- NYI as of 2013-06-08
    //Logger::Get ().SetAppender (Logger::IAppenderRepPtr (new Main::LoggerServiceWrapper (Logger::IAppenderRepPtr (new Logger::WindowsEventLogAppender (L"Stroika-Sample-SimpleService")))));
#endif
#endif

    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    shared_ptr<Main::IServiceIntegrationRep>    serviceIntegrationRep   =   Main::mkDefaultServiceIntegrationRep ();
    if (Execution::MatchesCommandLineArgument (args, L"run2Idle")) {
        cerr << "Warning: RunTilIdleService not really done correctly yet - no notion of idle" << endl;
        serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::RunTilIdleService ());
    }
#if     qUseLogger
    serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::LoggerServiceWrapper (serviceIntegrationRep));
#endif
    Main    m (shared_ptr<AppRep_> (new AppRep_ ()), serviceIntegrationRep);
    if (Execution::MatchesCommandLineArgument (args, L"status")) {
        cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
        return EXIT_SUCCESS;
    }
    else if (Execution::MatchesCommandLineArgument (args, L"help")) {
        ShowUsage_ ();
        return EXIT_SUCCESS;
    }
    try {
        m.Run (args);
    }
    catch (const Execution::InvalidCommandLineArgument& e) {
        ShowUsage_ (e);
    }
    catch (const std::exception& e) {
#if     qUseLogger
        Logger::Get ().Log (Logger::Priority::eError, String::FromNarrowSDKString (e.what ()));
#endif
        cerr << "FAILED: '" << e.what () << "'" << endl;
        return EXIT_FAILURE;
    }
    catch (const Execution::StringException& e) {
#if     qUseLogger
        Logger::Get ().Log (Logger::Priority::eError, e.As<String> ());
#endif
		cerr << "FAILED: '" << e.As<String> ().AsNarrowSDKString () << "'" << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
#if     qUseLogger
        Logger::Get ().Log (Logger::Priority::eError, L"Unknown Exception...");
#endif
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
