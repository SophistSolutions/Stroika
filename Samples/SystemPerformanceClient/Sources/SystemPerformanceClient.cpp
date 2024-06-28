/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
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
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Characters::Character;
using Characters::String;
using Containers::Sequence;
using Containers::Set;
using Execution::pid_t;
using Execution::Synchronized;
using Time::DateTime;
using Time::DisplayedRealtimeClock;
using Time::Duration;
using Time::TimePointSeconds;

namespace {
    string Serialize_ (VariantValue v, bool oneLineMode)
    {
        Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
        DataExchange::Variant::JSON::Writer{}.Write (v, out);
        // strip CRLF - so shows up on one line
        String result = String::FromUTF8 (out.As<string> ());
        if (oneLineMode) {
            result = result.StripAll ([] (Character c) -> bool { return c == '\n' or c == '\r'; });
        }
        return result.AsNarrowSDKString ();
    }
#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    template <typename X>
    using RANGE_TEMPLATE_BWA = Range<X>;
#endif
    Range<DisplayedRealtimeClock::time_point> toDisp_ (Range<TimePointSeconds> tpRange)
    {
#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
        return Time::clock_cast<DisplayedRealtimeClock, RANGE_TEMPLATE_BWA> (tpRange);
#elif qCompilerAndStdLib_template_template_auto_deduced_Buggy
        return Time::clock_cast<DisplayedRealtimeClock, Range> (tpRange);
#else
        return Time::clock_cast<DisplayedRealtimeClock> (tpRange);
#endif
    }
    void Demo_PrintInstruments_ ()
    {
        cout << "Instrument:" << endl;
        for (const Instrument& i : SystemPerformance::GetAllInstruments ()) {
            cout << "  " << i.instrumentName ().GetPrintName ().AsNarrowSDKString () << endl;
            // print measurements too?
        }
    }
    void Demo_UsingCapturerWithCallbacks_ (Set<InstrumentNameType> run, bool oneLineMode, Duration captureInterval, Duration runFor)
    {
        Capturer capturer;
        {
            CaptureSet cs;
            cs.runPeriod = captureInterval;
            for (const Instrument& i : SystemPerformance::GetAllInstruments ()) {
                if (not run.empty ()) {
                    if (not run.Contains (i.instrumentName)) {
                        continue;
                    }
                }
                cs.AddInstrument (i);
            }
            capturer.AddCaptureSet (cs);
        }
        capturer.AddMeasurementsCallback ([oneLineMode] (MeasurementSet ms) {
            cout << "    Measured-At: " << toDisp_ (ms.fMeasuredAt).ToString ().AsNarrowSDKString () << endl;
            for (const Measurement& mi : ms.fMeasurements) {
                cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
            }
        });

        // run til timeout and then fall out...
        IgnoreExceptionsForCall (Execution::WaitableEvent{}.Wait (runFor));
    }
    void Demo_Using_Direct_Capture_On_Instrument_ (Set<InstrumentNameType> run, bool oneLineMode, Duration captureInterval)
    {
        cout << "Results for each instrument:" << endl;
        for (Instrument i : SystemPerformance::GetAllInstruments ()) {
            if (not run.empty ()) {
                if (not run.Contains (i.instrumentName)) {
                    continue;
                }
            }
            cout << "  " << i.instrumentName ().GetPrintName ().AsNarrowSDKString () << endl;
            Execution::Sleep (captureInterval);
            MeasurementSet m = i.Capture ();
            if (m.fMeasurements.empty ()) {
                cout << "    NO DATA" << endl;
            }
            else {
                cout << "    Measured-At: " << toDisp_ (m.fMeasuredAt).ToString ().AsNarrowSDKString () << endl;
                for (const Measurement& mi : m.fMeasurements) {
                    cout << "    " << mi.fType.GetPrintName ().AsNarrowSDKString () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
                }
            }
        }
    }
}

namespace {
    namespace Demo_Using_Capturer_GetMostRecentMeasurements__Private_ {
        using namespace Stroika::Frameworks::SystemPerformance;

        struct MyCapturer_ : Capturer {
        public:
            Instruments::CPU::Instrument     fCPUInstrument;
            Instruments::Process::Instrument fProcessInstrument;

            MyCapturer_ ()
                : fProcessInstrument{Instruments::Process::Options{.fRestrictToPIDs = Set<pid_t>{Execution::GetCurrentProcessID ()}}}
            {
                AddCaptureSet (CaptureSet{30s, {fCPUInstrument, fProcessInstrument}});
            }
        };
    }
    void Demo_Using_Capturer_GetMostRecentMeasurements_ (const Duration& runFor)
    {
        /*
         *  The idea here is that the capturer runs in the thread in the background capturing stuff (on a periodic schedule).
         *  and you can simply grab (ANYTIME) the most recently captured values.
         * 
         *  This also demos using 'MeasurementsAs' so you can see the measurements as a structured result, as opposed to as
         *  a variant value.
         */
        using namespace Demo_Using_Capturer_GetMostRecentMeasurements__Private_;

        MyCapturer_ capturer; // initialized threadsafe, but internally syncrhonized class

        Time::TimePointSeconds doneAt = Time::GetTickCount () + runFor;
        unsigned int           pass{};
        cout << "Printing most recent measurements (in loop):" << endl;
        while (Time::GetTickCount () < doneAt) {
            auto measurements = capturer.pMostRecentMeasurements (); // capture results on a regular cadence with MyCapturer, and just report the latest stats
            DateTime now = DateTime::Now ();

            optional<double> runQLength;
            optional<double> totalCPUUsage;
            optional<double> totalCPURatio;
            if (auto om = capturer.fCPUInstrument.MeasurementAs<Instruments::CPU::Info> (measurements)) {
                runQLength    = om->fRunQLength;
                totalCPUUsage = om->fTotalCPUUsage;
                totalCPURatio = om->GetTotalCPURatio ();
            }
            optional<Duration> thisProcUptime;
            optional<Duration> thisProcAverageCPUTimeUsed;
            optional<uint64_t> thisProcWorkingOrResidentSetSize;
            optional<double>   thisProcCombinedIORate;
            if (auto om = capturer.fProcessInstrument.MeasurementAs<Instruments::Process::Info> (measurements)) {
                // It might not be found for some instruments (not implemented?)
                Assert (om->size () <= 1);
                if (om->size () == 1) {
                    Instruments::Process::ProcessType thisProcess = (*om)[Execution::GetCurrentProcessID ()];
                    if (auto o = thisProcess.fProcessStartedAt) {
                        thisProcUptime = now - *o;
                    }
                    thisProcAverageCPUTimeUsed       = thisProcess.fAverageCPUTimeUsed;
                    thisProcWorkingOrResidentSetSize = Memory::NullCoalesce (thisProcess.fWorkingSetSize, thisProcess.fResidentMemorySize);
                    thisProcCombinedIORate           = thisProcess.fCombinedIOWriteRate;
                }
            }
            using namespace Memory; // for optional operator overloads
            cout << "\tPass: " << pass << endl;
            cout << "\t\tSys: " << endl;
            cout << "\t\t\tRun-Q Length:                 " << Characters::ToString (runQLength).AsNarrowSDKString () << endl;
            cout << "\t\t\tTotal CPU Usage:              " << Characters::ToString (totalCPUUsage).AsNarrowSDKString () << " ("
                 << Characters::ToString (totalCPURatio * 100.0).AsNarrowSDKString () << "% of computer)" << endl;
            cout << "\t\tThis Process: " << endl;
            cout << "\t\t\tUptime:                       " << Characters::ToString (thisProcUptime).AsNarrowSDKString () << endl;
            cout << "\t\t\tAverage CPU Time Used:        " << Characters::ToString (thisProcAverageCPUTimeUsed).AsNarrowSDKString () << endl;
            cout << "\t\t\tWorking Or Resident-Set Size: " << Characters::ToString (thisProcWorkingOrResidentSetSize).AsNarrowSDKString () << endl;
            cout << "\t\t\tCombined IO Rate:             " << Characters::ToString (thisProcCombinedIORate).AsNarrowSDKString () << endl;
            Execution::Sleep (30s);
            ++pass;
        }
    }
}

int main (int argc, const char* argv[])
{
    Execution::CommandLine    cmdLine{argc, argv};
    Debug::TraceContextBumper ctx{"main", "argv={}"_f, cmdLine};
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    using namespace Execution::StandardCommandLineOptions;
    const Execution::CommandLine::Option kPrintNamesO_{.fSingleCharName = 'l', .fHelpOptionText = "prints only the instrument names"sv};
    const Execution::CommandLine::Option kMostRecentO_{.fSingleCharName = 'm', .fHelpOptionText = "runs in most-recent-capture-mode"sv};
    const Execution::CommandLine::Option kOneLineModeO_{.fSingleCharName = 'o', .fHelpOptionText = "prints instrument results (with newlines stripped)"sv};
    const Execution::CommandLine::Option kRunInstrumentArg_{.fSingleCharName   = 'r',
                                                            .fSupportsArgument = true,
                                                            .fRepeatable       = true,
                                                            .fHelpArgName      = "RUN-INSTRUMENT"sv,
                                                            .fHelpOptionText   = "runs the given instrument (it can be repeated)"sv};
    const Execution::CommandLine::Option kRunForO_{
        .fSingleCharName = 't', .fSupportsArgument = true, .fHelpOptionText = "time to run for (if zero run each matching instrument once)"sv};
    const Execution::CommandLine::Option kTimeBetweenCapturesO_{
        .fSingleCharName = 'c', .fSupportsArgument = true, .fHelpArgName = "NSEC"sv, .fHelpOptionText = "time interval between captures"sv};

    const initializer_list<Execution::CommandLine::Option> kAllOptions = {
        kHelp, kPrintNamesO_, kMostRecentO_, kOneLineModeO_, kRunInstrumentArg_, kRunForO_, kTimeBetweenCapturesO_};

    bool                  printUsage            = cmdLine.Has (kHelp);
    bool                  mostRecentCaptureMode = cmdLine.Has (kMostRecentO_);
    bool                  printNames            = cmdLine.Has (kPrintNamesO_);
    bool                  oneLineMode           = cmdLine.Has (kOneLineModeO_);
    Time::DurationSeconds runFor                = 0s; // default to runfor 0, so we do each once.
    if (auto o = cmdLine.GetArgument (kRunForO_)) {
        runFor = Duration{Characters::FloatConversion::ToFloat<Duration::rep> (*o)};
    }
    Time::DurationSeconds captureInterval = 15s;
    if (auto o = cmdLine.GetArgument (kTimeBetweenCapturesO_)) {
        captureInterval = Duration{Characters::FloatConversion::ToFloat<Duration::rep> (*o)};
    }
    Set<InstrumentNameType> run =
        cmdLine.GetArguments (kRunInstrumentArg_).Map<Set<InstrumentNameType>> ([] (const String& s) { return InstrumentNameType{s}; });

    if (printUsage) {
        cerr << Execution::CommandLine::GenerateUsage ("SystemPerformanceClient"sv, kAllOptions).AsNarrowSDKString ();
        return EXIT_SUCCESS;
    }

    try {
        cmdLine.Validate ({kHelp, kPrintNamesO_, kMostRecentO_, kOneLineModeO_, kRunInstrumentArg_, kRunForO_, kTimeBetweenCapturesO_});
        if (printNames) {
            Demo_PrintInstruments_ ();
        }
        else if (mostRecentCaptureMode) {
            Demo_Using_Capturer_GetMostRecentMeasurements_ (runFor);
        }
        else if (runFor > 0s) {
            Demo_UsingCapturerWithCallbacks_ (run, oneLineMode, captureInterval, runFor);
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
