/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Fatal.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#include "Stroika/Foundation/Execution/Platform/Windows/StructuredException.h"
#endif

#include "AppConfiguration.h"
#include "AppVersion.h"
#include "Service.h"
#include "WSImpl.h"
#include "WebServer.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Characters::Literals;

using namespace Stroika::Frameworks::Service;

using Characters::String;
using Containers::Sequence;

using namespace Stroika::Samples::HTMLUI;

namespace {
    using StandardCommandLineOptions::kHelp;
    struct MyApp_ {
        SignalHandlerRegistry::SafeSignalsManager fSafeSignals;

        Logger::Activator fLoggerActivation{Logger::Options{
            .fLogBufferingEnabled         = true,
            .fSuppressDuplicatesThreshold = 30s,
        }};

        static inline const Execution::CommandLine::Option kPortO_{.fLongName = "port"sv, .fSupportsArgument = true};
        static inline const Execution::CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true};
        static inline const Sequence<Execution::CommandLine::Option> kAllOptions_{kHelp, kPortO_, kQuitAfterO_}; //wrong add opts from service

        MyApp_ ()
        {
            // Default - in case any logging writes happen before we setup the configured one
            Logger::sThe.SetAppenders (make_shared<Logger::StreamAppender> (
                IO::FileSystem::FileOutputStream::New (1, IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction)));

#if qPlatform_Windows
            Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
            Execution::Platform::Windows::RegisterDefaultHandler_StructuredException ();
#endif
            Debug::RegisterDefaultFatalErrorHandlers (DefaultLoggingFatalErrorHandler);
            SignalHandlerRegistry::Get ().SetStandardCrashHandlerSignals (SignalHandler{DefaultLoggingCrashSignalHandler, SignalHandler::Type::eDirect});
#if qPlatform_POSIX
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif

            // replace preliminary appenders
            Logger::sThe.SetAppenders ([] () {
                static const String kAppName_                            = "Stroika-Sample-HTMLUI"sv;
                using Logging                                            = AppConfigurationType::Logging;
                Logging                                    loggingConfig = gAppConfiguration->fLogging.value_or (Logging{});
                Sequence<shared_ptr<Logger::IAppenderRep>> appenders;
                if (loggingConfig.ToStdOut.value_or (Logging::kToStdOut_Default)) {
                    appenders += make_shared<Logger::StreamAppender> (
                        IO::FileSystem::FileOutputStream::New (1, IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction));
                }
#if qHas_Syslog
                if (loggingConfig.ToSysLog.value_or (Logging::kToSysLog_Default)) {
                    appenders += make_shared<Logger::SysLogAppender> (kAppName_);
                }
#elif qPlatform_Windows
                if (loggingConfig.ToWindowsEventLog.value_or (Logging::kToWindowsEventLog_Default)) {
                    appenders += make_shared<Logger::WindowsEventLogAppender> (kAppName_);
                }
#endif
                return appenders;
            }());
        }

        int Run (const Execution::CommandLine& cmdLine)
        {
            Options_ options{cmdLine};

            /*
             * Several components use interval timers, and this allows those modules to run (but have timer service started/shutdown in a controlled
             * fashion).
             */
            Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator;

            shared_ptr<Main::IServiceIntegrationRep> serviceIntegrationRep =
                make_shared<Main::LoggerServiceWrapper> (Main::mkDefaultServiceIntegrationRep ());

            Main m{make_shared<Stroika::Samples::HTMLUI ::Service::SampleAppServiceRep> (options.fPortNumberOverride), serviceIntegrationRep};

            if (cmdLine.Has (StandardCommandLineOptions::kHelp)) {
                ShowUsage_ (m);
                return EXIT_SUCCESS;
            }
            try {
                cmdLine.Validate (kAllOptions_);
            }
            catch (const Execution::InvalidCommandLineArgument&) {
                cerr << Characters::ToString (current_exception ()).AsNarrowSDKString () << endl;
                cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
                return EXIT_FAILURE;
            }

            try {
                const CommandLine::Option kStatusOpt_ = CommandLine::Option{.fLongName = "status"sv};
                if (cmdLine.Has (kStatusOpt_)) {
                    cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
                    return EXIT_SUCCESS;
                }
                else if (cmdLine.Has (StandardCommandLineOptions::kVersion)) {
                    cout << m.GetServiceDescription ().fPrettyName.AsNarrowSDKString () << ": "sv
                         << Characters::ToString (AppVersion::kVersion).AsNarrowSDKString () << endl;
                    return EXIT_SUCCESS;
                }
                else {
                    m.Run (cmdLine);
                }
            }
            catch (const Execution::InvalidCommandLineArgument& e) {
                ShowUsage_ (m, e);
            }
            return EXIT_SUCCESS;
        }

        static void ShowUsage_ (const Main& m, const Execution::InvalidCommandLineArgument& e = {})
        {
            if (not e.fMessage.empty ()) {
                cerr << "Error: " << e.fMessage.AsUTF8<string> () << endl;
                cerr << endl;
            }
            cerr << "Usage: " << m.GetServiceDescription ().fRegistrationName.AsNarrowSDKString () << " [options] where options can be :\n ";
            if (m.GetServiceIntegrationFeatures ().Contains (Main::ServiceIntegrationFeatures::eInstall)) {
                cerr << "\t--" << String{Main::CommandNames::kInstall}.AsNarrowSDKString ()
                     << "               /* Install service (only when debugging - should use real installer like WIX) */" << endl;
                cerr << "\t--" << String{Main::CommandNames::kUnInstall}.AsNarrowSDKString ()
                     << "             /* UnInstall service (only when debugging - should use real installer like WIX) */" << endl;
            }
            cerr << "\t--" << String{Main::CommandNames::kRunAsService}.AsNarrowSDKString ()
                 << "        /* Run this process as a service (doesn't exit until the serivce is done ...) */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kRunDirectly}.AsNarrowSDKString () << "          /* Run this process as a directly (doesn't exit until the serivce is done or ARGUMENT TIMEOUT seconds elapsed ...) but not using service infrastructure */"
                 << endl;
            cerr << "\t--" << String{Main::CommandNames::kStart}.AsNarrowSDKString ()
                 << "                 /* Service/Control Function: Start the service */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kStop}.AsNarrowSDKString ()
                 << "                  /* Service/Control Function: Stop the service */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kForcedStop}.AsNarrowSDKString ()
                 << "            /* Service/Control Function: Forced stop the service (after trying to normally stop) */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kRestart}.AsNarrowSDKString ()
                 << "               /* Service/Control Function: Stop and then re-start the service (ok if already stopped) */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kForcedRestart}.AsNarrowSDKString ()
                 << "         /* Service/Control Function: Stop (force if needed) and then re-start the service (ok if already stopped) */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kReloadConfiguration}.AsNarrowSDKString () << "  /* Reload service configuration */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kPause}.AsNarrowSDKString ()
                 << "                 /* Service/Control Function: Pause the service */" << endl;
            cerr << "\t--" << String{Main::CommandNames::kContinue}.AsNarrowSDKString ()
                 << "              /* Service/Control Function: Continue the paused service */" << endl;
            cerr << "\t--Status                /* Service/Control Function: Print status of running service */ " << endl;
            cerr << "\t--Version               /* print this application version */ " << endl;
            cerr << "\t--help                  /* Print this help. */ " << endl;
            cerr << endl
                 << "\tExtra unrecognized parameters for start/restart, and forcedrestart operations will be passed along to the actual "
                    "service process"
                 << endl;
            cerr << endl;
        }

    public:
        struct Options_ {
            // static inline const initializer_list<CommandLine::Option> kAllOptions_{
            //     StandardCommandLineOptions::kHelp,
            // };
            optional<uint16_t>    fPortNumberOverride;
            Time::DurationSeconds fQuitAfter{Time::kInfinity};

            Options_ (const Execution::CommandLine& cmdLine)
            {
                if (auto o = cmdLine.GetArgument (kPortO_)) {
                    fPortNumberOverride = Characters::String2Int<uint16_t> (*o);
                }
                if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
                    fQuitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
                }
            }
        };
    };

}

int main (int argc, char* argv[])
{
    try {
        MyApp_ myApp;
        return myApp.Run (Execution::CommandLine{argc, argv});
    }
    catch (...) {
        cerr << endl << "EXCEPTION: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << endl;
    }
    return EXIT_SUCCESS;
#if 0
    Execution::CommandLine                               cmdLine{argc, argv};
    Debug::TraceContextBumper                            ctx{"main", "argv={}"_f, cmdLine};
    Execution::SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    optional<uint16_t>    portNumber;
    Time::DurationSeconds quitAfter = Time::kInfinity;

    //tmphack will go into service module
    using Execution::Logger;
    Logger::Activator loggerActivation{Logger::Options{
        .fLogBufferingEnabled         = true,
        .fSuppressDuplicatesThreshold = 15s,
    }};
    using namespace Stroika::Foundation::IO::FileSystem::FileStream; //tmphack
    //  Logger::sThe.AddAppender (make_shared<Logger::StreamAppender> (FileOutputStream::New (STDOUT_FILENO, AdoptFDPolicy::eDisconnectOnDestruction)));
    Logger::sThe.AddAppender (make_shared<Logger::StreamAppender> (FileOutputStream::New (1, AdoptFDPolicy::eDisconnectOnDestruction)));

    using Execution::StandardCommandLineOptions::kHelp;
    const Execution::CommandLine::Option                   kPortO_{.fLongName = "port"sv, .fSupportsArgument = true};
    const Execution::CommandLine::Option                   kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true};
    const initializer_list<Execution::CommandLine::Option> kAllOptions_{kHelp, kPortO_, kQuitAfterO_};
    try {
        cmdLine.Validate (kAllOptions_);

        if (auto o = cmdLine.GetArgument (kPortO_)) {
            portNumber = Characters::String2Int<uint16_t> (*o);
        }
        if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
            quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
        }
        if (cmdLine.Has (kHelp)) {
            cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
            return EXIT_SUCCESS;
        }

        WebServer myWebServer{portNumber};           // listen and dispatch while this object exists
        Execution::WaitableEvent{}.Wait (quitAfter); // wait quitAfter seconds, or til user hits ctrl-c
    }
    catch (const Execution::TimeOutException&) {
        cerr << "Timed out - so - exiting..." << endl;
        return EXIT_SUCCESS;
    }
    catch (const Execution::InvalidCommandLineArgument&) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << endl;
        cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
#endif
}
