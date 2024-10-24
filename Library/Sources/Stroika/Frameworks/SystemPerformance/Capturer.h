/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Capturer_h_
#define _Stroika_Framework_SystemPerformance_Capturer_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "CaptureSet.h"
#include "Measurement.h"

/*
 * TODO:
 *      @todo   Somehow make captrueset comparable, so that we can use it as a KEY with mreasumrnes
 *              and then store latest for each captureset.
 *
 *              Then combine into one massive measuremnets list fMeasurmentsMapping.Values() (once
 *              we have impelemtend values() on mapping).
 *
 *      @todo   make it optional storing the most recent measurement set.
 *
 *      @todo   Use threadpool CORRECTLY!. Maybe push ALL measurements into
 *              threadpool at just the right time, and then paralell process as much as the threadpool size allows.
 *              That seems the best appraoch! Default to theadpool size of one! One recurring task in threadpool
 *              would be to re-schedule rest of measurements...
 */

namespace Stroika::Frameworks::SystemPerformance {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Collection;
    using Containers::Set;
    using Execution::Function;
    using Time::Duration;

    /**
     *  A Capturer is a utility you MAY wish to use the the SystemPerformance framework. It provides some
     *  fairly handy default mechanisms to manage (possibly multiple) CaptureSets, and either store the last result
     *  for each, or to run callbacks on those results.
     *
     *  This also runs its capturing on a (single) background thread (so that captures / averages are over a consistent time interval). 
     *  This has implications for how much its able to keep up with and maintain all the measurements in question.
     *
     *  Note - there is no reason you cannot use the rest of the SystemPerformance framework without this class,
     *  if its pattern doesn't meet your needs.
     * 
     *  \req  Debug::AppearsDuringMainLifetime() at all points during the Capturer lifetime since it owns threads.
     *
     *  \par Example Usage
     *      This example shows using capturer with callbacks, and raw mode - treating the measurements as 'untyped'
     *      VariantValue objects.
     * 
     *      \code
     *          Capturer capturer;
     *          {
     *              CaptureSet cs;
     *              cs.runPeriod = 15s;
     *              for (Instrument i : SystemPerformance::GetAllInstruments ()) {
     *                  cs.AddInstrument (i);
     *              }
     *              capturer.AddCaptureSet (cs);
     *          }
     *          capturer.AddMeasurementsCallback ([oneLineMode] (MeasurementSet ms) {
     *              cout << "    Measured-At: " << ms.fMeasuredAt.ToString () << endl;
     *              for (Measurement mi : ms.fMeasurements) {
     *                  cout << "    " << mi.fType.GetPrintName () << ": " << Serialize_ (mi.fValue, oneLineMode) << endl;
     *              }
     *          });
     *          // run til timeout and then fall out...
     *          IgnoreExceptionsForCall (Execution::WaitableEvent{}.Wait (runFor));
     *      \endcode
     * 
     *  \par Example Usage
     *      This example shows using capturer, running in the background at regular time intervals, and allows callers to capture bits
     *      of performance status (like to report in web service) - just reporting the latest captured data as of when called.
     * 
     *      \code
     *          struct MyCapturer_ : Capturer {
     *          public:
     *              Instruments::CPU::Instrument     fCPUInstrument;
     *              Instruments::Process::Instrument fProcessInstrument;
     *              MyCapturer_ ()
     *                  : fProcessInstrument{Instruments::Process::Options{
     *                      .fRestrictToPIDs           = Set<pid_t>{Execution::GetCurrentProcessID ()},
     *                  }}
     *              {
     *                  AddCaptureSet (CaptureSet{30s, {fCPUInstrument, fProcessInstrument}});
     *              }
     *          };
     * 
     *          MyCapturer_ capturer;  // capturer internally syncrhonized so no need for extra sync...; must must respect Require (Debug::AppearsDuringMainLifetime ());
     *
     *          // Now do from any thread as often as desired, reporting latest data:
     *          auto     measurements = capturer.GetMostRecentMeasurements (); // capture results on a regular cadence with MyCapturer, and just report the latest stats
     *          DateTime now          = DateTime::Now ();
     *          optional<double> runQLength;
     *          optional<double> totalCPUUsage;
     *          if (auto om = capturer.fCPUInstrument.MeasurementAs<Instruments::CPU::Info> (measurements)) {
     *              runQLength    = om->fRunQLength;
     *              totalCPUUsage = om->fTotalCPUUsage;
     *          }
     *          optional<Duration> processUptime;
     *          optional<double>   averageCPUTimeUsed;
     *          optional<uint64_t> workingOrResidentSetSize;
     *          optional<double>   combinedIORate;
     *          if (auto om = capturer.fProcessInstrument.MeasurementAs<Instruments::Process::Info> (measurements)) {
     *              Assert (om->size () == 1);
     *              Instruments::Process::ProcessType thisProcess = (*om)[Execution::GetCurrentProcessID ()];
     *              if (auto o = thisProcess.fProcessStartedAt) {
     *                  processUptime = now - *o;
     *              }
     *              averageCPUTimeUsed       = thisProcess.fAverageCPUTimeUsed;
     *              workingOrResidentSetSize = Memory::NullCoalesce (thisProcess.fWorkingSetSize, thisProcess.fResidentMemorySize);
     *              combinedIORate           = thisProcess.fCombinedIOWriteRate;
     *          }
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    class Capturer {
    public:
        /**
         *  Note - if the constructor is called with explicit capture-sets, then these are run once before the constructor returns
         *  (so the caller can read current value)
         */
        Capturer ();
        Capturer (const CaptureSet& cs);
        Capturer (const Capturer&) = delete;

    public:
        nonvirtual Capturer& operator= (const Capturer&) = delete;

    public:
        /**
         *  Read this anytime (for example if you don't want to bother with callbacks or if
         *  some other process needs to query the latest values from the instrument measurers.
         */
        Common::ReadOnlyProperty<MeasurementSet> pMostRecentMeasurements;

    public:
        /**
         */
        using NewMeasurementsCallbackType = Function<void (const MeasurementSet&)>;

    public:
        /**
         */
        Common::Property<Collection<NewMeasurementsCallbackType>> measurementsCallbacks;

    public:
        /**
         */
        nonvirtual void AddMeasurementsCallback (const NewMeasurementsCallbackType& cb);

    public:
        /**
         */
        nonvirtual void RemoveMeasurementsCallback (const NewMeasurementsCallbackType& cb);

    public:
        /**
         */
        Common::Property<Collection<CaptureSet>> captureSets;

    public:
        /**
         */
        nonvirtual void AddCaptureSet (const CaptureSet& cs);

    private:
        nonvirtual void ManageRunner_ (bool on);

    private:
        nonvirtual void Runner_ ();

    private:
        nonvirtual void RunnerOnce_ (const CaptureSet& cs);

    private:
        // FOR NOW - just assign/overwrite the latest measurement set, and call
        // callbacks as needed
        nonvirtual void UpdateMeasurementSet_ (const MeasurementSet& ms);

    private:
        Execution::Synchronized<Collection<CaptureSet>> fCaptureSets_;
        uint64_t fCaptureSetChangeCount_{0}; // doesn't need to be atomic because only updated/checked holdign capturesets lock
        Execution::Synchronized<Collection<NewMeasurementsCallbackType>> fCallbacks_;
        Execution::Synchronized<MeasurementSet>                          fCurrentMeasurementSet_;
        Execution::ThreadPool fThreadPool_; // Subtle - construct last so auto-destructed first (shuts down threads)
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Capturer.inl"

#endif /*_Stroika_Framework_SystemPerformance_Capturer_h_*/
