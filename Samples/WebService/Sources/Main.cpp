/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "WSImpl.h"
#include "WebServer.h"

#include "AppVersion.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Execution;

using Characters::String;
using Containers::Sequence;
using Memory::MakeSharedPtr;

using namespace StroikaSample::WebServices;

int main (int argc, const char* argv[])
{
    CommandLine                               cmdLine{argc, argv};
    Debug::TraceContextBumper                 ctx{"main", "argv={}"_f, cmdLine};
    SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qStroika_Platform_POSIX
    SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif
    uint16_t              portNumber{}; // from --port
    Time::DurationSeconds quitAfter = Time::kInfinity;

    const CommandLine::Option kPortO_{
        .fLongName = "port"sv, .fSupportsArgument = true, .fDefaultArgument = "8080"sv, .fHelpOptionText = "specify webservice listen port"sv};
    const CommandLine::Option kQuitAfterO_{
        .fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpOptionText = "automatically quit after <argument> seconds"sv};
    const Sequence<CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, kPortO_, kQuitAfterO_};

    try {
        CommandLine::OptionValues args = cmdLine.ApplyOptions (kAllOptions_);
        if (args.Has (StandardCommandLineOptions::kHelp)) {
            cerr << cmdLine.GenerateUsage (kAllOptions_);
            return EXIT_SUCCESS;
        }
        portNumber = args.GetArgumentValue<uint16_t> (kPortO_);
        if (optional<double> o = args.GetArgument<double> (kQuitAfterO_)) {
            quitAfter = Time::DurationSeconds{*o};
        }
    }
    catch (const InvalidCommandLineArgument& e) {
        cerr << Characters::ToString (e) << endl;
        cerr << cmdLine.GenerateUsage (kAllOptions_);
        return EXIT_FAILURE;
    }

    try {
        WebServer myWebServer{portNumber, MakeSharedPtr<WSImpl> ()}; // listen and dispatch while this object exists
        WaitableEvent{}.Wait (quitAfter);                            // wait quitAfter seconds, or til user hits ctrl-c
    }
    catch (const system_error& e) {
        if (Execution::IsA (e, errc::timed_out)) {
            cerr << "Timed out - so - exiting..." << endl;
            return EXIT_SUCCESS;
        }
        cerr << "Exception - " << Characters::ToString (e) << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()) << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
