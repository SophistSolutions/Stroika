/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Debug/Main.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "Capturer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ************************* SystemPerformance::Capturer **************************
 ********************************************************************************
 */
Capturer::Capturer ()
    : pMostRecentMeasurements{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> MeasurementSet {
        const Capturer* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::pMostRecentMeasurements);
        return thisObj->fCurrentMeasurementSet_.load ();
    }}
    , pMeasurementsCallbacks{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Collection<NewMeasurementsCallbackType> {
              const Capturer* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::pMeasurementsCallbacks);
              return thisObj->fCallbacks_.load ();
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& callbacks) {
              Capturer* thisObj    = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::pMeasurementsCallbacks);
              thisObj->fCallbacks_ = callbacks;
          }}
    , pCaptureSets{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Collection<CaptureSet> {
                       const Capturer* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::pCaptureSets);
                       return thisObj->fCaptureSets_.load ();
                   },
                   [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& captureSets) {
                       Capturer* thisObj      = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::pCaptureSets);
                       thisObj->fCaptureSets_ = captureSets;
                   }}
{
    Require (Debug::AppearsDuringMainLifetime ());
}

Capturer::Capturer (const CaptureSet& cs)
    : Capturer{}
{
    AddCaptureSet (cs);
}

void Capturer::AddMeasurementsCallback (const NewMeasurementsCallbackType& cb)
{
    fCallbacks_.rwget ()->Add (cb);
}

void Capturer::RemoveMeasurementsCallback (const NewMeasurementsCallbackType& cb)
{
    fCallbacks_.rwget ()->Remove (cb);
}

void Capturer::AddCaptureSet (const CaptureSet& cs)
{
    RunnerOnce_ (cs);
    auto rwLock = fCaptureSets_.rwget ();
    rwLock->Add (cs);
    ++fCaptureSetChangeCount_;
    ManageRunner_ (true); // start while still holding lock
}

void Capturer::ManageRunner_ (bool on)
{
    bool threadPoolRunning = fThreadPool_.GetTasksCount () != 0;
    if (on) {
        if (not threadPoolRunning) {
            fThreadPool_.AddTask ([this] () { Runner_ (); });
            fThreadPool_.SetPoolSize (1);
        }
    }
    else {
        if (threadPoolRunning) {
            fThreadPool_.AbortTasks ();
            fThreadPool_.SetPoolSize (0);
        }
    }
}

void Capturer::Runner_ ()
{
    //
    // really only need one thread - and just wait right amount of time to wakeup to service next captureset.
    //
    // Compute an list of the 'next runs' (sorted by next first). Then WaitUnil () on that.
    // and then RunOnce_(thatCaptureSet); this doesn't exactly guarantee they run at the right time but close enuf and with just one thread
    //

    // First walk list of capture-sets and produce such an 'queue' as a local variable. At top of loop, recompute if change count
    // indicates in needs recomputing (rare). Else just keep adjusting it.
    uint64_t changeCountForCaptureSet{0};

    // NOTE - we use a SortedMapping instead of an acutal Queue, because we always know WHEN each item should run next, and a sorted mapping
    // tells is quicly and easily the 'next' item to run. If we used a Q, we would add back the just ran item at the end but it might
    // not belong at the end of the Queue cuz it runs on a different schedule
    SortedMapping<TimePointSeconds, CaptureSet> runQueue;

    auto recomputeSortOrder = [&] () {
        TimePointSeconds now  = Time::GetTickCount ();
        auto             lock = fCaptureSets_.cget (); // hold lock until I've examined changeCount

        // it would be nice to be able to preserve the timing info in the runQueue for items not changed, but that would require
        // being able to tell which CaptureSets were new and old, which is not always possible given the current API (SetCaptureSet).
        // and updating the capture sets should be rare, so it should cause little problem to recompute it, and reset the counters.
        runQueue.clear ();
        SortedMapping<TimePointSeconds, CaptureSet> tmp;
        for (const auto& i : lock.load ()) {
            runQueue.Add (now + i.pRunPeriod (), i);
        }
        changeCountForCaptureSet = fCaptureSetChangeCount_;
    };

    // Note this runs in a threadpool which can be canceled as needed, so this need not check for a termination condition - it will be ended by a thread abort
    while (true) {
        if (changeCountForCaptureSet < fCaptureSetChangeCount_) {
            recomputeSortOrder (); // updates runQueue as a side-effect
        }

        Assert (not runQueue.empty ()); // because otherwise the thread would have been aborted, and we wouldn't get this far (race??? - maybe need to do ifcheck)

        // otherwise pop the first item from the Q, and wait til then. Then process it, and push it back onto the Q with the appropriate
        // 'next' time.
        auto iterator = runQueue.begin ();
        Assert (iterator != runQueue.end ());
        KeyValuePair<TimePointSeconds, CaptureSet> runNext = *iterator;
        Execution::SleepUntil (runNext.fKey);
        RunnerOnce_ (runNext.fValue);
        runQueue.erase (iterator);
        runQueue.Add (runNext.fKey + runNext.fValue.pRunPeriod (), runNext.fValue); // interpret time offset as wrt leading edge
    }
}

void Capturer::RunnerOnce_ (const CaptureSet& cs)
{
    MeasurementSet measurements;
    for (Instrument i : cs.pInstruments ()) {
        try {
            measurements.MergeAdditions (i.Capture ());
        }
        catch (const Execution::Thread::AbortException&) {
            Execution::ReThrow ();
        }
        catch (...) {
            using namespace Characters::Literals;
            DbgTrace ("Eating exception in Capturer runner"_f);
        }
    }
    UpdateMeasurementSet_ (measurements);
}

void Capturer::UpdateMeasurementSet_ (const MeasurementSet& ms)
{
    fCurrentMeasurementSet_.rwget ()->MergeAdditions (ms);
    for (const auto& cb : fCallbacks_.load ()) {
        cb (ms);
    }
}
