/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Fatal.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Thread.h"
#if qStroika_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#include "Stroika/Foundation/Execution/Platform/Windows/StructuredException.h"
#endif
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
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
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Frameworks::Service;

using Containers::Sequence;
using Memory::MakeSharedPtr;

#if qUseLogger
#include "Stroika/Foundation/Debug/BackTrace.h"
#include "Stroika/Foundation/Execution/Logger.h"
#endif

int main (int argc, const char* argv[])
{
    CommandLine               cmdLine{argc, argv};
    Debug::TraceContextBumper ctx{"main", "argv={}"_f, cmdLine};

#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
    [[maybe_unused]] auto&& cleanupReport =
        Finally ([] () { DbgTrace ("Exiting main with thread {} running"_f, Thread::GetStatistics ().fRunningThreads); });
#endif

    /*
     *  This allows for safe signals to be managed in a threadsafe way
     */
    SignalHandlerRegistry::SafeSignalsManager safeSignals;

    /*
     *  Setup basic (optional) error handling.
     */
#if qStroika_Platform_Windows
    Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
    Execution::Platform::Windows::RegisterDefaultHandler_StructuredException ();
#endif
    Debug::RegisterDefaultFatalErrorHandlers (DefaultLoggingFatalErrorHandler);

    /*
     *  SetStandardCrashHandlerSignals not really needed, but helpful for many applications so you get a decent log message/debugging on crash.
     */
    SignalHandlerRegistry::sThe.SetStandardCrashHandlerSignals (SignalHandler{DefaultLoggingCrashSignalHandler, SignalHandler::Type::eDirect});

    /*
     *  Ignore SIGPIPE is common practice/helpful in POSIX, but not required by the service manager.
     */
#if qStroika_Platform_POSIX
    SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif

    /*
     *  Setup Logging to the OS logging facility.
     */
#if qUseLogger
    /*
     *  Optional - use buffering feature
     *  Optional - use suppress duplicates in a 15 second window
     */
    Logger::Activator loggerActivation{Logger::Options{
        .fLogBufferingEnabled         = true,
        .fSuppressDuplicatesThreshold = 15s,
    }};
    bool              dockerContainerFlag = false; // get from command-line???
    if (dockerContainerFlag) {
        using namespace IO::FileSystem;
        Logger::sThe.AddAppender (
            MakeSharedPtr<Logger::StreamAppender> (FileOutputStream::New (STDOUT_FILENO, FileStream::AdoptFDPolicy::eDisconnectOnDestruction)));
    }
    else {
#if qStroika_HasComponent_syslog
        Logger::sThe.SetAppenders (MakeSharedPtr<Logger::SysLogAppender> ("Stroika-Sample-Service"sv));
#elif qStroika_Platform_Windows
        Logger::sThe.SetAppenders (MakeSharedPtr<Logger::WindowsEventLogAppender> ("Stroika-Sample-Service"sv));
#endif
    }
#endif

#if qStroika_Foundation_Debug_TraceToFile
    Logger::sThe.Log (Logger::eInfo, "Debugging Log2TraceFile: {}"_f, Debug::GetTraceFileName ());
#endif

    /*
     *  Parse command line arguments, and start looking at options.
     */
    shared_ptr<Main::IServiceIntegrationRep> serviceIntegrationRep = Main::mkDefaultServiceIntegrationRep ();
#if qUseLogger
    serviceIntegrationRep = MakeSharedPtr<Main::LoggerServiceWrapper> (serviceIntegrationRep);
#endif

    /*
     *  Create service handler instance.
     */
    Main m{MakeSharedPtr<Samples::Service::SampleAppServiceRep> (), serviceIntegrationRep};

    /*
     *  Run request.
     */
    using StandardCommandLineOptions::kHelp;
    using StandardCommandLineOptions::kVersion;
    Sequence<CommandLine::Option> allMyOptions =
        Sequence<CommandLine::Option>{Main::CommandOptions::kAll} + Sequence<CommandLine::Option>{kHelp, kVersion};
    try {
        CommandLine::OptionValues args = cmdLine.ApplyOptions (allMyOptions); // throws InvalidCommandLineArgument, as m.Run () can

        if (args.Has (Main::CommandOptions::kStatus)) {
            cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
            return EXIT_SUCCESS;
        }
        else if (args.Has (kHelp)) {
            cerr << cmdLine.GenerateUsage (allMyOptions);
            return EXIT_SUCCESS;
        }
        else if (args.Has (kVersion)) {
            cout << m.GetServiceDescription ().fPrettyName << ": "sv << Characters::ToString (AppVersion::kVersion) << endl;
            return EXIT_SUCCESS;
        }
        else {
            /*
             *  Run the commands, and capture/display exceptions
             */
            m.Run (args);
        }
    }
    catch (const InvalidCommandLineArgument& e) {
        cerr << Characters::ToString (e) << endl;
        cerr << cmdLine.GenerateUsage (allMyOptions);
        return EXIT_FAILURE;
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
#if qUseLogger
        Logger::sThe.Log (Logger::eError, "{}"_f, exceptMsg);
#endif
        cerr << "FAILED: " << exceptMsg << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
