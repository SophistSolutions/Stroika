/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Finally.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#include    "Stroika/Foundation/Execution/Thread.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Frameworks/Service/Main.h"


/**
 *  \file
 *
 *  SAMPLE CODE
 *
 *  Sample Simple Service Application
 *
 *  This sample demonstrates a few Stroika features.
 *
 *      o   Creating a service application (one that can be automatically started/stopped by
 *          the OS, and one where you can query the status, check process ID, etc)
 *
 *      o   Simple example of command line processing
 *
 *      o   Simple example of Logging (to syslog or windows log or other)
 */




using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::Service;


using   Containers::Sequence;
using   Execution::Thread;


/*
 *  Almost always would want to use logger with a service. But demarcate so clear what is 'service' demo
 *  and what is logger demo.
 *
 *  To test, on many systems, you can do
 *      tail -f /var/log/syslog (in one window - note - depending on your system, the logfile could be elsewhere)
 *  and
 *      Samples_SimpleService -start
 *      Samples_SimpleService -status
 *      Samples_SimpleService -stop
 *  and see the log messages appear in the logfile.
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
    void    _FatalErorrHandler_ (const Characters::SDKChar* msg)
    {
        DbgTrace (SDKSTR ("Fatal Error %s encountered"), msg);
#if     qUseLogger
        Logger::Log (Logger::Priority::eCriticalError, L"Fatal Error: %s; Aborting...", Characters::SDKString2NarrowSDK (msg).c_str ());
#endif
        _exit (1);
    }
}
#endif






namespace {
    struct   AppRep_ : Main::IApplicationRep {
        AppRep_ ()
        {
        }
        virtual ~AppRep_ ()
        {
        }

    public:
        virtual void  MainLoop (std::function<void()> startedCB) override
        {

            Execution::Finally cleanup ([this] () {
                /*
                 *  Now - we can shutdown any subsidiary threads, and exit
                 */
                fSomeOtherTaskDoingRealWork.AbortAndWaitForDone ();
#if     qUseLogger
                Logger::Log (Logger::Priority::eInfo, L"User-service code is shut down");
#endif
            });

            /*
             *  In your main loop, first run any setup.
             */
            // INITIALIZE_SOMETHING();
            fSomeOtherTaskDoingRealWork = Thread ([] () {
                Execution::Sleep (1 * 24 * 60 * 60);    // wait 1 day ... simple test....
            });
            fSomeOtherTaskDoingRealWork.Start ();

            startedCB ();       // Notify service control mgr that the service has started

#if     qUseLogger
            Logger::Log (Logger::Priority::eInfo, L"User-service code is started");
#endif
            if (false) {
                Execution::WaitableEvent forever (Execution::WaitableEvent::eAutoReset);
                forever.Wait ();            // until told to stop by abort exception
            }
            else {
                while (true) {
                    // Or you could use a waitable event and wait forever, or do some period bookkeeping.
                    Execution::Sleep (1 * 24 * 60 * 60);    // wait 1 day ... simple test....
                }
            }

            /*
             *  Cleanups - on exit handled in finally/Cleanup above.
             */
        }

    public:
        virtual Main::ServiceDescription  GetServiceDescription () const override
        {
            Main::ServiceDescription    t;
            t.fPrettyName = L"Test Service";
            t.fRegistrationName = L"Test-Service";
            return t;
        }

    public:
        // No NEED to use this technique, but its an easy way to have as many paralell process tasks
        // ask you want running while your main task does essentially nothing but cleanup when the
        // service shuts down
        Thread  fSomeOtherTaskDoingRealWork;
    };
}





namespace {
    void    ShowUsage_ (const Main& m, const Execution::InvalidCommandLineArgument& e = Execution::InvalidCommandLineArgument ())
    {
        if (not e.fMessage.empty ()) {
            cerr << "Error: " << e.fMessage.AsUTF8 () << endl;
            cerr << endl;
        }
        cerr << "Usage: Sample-SimpleService [options] where options can be:\n";
        if (m.GetServiceIntegrationFeatures ().Contains (Main::ServiceIntegrationFeatures::eInstall)) {
            cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kInstall) << "               /* Install service (only when debugging - should use real installer like WIX) */" << endl;
            cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kUnInstall) << "             /* UnInstall service (only when debugging - should use real installer like WIX) */" << endl;
        }
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRunAsService) << "        /* Run this process as a service (doesn't exit until the serivce is done ...) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRunDirectly) << "           /* Run this process as a directly (doesn't exit until the serivce is done ...) but not using service infrastructure */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kStart) << "                 /* Service/Control Function: Start the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kStop) << "                  /* Service/Control Function: Stop the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kForcedStop) << "            /* Service/Control Function: Forced stop the service (after trying to normally stop) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRestart) << "	        /* Service/Control Function: Stop and then re-start the service (ok if already stopped) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kForcedRestart) << "         /* Service/Control Function: Stop (force if needed) and then re-start the service (ok if already stopped) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kReloadConfiguration) << "  /* Reload service configuration */" << endl;
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
    Execution::SignalHandlerRegistry::SafeSignalsManager    safeSignals;
    /*
     *  Setup basic (optional) error handling.
     */
#if     qRegisterFatalErrorHandlers
#if qPlatform_Windows
    Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
    Execution::Platform::Windows::StructuredException::RegisterHandler ();
#endif
    Debug::RegisterDefaultFatalErrorHandlers (_FatalErorrHandler_);
#endif

    /*
     *  Setup Logging to the OS logging facility.
     */
#if     qUseLogger
#if     qHas_Syslog
    Logger::Get ().SetAppender (Logger::IAppenderRepPtr (new Logger::SysLogAppender (L"Stroika-Sample-SimpleService")));
#elif   qPlatform_Windows
    Logger::Get ().SetAppender (Logger::IAppenderRepPtr (new Logger::WindowsEventLogAppender ()));
#endif
#endif

    /*
     *  Parse command line arguments, and start looking at options.
     */
    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    shared_ptr<Main::IServiceIntegrationRep>    serviceIntegrationRep   =   Main::mkDefaultServiceIntegrationRep ();
    if (Execution::MatchesCommandLineArgument (args, L"run2Idle")) {
        cerr << "Warning: RunTilIdleService not really done correctly yet - no notion of idle" << endl;
        serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::RunTilIdleService ());
    }
#if     qUseLogger
    serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::LoggerServiceWrapper (serviceIntegrationRep));
#endif

    /*
     *  Create service handler instance.
     */
    Main    m (shared_ptr<AppRep_> (new AppRep_ ()), serviceIntegrationRep);

    /*
     *  Run request.
     */
    try {
        if (Execution::MatchesCommandLineArgument (args, L"status")) {
            cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
            return EXIT_SUCCESS;
        }
        else if (Execution::MatchesCommandLineArgument (args, L"help")) {
            ShowUsage_ (m);
            return EXIT_SUCCESS;
        }
        else {
            /*
             *  Run the commands, and capture/display exceptions
             */
            m.Run (args);
        }
    }
    catch (const Execution::InvalidCommandLineArgument& e) {
        ShowUsage_ (m, e);
    }
    catch (const Execution::StringException& e) {
#if     qUseLogger
        Logger::Log (Logger::Priority::eError, L"%s", e.As<String> ().c_str ());
#endif
        cerr << "FAILED: " << e.As<String> ().AsNarrowSDKString () << endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
#if     qUseLogger
        Logger::Log (Logger::Priority::eError, L"%s", String::FromNarrowSDKString (e.what ()).c_str ());
#endif
        cerr << "FAILED: " << e.what () << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
#if     qUseLogger
        Logger::Log (Logger::Priority::eError, L"Unknown Exception...");
#endif
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
