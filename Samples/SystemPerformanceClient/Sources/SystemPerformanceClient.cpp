/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Stroika/Frameworks/SystemPerformance/AllInstruments.h"
#include "Stroika/Frameworks/SystemPerformance/Capturer.h"
#include "Stroika/Frameworks/SystemPerformance/Measurement.h"

using namespace std;
using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Characters::Character;
using Characters::String;
using Containers::Sequence;

namespace {
    string Serialize_ (VariantValue v, bool oneLineMode)
    {
        Streams::MemoryStream<byte>::Ptr out = Streams::MemoryStream<byte>::New ();
        DataExchange::Variant::JSON::Writer ().Write (v, out);
        // strip CRLF - so shows up on one line
        String result = String::FromUTF8 (out.As<string> ());
        if (oneLineMode) {
            result = result.StripAll ([](Character c) -> bool { return c == '\n' or c == '\r'; });
        }
        return result.AsNarrowSDKString ();
    }
}

int main (int argc, const char* argv[])
{
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    bool                      printUsage      = false;
    bool                      printNames      = false;
    bool                      oneLineMode     = false;
    Time::DurationSecondsType runFor          = 0; // default to runfor 0, so we do each once.
    Time::DurationSecondsType captureInterval = 1;
    Set<InstrumentNameType>   run;
    Sequence<String>          args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"h") or Execution::MatchesCommandLineArgument (*argi, L"help")) {
            printUsage = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"l")) {
            printNames = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"o")) {
            oneLineMode = true;
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
        if (Execution::MatchesCommandLineArgument (*argi, L"t")) {
            ++argi;
            if (argi != args.end ()) {
                runFor = Characters::String2Float<Time::DurationSecondsType> (*argi);
            }
            else {
                cerr << "Expected arg to -t" << endl;
                return EXIT_FAILURE;
            }
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"c")) {
            ++argi;
            if (argi != args.end ()) {
                captureInterval = Characters::String2Float<Time::DurationSecondsType> (*argi);
            }
            else {
                cerr << "Expected arg to -c" << endl;
                return EXIT_FAILURE;
            }
        }
    }
    if (printUsage) {
        cerr << "Usage: SystemPerformanceClient [--help] [-h] [-l] [-f] [-r RUN-INSTRUMENT]*" << endl;
        cerr << "    --help prints this help" << endl;
        cerr << "    -h prints this help" << endl;
        cerr << "    -o prints instrument results (with newlines stripped)" << endl;
        cerr << "    -l prints only the instrument names" << endl;
        cerr << "    -r runs the given instrument (it can be repeated)" << endl;
        cerr << "    -t time to run for (if zero run each matching instrument once)" << endl;
        cerr << "    -c time interval between captures" << endl;
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

        bool useCapturer = runFor > 0;
        if (useCapturer) {
            /*
             * Demo using capturer
             */
            Capturer capturer;
            {
                CaptureSet cs;
                cs.SetRunPeriod (Duration (15));
                for (Instrument i : SystemPerformance::GetAllInstruments ()) {
                    if (not run.empty ()) {
                        if (not run.Contains (i.fInstrumentName)) {
                            continue;
                        }
                    }
                    cs.AddInstrument (i);
                }
                capturer.AddCaptureSet (cs);
            }
            capturer.AddMeasurementsCallback ([oneLineMode](MeasurementSet ms) {
                cout << "    Measured-At: " << ms.fMeasuredAt.ToString ().AsNarrowSDKString () << endl;
                for (Measurement mi : ms.fMeasurements) {
                    cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
                }
            });

            // run til timeout and then fall out...
            IgnoreExceptionsForCall (Execution::WaitableEvent{}.Wait (runFor));
        }
        else {
            /*
             * Demo NOT using capturer
             */
            cout << "Results for each instrument:" << endl;
            for (Instrument i : SystemPerformance::GetAllInstruments ()) {
                if (not run.empty ()) {
                    if (not run.Contains (i.fInstrumentName)) {
                        continue;
                    }
                }
                cout << "  " << i.fInstrumentName.GetPrintName ().AsNarrowSDKString () << endl;
                Execution::Sleep (captureInterval);
                MeasurementSet m = i.Capture ();
                if (m.fMeasurements.empty ()) {
                    cout << "    NO DATA" << endl;
                }
                else {
                    cout << "    Measured-At: " << m.fMeasuredAt.ToString ().AsNarrowSDKString () << endl;
                    for (Measurement mi : m.fMeasurements) {
                        cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
                    }
                }
            }
        }
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
