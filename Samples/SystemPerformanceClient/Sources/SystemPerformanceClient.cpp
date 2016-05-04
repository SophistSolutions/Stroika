/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
#include    <cstdio>
#else
#include    <iostream>
#endif

#include    "Stroika/Foundation/Characters/FloatConversion.h"
#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/DataExchange/JSON/Writer.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#if     qPlatform_POSIX
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/Streams/MemoryStream.h"

#include    "Stroika/Frameworks/SystemPerformance/AllInstruments.h"
#include    "Stroika/Frameworks/SystemPerformance/Capturer.h"
#include    "Stroika/Frameworks/SystemPerformance/Measurement.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks;
using   namespace Stroika::Frameworks::SystemPerformance;

using   Characters::Character;
using   Characters::String;
using   Containers::Sequence;
using   Memory::Byte;
using   Memory::Optional;



namespace {
    string  Serialize_ (VariantValue v, bool oneLineMode)
    {
        Streams::MemoryStream<Byte>    out;
        DataExchange::JSON::Writer ().Write (v, out);
        // strip CRLF - so shows up on one line
        String result = String::FromUTF8 (out.As<string> ());
        if (oneLineMode) {
            result = result.StripAll ([] (Character c)-> bool { return c == '\n' or c == '\r';});
        }
        return result.AsNarrowSDKString ();
    }
}



int     main (int argc, const char* argv[])
{
#if     qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    bool                        printUsage      =   false;
    bool                        printNames      =   false;
    bool                        oneLineMode     =   false;
    Time::DurationSecondsType   runFor          =   0;              // default to runfor 0, so we do each once.
    Time::DurationSecondsType   captureInterval =   1;
    Set<InstrumentNameType> run;
    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end(); ++argi) {
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
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::fprintf (stderr, "Expected arg to -r\n");
#else
                cerr << "Expected arg to -r" << endl;
#endif
                return EXIT_FAILURE;
            }
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"t")) {
            ++argi;
            if (argi != args.end ()) {
                runFor = Characters::String2Float<Time::DurationSecondsType> (*argi);
            }
            else {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::fprintf (stderr, "Expected arg to -t\n");
#else
                cerr << "Expected arg to -t" << endl;
                return EXIT_FAILURE;
#endif
            }
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"c")) {
            ++argi;
            if (argi != args.end ()) {
                captureInterval = Characters::String2Float<Time::DurationSecondsType> (*argi);
            }
            else {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::fprintf (stderr, "Expected arg to -c\n");
#else
                cerr << "Expected arg to -c" << endl;
#endif
                return EXIT_FAILURE;
            }
        }
    }
    if (printUsage) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "Usage: SystemPerformanceClient [--help] [-h] [-l] [-f] [-r RUN-INSTRUMENT]*\n");
        (void)::fprintf (stderr, "    --help prints this help\n");
        (void)::fprintf (stderr, "    -h prints this help\n");
        (void)::fprintf (stderr, "    -o prints instrument results (with newlines stripped)\n");
        (void)::fprintf (stderr, "    -l prints only the instrument names\n");
        (void)::fprintf (stderr, "    -r runs the given instrument (it can be repeated)\n");
        (void)::fprintf (stderr, "    -t time to run for (if zero run each matching instrument once)\n");
        (void)::fprintf (stderr, "    -c time interval between captures\n");
#else
        cerr << "Usage: SystemPerformanceClient [--help] [-h] [-l] [-f] [-r RUN-INSTRUMENT]*" << endl;
        cerr << "    --help prints this help" << endl;
        cerr << "    -h prints this help" << endl;
        cerr << "    -o prints instrument results (with newlines stripped)" << endl;
        cerr << "    -l prints only the instrument names" << endl;
        cerr << "    -r runs the given instrument (it can be repeated)" << endl;
        cerr << "    -t time to run for (if zero run each matching instrument once)" << endl;
        cerr << "    -c time interval between captures" << endl;
#endif
        return EXIT_SUCCESS;
    }

    try {
        if (printNames) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            (void)::printf ("Instrument:\n");
#else
            cout << "Instrument:" << endl;
#endif
            for (Instrument i : SystemPerformance::GetAllInstruments ()) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::printf ("  %s\n", i.fInstrumentName.GetPrintName ().AsNarrowSDKString ().c_str ());
#else
                cout << "  " << i.fInstrumentName.GetPrintName ().AsNarrowSDKString () << endl;
#endif
                // print measurements too?
            }
            return EXIT_SUCCESS;
        }


        bool    useCapturer =   runFor > 0;
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
            capturer.AddMeasurementsCallback ([oneLineMode] (MeasurementSet ms) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::printf ("    Measured-At: %s\n", ms.fMeasuredAt.Format ().AsNarrowSDKString ().c_str ());
#else
                cout << "    Measured-At: " << ms.fMeasuredAt.Format ().AsNarrowSDKString () << endl;
#endif
                for (Measurement mi : ms.fMeasurements) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                    (void)::printf ("    %s: %s\n", mi.fType.GetPrintName ().AsNarrowSDKString ().c_str (), Serialize_ (mi.fValue, oneLineMode).c_str ());
#else
                    cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
#endif
                }
            });

            // run til timeout and then fall out...
            IgnoreExceptionsForCall (Execution::WaitableEvent (Execution::WaitableEvent::eAutoReset).Wait (runFor));
        }
        else {
            /*
             * Demo NOT using capturer
             */
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            (void)::printf ("Results for each instrument:\n");
#else
            cout << "Results for each instrument:" << endl;
#endif
            for (Instrument i : SystemPerformance::GetAllInstruments ()) {
                if (not run.empty ()) {
                    if (not run.Contains (i.fInstrumentName)) {
                        continue;
                    }
                }
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::printf ("  %s\n", i.fInstrumentName.GetPrintName ().AsNarrowSDKString ().c_str ());
#else
                cout << "  " << i.fInstrumentName.GetPrintName ().AsNarrowSDKString () << endl;
#endif
                Execution::Sleep (captureInterval);
                MeasurementSet m = i.Capture ();
                if (m.fMeasurements.empty ()) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                    (void)::printf ("    NO DATA\n");
#else
                    cout << "    NO DATA" << endl;
#endif
                }
                else {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                    (void)::printf ("    Measured-At: %s\n", m.fMeasuredAt.Format ().AsNarrowSDKString ().c_str ());
#else
                    cout << "    Measured-At: " << m.fMeasuredAt.Format ().AsNarrowSDKString () << endl;
#endif
                    for (Measurement mi : m.fMeasurements) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                        (void)::printf ("    %s: %s\n", mi.fType.GetPrintName ().AsNarrowSDKString ().c_str (), Serialize_ (mi.fValue, oneLineMode).c_str ());
#else
                        cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
#endif
                    }
                }
            }
        }
    }
    catch (...) {
        String  exceptMsg = Characters::ToString (current_exception ());
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "Exception - %s - terminating...\n", exceptMsg.AsNarrowSDKString ().c_str ());
#else
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
