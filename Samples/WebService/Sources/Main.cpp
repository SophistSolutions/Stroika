/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
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

using Characters::String;
using Containers::Sequence;

using namespace StroikaSample::WebServices;

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper                            ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
    Execution::SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    uint16_t                  portNumber = 8080;
    Time::DurationSecondsType quitAfter  = numeric_limits<Time::DurationSecondsType>::max ();

    Sequence<String> args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"port")) {
            ++argi;
            if (argi != args.end ()) {
                portNumber = Characters::String2Int<uint16_t> (*argi);
            }
            else {
                cerr << "Expected arg to -port" << endl;
                return EXIT_FAILURE;
            }
        }
        else if (Execution::MatchesCommandLineArgument (*argi, L"quit-after")) {
            ++argi;
            if (argi != args.end ()) {
                quitAfter = Characters::String2Float<Time::DurationSecondsType> (*argi);
            }
            else {
                cerr << "Expected arg to -quit-after" << endl;
                return EXIT_FAILURE;
            }
        }
    }

    try {
        WebServer myWebServer{portNumber, make_shared<WSImpl> ()}; // listen and dispatch while this object exists
        Execution::WaitableEvent{}.Wait (quitAfter);               // wait forever - til user hits ctrl-c
    }
    catch (const Execution::TimeOutException&) {
        cerr << "Timed out - so - terminating..." << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
