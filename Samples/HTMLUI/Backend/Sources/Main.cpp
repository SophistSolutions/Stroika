/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"

#include "WSImpl.h"
#include "WebServer.h"

#include "AppVersion.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;

using Characters::String;
using Containers::Sequence;

using namespace StroikaSample::WebServices;

int main (int argc, const char* argv[])
{
    Execution::CommandLine                               cmdLine{argc, argv};
    Debug::TraceContextBumper                            ctx{"main", "argv={}"_f, cmdLine};
    Execution::SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    uint16_t              portNumber = 8080;
    Time::DurationSeconds quitAfter  = Time::kInfinity;

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

        WebServer myWebServer{portNumber, make_shared<WSImpl> ()}; // listen and dispatch while this object exists
        Execution::WaitableEvent{}.Wait (quitAfter);               // wait quitAfter seconds, or til user hits ctrl-c
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
}
