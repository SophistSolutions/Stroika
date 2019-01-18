/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Fatal.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Thread.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#include "Stroika/Foundation/Execution/Platform/Windows/StructuredException.h"
#endif
#include "Stroika/Frameworks/Service/Main.h"

#include "AppVersion.h"

#include "Service.h"

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

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Service;

using Characters::String_Constant;
using Containers::Sequence;
using Execution::SignalHandler;
using Execution::SignalHandlerRegistry;
using Execution::Thread;

#if qUseLogger
#include "Stroika/Foundation/Debug/BackTrace.h"
#include "Stroika/Foundation/Execution/Logger.h"
using Execution::Logger;
#endif

namespace {
    void FatalErorrHandler_ (const Characters::SDKChar* msg) noexcept
    {
        Thread::SuppressInterruptionInContext suppressCtx;
        DbgTrace (SDKSTR ("Fatal Error %s encountered"), msg);
#if qUseLogger
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Fatal Error: %s; Aborting...", String::FromSDKString (msg).c_str ());
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Backtrace: %s", Debug::BackTrace ().c_str ());
        if (std::exception_ptr exc = std::current_exception ()) {
            Logger::Get ().Log (Logger::Priority::eCriticalError, L"Uncaught exception: %s", Characters::ToString (exc).c_str ());
        }
        Logger::Get ().Flush ();
#endif
        std::_Exit (EXIT_FAILURE); // skip
    }
    void FatalSignalHandler_ (Execution::SignalID signal) noexcept
    {
        Thread::SuppressInterruptionInContext suppressCtx;
        DbgTrace (L"Fatal Signal encountered: %s", Execution::SignalToName (signal).c_str ());
#if qUseLogger
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Fatal Signal: %s; Aborting...", Execution::SignalToName (signal).c_str ());
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Backtrace: %s", Debug::BackTrace ().c_str ());
        Logger::Get ().Flush ();
#endif
        std::_Exit (EXIT_FAILURE); // skip
    }
}

namespace {
    void ShowUsage_ (const Main& m, const Execution::InvalidCommandLineArgument& e = Execution::InvalidCommandLineArgument ())
    {
        if (not e.fMessage.empty ()) {
            cerr << "Error: " << e.fMessage.AsUTF8 () << endl;
            cerr << endl;
        }
        cerr << "Usage: " << m.GetServiceDescription ().fRegistrationName.AsNarrowSDKString () << " [options] where options can be :\n ";
        if (m.GetServiceIntegrationFeatures ().Contains (Main::ServiceIntegrationFeatures::eInstall)) {
            cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kInstall) << "               /* Install service (only when debugging - should use real installer like WIX) */" << endl;
            cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kUnInstall) << "             /* UnInstall service (only when debugging - should use real installer like WIX) */" << endl;
        }
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRunAsService) << "        /* Run this process as a service (doesn't exit until the serivce is done ...) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRunDirectly) << "          /* Run this process as a directly (doesn't exit until the serivce is done ...) but not using service infrastructure */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kStart) << "                 /* Service/Control Function: Start the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kStop) << "                  /* Service/Control Function: Stop the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kForcedStop) << "            /* Service/Control Function: Forced stop the service (after trying to normally stop) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kRestart) << "               /* Service/Control Function: Stop and then re-start the service (ok if already stopped) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kForcedRestart) << "         /* Service/Control Function: Stop (force if needed) and then re-start the service (ok if already stopped) */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kReloadConfiguration) << "  /* Reload service configuration */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kPause) << "                 /* Service/Control Function: Pause the service */" << endl;
        cerr << "\t--" << Characters::WideStringToNarrowSDKString (Main::CommandNames::kContinue) << "              /* Service/Control Function: Continue the paused service */" << endl;
        cerr << "\t--Status                /* Service/Control Function: Print status of running service */ " << endl;
        cerr << "\t--Version               /* print this application version */ " << endl;
        cerr << "\t--run2Idle              /* run2Idle (@todo  TDB) */ " << endl;
        cerr << "\t--help                  /* Print this help. */ " << endl;
        cerr << endl
             << "\tExtra unrecognized parameters for start/restart, and forcedrestart operations will be passed along to the actual service process" << endl;
        cerr << endl;
    }
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};

#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
    [[maybe_unused]] auto&& cleanupReport = Execution::Finally ([]() {
        DbgTrace (L"Exiting main with thread %s running", Characters::ToString (Execution::Thread::GetStatistics ().fRunningThreads).c_str ());
    });
#endif

    /*
     *  This allows for safe signals to be managed in a threadsafe way
     */
    SignalHandlerRegistry::SafeSignalsManager safeSignals;

    /*
     *  Setup basic (optional) error handling.
     */
#if qPlatform_Windows
    Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
    Execution::Platform::Windows::StructuredException::RegisterHandler ();
#endif
    Debug::RegisterDefaultFatalErrorHandlers (FatalErorrHandler_); // override the default handler to emit message via Logger

    /*
     *  SetStandardCrashHandlerSignals not really needed, but helpful for many applications so you get a decent log message/debugging on crash.
     */
    SignalHandlerRegistry::Get ().SetStandardCrashHandlerSignals (SignalHandler{FatalSignalHandler_, SignalHandler::Type::eDirect});

    /*
     *  Ignore SIGPIPE is common practice/helpful in POSIX, but not required by the service manager.
     */
#if qPlatform_POSIX
    SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif

    /*
     *  Setup Logging to the OS logging facility.
     */
#if qUseLogger
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([]() {
        Logger::ShutdownSingleton (); // make sure Logger threads shutdown before the end of main (), and flush buffered messages
    });
#if qHas_Syslog
    Logger::Get ().SetAppender (make_shared<Logger::SysLogAppender> (L"Stroika-Sample-Service"));
#elif qPlatform_Windows
    Logger::Get ().SetAppender (make_shared<Logger::WindowsEventLogAppender> (L"Stroika-Sample-Service"));
#endif
    /*
     *  Optional - use buffering feature
     *  Optional - use suppress duplicates in a 15 second window
     */
    Logger::Get ().SetBufferingEnabled (true);
    Logger::Get ().SetSuppressDuplicates (15);
#endif

    /*
     *  Parse command line arguments, and start looking at options.
     */
    Sequence<String>                         args                  = Execution::ParseCommandLine (argc, argv);
    shared_ptr<Main::IServiceIntegrationRep> serviceIntegrationRep = Main::mkDefaultServiceIntegrationRep ();
    if (Execution::MatchesCommandLineArgument (args, L"run2Idle")) {
        cerr << "Warning: RunTilIdleService not really done correctly yet - no notion of idle" << endl;
        serviceIntegrationRep = make_shared<Main::RunTilIdleService> ();
    }
#if qUseLogger
    serviceIntegrationRep = make_shared<Main::LoggerServiceWrapper> (serviceIntegrationRep);
#endif

    /*
     *  Create service handler instance.
     */
    Main m (make_shared<Samples::Service::SampleAppServiceRep> (), serviceIntegrationRep);

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
        else if (Execution::MatchesCommandLineArgument (args, L"version")) {
            cout << m.GetServiceDescription ().fPrettyName.AsNarrowSDKString () << ": " << Characters::ToString (AppVersion::kVersion).AsNarrowSDKString () << endl;
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
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
#if qUseLogger
        Logger::Get ().Log (Logger::Priority::eError, L"%s", exceptMsg.c_str ());
#endif
        cerr << "FAILED: " << exceptMsg.AsNarrowSDKString () << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
