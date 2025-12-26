/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
#if qStroika_Foundation_Common_Platform_POSIX
    SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif
    uint16_t              portNumber = 8080;
    Time::DurationSeconds quitAfter  = Time::kInfinity;

    const CommandLine::Option kPortO_{.fLongName = "port"sv, .fSupportsArgument = true};
    const CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true};

    try {
        cmdLine.Validate ({kPortO_, kQuitAfterO_});

        if (auto o = cmdLine.GetArgument (kPortO_)) {
            portNumber = Characters::String2Int<uint16_t> (*o);
        }
        if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
            quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
        }

        WebServer myWebServer{portNumber, MakeSharedPtr<WSImpl> ()}; // listen and dispatch while this object exists
        WaitableEvent{}.Wait (quitAfter);                            // wait quitAfter seconds, or til user hits ctrl-c
    }
    catch (const TimeOutException&) {
        cerr << "Timed out - so - exiting..." << endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()) << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
