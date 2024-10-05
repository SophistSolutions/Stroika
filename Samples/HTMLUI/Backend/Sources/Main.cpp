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
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
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
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks::Service;

using namespace Stroika::Samples::HTMLUI;

namespace {
    /**
     * Simple shell application logic - setting up appropriate modules/components as needed, and handling command line processing.
     */
    struct MyApp_ {
        /*
         * Setup of modules which always happens (not just when running service) - can be done here, if it requires
         * no command line arguments. Then neatly shut down in DTOR automatically.
         */

        SignalHandlerRegistry::SafeSignalsManager fSafeSignals;

        Logger::Activator fLoggerActivation{Logger::Options{
            .fLogBufferingEnabled         = true,
            .fSuppressDuplicatesThreshold = 30s,
        }};

        static inline const CommandLine::Option           kPortO_{.fLongName = "port"sv, .fSupportsArgument = true};
        static inline const Sequence<CommandLine::Option> kAllOptions_{
            Sequence<CommandLine::Option>{Main::CommandOptions::kAll} +
            Sequence<CommandLine::Option>{StandardCommandLineOptions::kHelp, kPortO_, StandardCommandLineOptions::kVersion}};

        MyApp_ ()
        {
            // Default - in case any logging writes happen before we setup the configured logging appenders
            Logger::sThe.SetAppenders (make_shared<Logger::StreamAppender> (
                IO::FileSystem::FileOutputStream::New (STDOUT_FILENO, IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction)));

            /**
             * Setup various error/assertion error handlers/checkers
             */
#if qPlatform_Windows
            Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
            Execution::Platform::Windows::RegisterDefaultHandler_StructuredException ();
#endif
            Debug::RegisterDefaultFatalErrorHandlers (DefaultLoggingFatalErrorHandler);
            SignalHandlerRegistry::Get ().SetStandardCrashHandlerSignals (SignalHandler{DefaultLoggingCrashSignalHandler, SignalHandler::Type::eDirect});
#if qPlatform_POSIX
            SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif
        }

        int Run (const CommandLine& cmdLine)
        {
            /**
             *  Validate command-line arguments, handle some simple command line tasks, and parse the rest of the options
             *  into Options_ which will be used later.
             */
            try {
                cmdLine.Validate (kAllOptions_);
            }
            catch (const InvalidCommandLineArgument&) {
                cerr << Characters::ToString (current_exception ()).AsNarrowSDKString () << endl;
                cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
                return EXIT_FAILURE;
            }

            if (cmdLine.Has (StandardCommandLineOptions::kHelp)) {
                cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
                return EXIT_SUCCESS;
            }

            Options_ options{cmdLine};

            // replace preliminary logging appenders, after we've read the configuration (gAppConfiguration)
            Logger::sThe.SetAppenders ([] () {
                static const String kAppName_                            = "Stroika-Sample-HTMLUI"sv;
                using Logging                                            = AppConfigurationType::Logging;
                Logging                                    loggingConfig = gAppConfiguration->fLogging.value_or (Logging{});
                Sequence<shared_ptr<Logger::IAppenderRep>> appenders;
                if (loggingConfig.ToStdOut.value_or (Logging::kToStdOut_Default)) {
                    appenders += make_shared<Logger::StreamAppender> (IO::FileSystem::FileOutputStream::New (
                        STDOUT_FILENO, IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction));
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

            /*
             *  Create the service manager objects
             */
            shared_ptr<Main::IServiceIntegrationRep> serviceIntegrationRep =
                make_shared<Main::LoggerServiceWrapper> (Main::mkDefaultServiceIntegrationRep ());
            Main m{make_shared<Stroika::Samples::HTMLUI ::Service::SampleAppServiceRep> (options.fPortNumberOverride), serviceIntegrationRep};

            if (cmdLine.Has (Main::CommandOptions::kStatus)) {
                cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
                return EXIT_SUCCESS;
            }
            else if (cmdLine.Has (StandardCommandLineOptions::kVersion)) {
                cout << m.GetServiceDescription ().fPrettyName.AsNarrowSDKString () << ": "sv
                     << Characters::ToString (AppVersion::kVersion).AsNarrowSDKString () << endl;
                return EXIT_SUCCESS;
            }

            /*
             * Several components use interval timers, and this allows those modules to run (but have timer service started/shutdown in a controlled
             * fashion).
             */
            IntervalTimer::Manager::Activator intervalTimerMgrActivator;

            /*
             *  This handles the typical case, where the service is just run, or some service command is being issued to the service manager.
             */
            m.Run (options.fServiceArgs);
            return EXIT_SUCCESS;
        }

    public:
        struct Options_ {
            optional<uint16_t> fPortNumberOverride;
            Main::CommandArgs  fServiceArgs;

            Options_ (const CommandLine& cmdLine)
                : fServiceArgs{cmdLine}
            {
                if (auto o = cmdLine.GetArgument (kPortO_)) {
                    fPortNumberOverride = Characters::String2Int<uint16_t> (*o);
                }
            }
        };
    };

}

int main (int argc, char* argv[])
{
    try {
        MyApp_ myApp;
        return myApp.Run (CommandLine{argc, argv});
    }
    catch (...) {
        cerr << endl << "EXCEPTION: {}"_f(current_exception ()).AsNarrowSDKString () << endl;
    }
    return EXIT_SUCCESS;
}
