/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/DataExchange/JSON/Writer.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#if     qPlatform_POSIX
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"

#include    "Stroika/Frameworks/SystemPerformance/AllInstruments.h"
#include    "Stroika/Frameworks/SystemPerformance/Measurement.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks;
using   namespace Stroika::Frameworks::SystemPerformance;

using   Characters::Character;
using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;



namespace {
    string  serialize_ (VariantValue v)
    {
        Streams::BasicBinaryOutputStream    out;
        DataExchange::JSON::Writer ().Write (v, out);
        // strip CRLF - so shows up on one line
        return String::FromUTF8 (out.As<string> ()).StripAll ([] (Character c)-> bool { return c == '\n' or c == '\r';}).AsNarrowSDKString ();
    }
}



int main (int argc, const char* argv[])
{
#if     qPlatform_POSIX
    // Many performance instruments use pipes
    // @todo - REVIEW IF REALLY NEEDED AND WHY? SO LONG AS NO FAIL SHOULDNT BE?
    //  --LGP 2014-02-05
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (Execution::SignalHandlerRegistry::kIGNORED);
#endif
    bool                    printUsage  =   false;
    bool                    printNames  =   false;
    Set<InstrumentNameType> run;
    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end(); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"h")) {
            printUsage = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"l")) {
            printNames = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"r")) {
            ++argi;
            if (argi != args.end ()) {
                run.Add (*argi);
            }
            else {
                cerr << "Expected arg to -r" << endl;
                return EXIT_FAILURE;
            }
        }
    }
    if (printUsage) {
        cerr << "Usage: SystemPerformanceClient [-h] [-l] [-r RUN-INSTRUMENT]*" << endl;
        return EXIT_SUCCESS;
    }

    try {
        if (printNames) {
            cout << "Instrument:" << endl;
            for (Instrument i : SystemPerformance::GetAllInstruments ()) {
                cout << "  " << i.fInstrumentName.GetPrintName ().AsNarrowSDKString () << endl;
                // print measurements too?
            }
            return EXIT_SUCCESS;
        }

        cout << "Results for each instrument:" << endl;
        for (Instrument i : SystemPerformance::GetAllInstruments ()) {
            if (not run.empty ()) {
                if (not run.Contains (i.fInstrumentName)) {
                    continue;
                }
            }
            cout << "  " << i.fInstrumentName.GetPrintName ().AsNarrowSDKString () << endl;
            Measurements m = i.fCaptureFunction ();
            if (m.fMeasurements.empty ()) {
                cout << "    NO DATA";
            }
            else {
                cout << "    Measured-At: " << m.fMeasuredAt.Format ().AsNarrowSDKString () << endl;
                for (Measurement mi : m.fMeasurements) {
                    cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << serialize_ (mi.fValue) << endl;
                }
            }
        }
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
