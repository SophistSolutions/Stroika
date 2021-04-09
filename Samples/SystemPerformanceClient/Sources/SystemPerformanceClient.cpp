/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Process.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Stroika/Frameworks/SystemPerformance/AllInstruments.h"
#include "Stroika/Frameworks/SystemPerformance/Capturer.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/CPU.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Memory.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Process.h"
#include "Stroika/Frameworks/SystemPerformance/Measurement.h"

using namespace std;
using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Characters::Character;
using Characters::String;
using Containers::Sequence;
using Containers::Set;
using Execution::pid_t;
using Execution::Synchronized;
using Time::DateTime;
using Time::Duration;

namespace {
    string Serialize_ (VariantValue v, bool oneLineMode)
    {
        Streams::MemoryStream<byte>::Ptr out = Streams::MemoryStream<byte>::New ();
        DataExchange::Variant::JSON::Writer{}.Write (v, out);
        // strip CRLF - so shows up on one line
        String result = String::FromUTF8 (out.As<string> ());
        if (oneLineMode) {
            result = result.StripAll ([] (Character c) -> bool { return c == '\n' or c == '\r'; });
        }
        return result.AsNarrowSDKString ();
    }
}

namespace {
    void Demo_PrintInstruments_ ()
    {
        cout << "Instrument:" << endl;
        for (Instrument i : SystemPerformance::GetAllInstruments ()) {
            cout << "  " << i.pInstrumentName ().GetPrintName ().AsNarrowSDKString () << endl;
            // print measurements too?
        }
    }
}

namespace {
    void Demo_UsingCapturerWithCallbacks_ (Set<InstrumentNameType> run, bool oneLineMode, Duration runFor)
    {
        Capturer capturer;
        {
            CaptureSet cs;
            cs.pRunPeriod = 15s;
            for (Instrument i : SystemPerformance::GetAllInstruments ()) {
                if (not run.empty ()) {
                    if (not run.Contains (i.pInstrumentName)) {
                        continue;
                    }
                }
                cs.AddInstrument (i);
            }
            capturer.AddCaptureSet (cs);
        }
        capturer.AddMeasurementsCallback ([oneLineMode] (MeasurementSet ms) {
            cout << "    Measured-At: " << ms.fMeasuredAt.ToString ().AsNarrowSDKString () << endl;
            for (Measurement mi : ms.fMeasurements) {
                cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
            }
        });

        // run til timeout and then fall out...
        IgnoreExceptionsForCall (Execution::WaitableEvent{}.Wait (runFor));
    }
}

namespace {
    void Demo_Using_Direct_Capture_On_Instrument_ (Set<InstrumentNameType> run, bool oneLineMode, Duration captureInterval)
    {
        cout << "Results for each instrument:" << endl;
        for (Instrument i : SystemPerformance::GetAllInstruments ()) {
            if (not run.empty ()) {
                if (not run.Contains (i.pInstrumentName)) {
                    continue;
                }
            }
            cout << "  " << i.pInstrumentName ().GetPrintName ().AsNarrowSDKString () << endl;
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

namespace {
    namespace Demo_Using_Capturer_GetMostRecentMeasurements__Private_ {
        using namespace Stroika::Frameworks::SystemPerformance;

#if __cpp_designated_initializers < 201707L
        Instruments::Process::Options mkProcessInstrumentOptions_ ()
        {
            auto o            = Instruments::Process::Options{};
            o.fRestrictToPIDs = Set<pid_t>{Execution::GetCurrentProcessID ()};
            return o;
        }
#endif

        struct MyCapturer_ : Capturer {
        public:
            Instrument fCPUInstrument;
            Instrument fProcessInstrument;

            MyCapturer_ ()
                : fCPUInstrument
            {
                Instruments::CPU::GetInstrument ()
            }
#if __cpp_designated_initializers >= 201707L
            , fProcessInstrument
            {
                Instruments::Process::GetInstrument (Instruments::Process::Options{
                    .fRestrictToPIDs = Set<pid_t>{Execution::GetCurrentProcessID ()},
                })
            }
#else
            , fProcessInstrument
            {
                Instruments::Process::GetInstrument (mkProcessInstrumentOptions_ ())
            }
#endif
            {
                AddCaptureSet (CaptureSet{30s, {fCPUInstrument, fProcessInstrument}});
            }
        };
    }
    void Demo_Using_Capturer_GetMostRecentMeasurements_ ()
    {
        /*
         *  The idea here is that the capturer runs in the thread in the background capturing stuff (on a periodic schedule).
         *  and you can simply grab (ANYTIME) the most recently captured values.
         * 
         *  This also demos using 'MeasurementsAs' so you can see the measurements as a structured result, as opposed to as
         *  a variant value.
         */
        using namespace Demo_Using_Capturer_GetMostRecentMeasurements__Private_;

        static MyCapturer_ sCapturer_; // initialized threadsafe, but internally syncrhonized class

        unsigned int pass{};
        cout << "Printing most recent measurements (in loop):" << endl;
        while (true) {
            auto     measurements = sCapturer_.GetMostRecentMeasurements (); // capture results on a regular cadence with MyCapturer, and just report the latest stats
            DateTime now          = DateTime::Now ();

            optional<double> runQLength;
            optional<double> totalCPUUsage;
            if (auto om = sCapturer_.fCPUInstrument.MeasurementAs<Instruments::CPU::Info> (measurements)) {
                runQLength    = om->fRunQLength;
                totalCPUUsage = om->fTotalCPUUsage;
            }
            optional<Duration> processUptime;
            optional<double>   averageCPUTimeUsed;
            optional<uint64_t> workingOrResidentSetSize;
            optional<double>   combinedIORate;
            if (auto om = sCapturer_.fProcessInstrument.MeasurementAs<Instruments::Process::Info> (measurements)) {
                Assert (om->GetLength () == 1);
                Instruments::Process::ProcessType thisProcess = (*om)[Execution::GetCurrentProcessID ()];
                if (auto o = thisProcess.fProcessStartedAt) {
                    processUptime = now - *o;
                }
                averageCPUTimeUsed       = thisProcess.fAverageCPUTimeUsed;
                workingOrResidentSetSize = Memory::NullCoalesce (thisProcess.fWorkingSetSize, thisProcess.fResidentMemorySize);
                combinedIORate           = thisProcess.fCombinedIOWriteRate;
            }
            cout << "\tPass: " << pass << endl;
            cout << "\t\trunQLength:               " << Characters::ToString (runQLength).AsNarrowSDKString () << endl;
            cout << "\t\ttotalCPUUsage:            " << Characters::ToString (totalCPUUsage).AsNarrowSDKString () << endl;
            cout << "\t\tprocessUptime:            " << Characters::ToString (processUptime).AsNarrowSDKString () << endl;
            cout << "\t\taverageCPUTimeUsed:       " << Characters::ToString (averageCPUTimeUsed).AsNarrowSDKString () << endl;
            cout << "\t\tworkingOrResidentSetSize: " << Characters::ToString (workingOrResidentSetSize).AsNarrowSDKString () << endl;
            cout << "\t\tcombinedIORate:           " << Characters::ToString (combinedIORate).AsNarrowSDKString () << endl;
            Execution::Sleep (30s);
            pass++;
        }
    }
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*> (argv, argv + argc)).c_str ())};
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    bool                      printUsage            = false;
    bool                      mostRecentCaptureMode = false;
    bool                      printNames            = false;
    bool                      oneLineMode           = false;
    Time::DurationSecondsType runFor                = 0; // default to runfor 0, so we do each once.
    Time::DurationSecondsType captureInterval       = 1;
    Set<InstrumentNameType>   run;
    Sequence<String>          args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"h") or Execution::MatchesCommandLineArgument (*argi, L"help")) {
            printUsage = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"l")) {
            printNames = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"m")) {
            mostRecentCaptureMode = true;
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
        cerr << "    -m runs in most-recent-capture-mode" << endl;
        cerr << "    -r runs the given instrument (it can be repeated)" << endl;
        cerr << "    -t time to run for (if zero run each matching instrument once)" << endl;
        cerr << "    -c time interval between captures" << endl;
        return EXIT_SUCCESS;
    }

    try {
        if (printNames) {
            Demo_PrintInstruments_ ();
        }
        else if (mostRecentCaptureMode) {
            Demo_Using_Capturer_GetMostRecentMeasurements_ ();
        }
        else if (runFor > 0) {
            Demo_UsingCapturerWithCallbacks_ (run, oneLineMode, Duration{runFor});
        }
        else {
            Demo_Using_Direct_Capture_On_Instrument_ (run, oneLineMode, Duration{captureInterval});
        }
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
