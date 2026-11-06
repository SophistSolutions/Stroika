/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Fatal.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#if qStroika_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#include "Stroika/Foundation/Execution/Platform/Windows/StructuredException.h"
#endif
#include "Stroika/Foundation/Memory/BlockAllocated.h"

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

using Memory::MakeSharedPtr;

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
            /**
             * Default - in case any logging writes happen before we setup the configured logging appenders (e.g. OptionsFile)
             */
            Logger::sThe.SetAppenders (MakeSharedPtr<Logger::StreamAppender> (
                IO::FileSystem::FileOutputStream::New (STDOUT_FILENO, IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction)));

            /**
             * Setup various error/assertion error handlers/checkers
             */
#if qStroika_Platform_Windows
            Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
            Execution::Platform::Windows::RegisterDefaultHandler_StructuredException ();
#endif
            Debug::RegisterDefaultFatalErrorHandlers (DefaultLoggingFatalErrorHandler);
            SignalHandlerRegistry::sThe.SetStandardCrashHandlerSignals (SignalHandler{DefaultLoggingCrashSignalHandler, SignalHandler::Type::eDirect});
#if qStroika_Platform_POSIX
            SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif
        }

        int Run (const CommandLine& cmdLine)
        {
            try {
                /**
                 *  Validate command-line arguments, handle some simple command line tasks, and parse the rest of the options
                 *  into Options_ which will be used later.
                 */
                CommandLine::OptionValues args = cmdLine.ApplyOptions (kAllOptions_);
                if (args.Has (StandardCommandLineOptions::kHelp)) {
                    cerr << cmdLine.GenerateUsage (kAllOptions_);
                    return EXIT_SUCCESS;
                }
                else if (args.Has (Main::CommandOptions::kStatus)) {
                    // no need to wrap in logger, and load options for this
                    Main m{MakeSharedPtr<Service::SampleAppServiceRep> (nullopt), Main::mkDefaultServiceIntegrationRep ()};
                    cout << m.GetServiceStatusMessage ();
                    return EXIT_SUCCESS;
                }
                else if (args.Has (StandardCommandLineOptions::kVersion)) {
                    // no need to wrap in logger, and load options for this
                    Main m{MakeSharedPtr<Service::SampleAppServiceRep> (nullopt), Main::mkDefaultServiceIntegrationRep ()};
                    cout << m.GetServiceDescription ().fPrettyName << ": "sv << Characters::ToString (AppVersion::kVersion) << endl;
                    return EXIT_SUCCESS;
                }

                // replace preliminary logging appenders, after we've read the configuration (gAppConfiguration)
                Logger::sThe.SetAppenders ([] () {
                    static const String kAppName_          = "Stroika-Sample-HTMLUI"sv;
                    using LoggingConfigurationType         = AppConfigurationType::Logging;
                    LoggingConfigurationType loggingConfig = gAppConfiguration->fLogging.value_or (LoggingConfigurationType{});
                    Sequence<shared_ptr<Logger::IAppenderRep>> appenders;
                    if (loggingConfig.ToStdOut.value_or (LoggingConfigurationType::kToStdOut_Default)) {
                        appenders += MakeSharedPtr<Logger::StreamAppender> (IO::FileSystem::FileOutputStream::New (
                            STDOUT_FILENO, IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction));
                    }
#if qStroika_HasComponent_syslog
                    if (loggingConfig.ToSysLog.value_or (LoggingConfigurationType::kToSysLog_Default)) {
                        appenders += MakeSharedPtr<Logger::SysLogAppender> (kAppName_);
                    }
#elif qStroika_Platform_Windows
                    if (loggingConfig.ToWindowsEventLog.value_or (LoggingConfigurationType::kToWindowsEventLog_Default)) {
                        appenders += MakeSharedPtr<Logger::WindowsEventLogAppender> (kAppName_);
                    }
#endif
                    return appenders;
                }());

#if qStroika_Foundation_Debug_TraceToFile
                Logger::sThe.Log (Logger::eInfo, "Debugging Log2TraceFile: {}"_f, Debug::GetTraceFileName ());
#endif

                Options_ options{args};

                /*
                 *  Create the service manager objects
                 */
                shared_ptr<Main::IServiceIntegrationRep> serviceIntegrationRep =
                    MakeSharedPtr<Main::LoggerServiceWrapper> (Main::mkDefaultServiceIntegrationRep ());
                Main m{MakeSharedPtr<Service::SampleAppServiceRep> (options.fPortNumberOverride), serviceIntegrationRep};

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
            catch (const InvalidCommandLineArgument& e) {
                cerr << Characters::ToString (e) << endl;
                cerr << cmdLine.GenerateUsage (kAllOptions_);
                return EXIT_FAILURE;
            }
        }

    private:
        // Capture any application-specific command-line options as fields of the Options struct
        struct Options_ {
            optional<uint16_t> fPortNumberOverride;
            Main::CommandArgs  fServiceArgs;

            Options_ (const CommandLine::OptionValues& args)
                : fPortNumberOverride{args.GetArgument<uint16_t> (kPortO_)}
                , fServiceArgs{args}
            {
            }
        };
    };

}

int main (int argc, const char* argv[])
{
    try {
        CommandLine               cmdLine{argc, argv};
        Debug::TraceContextBumper ctx{"main", "argv={}"_f, cmdLine};
        MyApp_                    myApp;
        return myApp.Run (cmdLine);
    }
    catch (...) {
        cerr << endl << "EXCEPTION: {}"_f(current_exception ()) << endl;
    }
    return EXIT_SUCCESS;
}
