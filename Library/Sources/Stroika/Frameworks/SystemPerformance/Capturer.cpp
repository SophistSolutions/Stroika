/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/SortedMapping.h"
#include "../../Foundation/Execution/Sleep.h"

#include "Capturer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ************************* SystemPerformance::Capturer **************************
 ********************************************************************************
 */
Capturer::Capturer (const CaptureSet& cs)
{
    AddCaptureSet (cs);
}

Collection<Capturer::NewMeasurementsCallbackType> Capturer::GetMeasurementsCallbacks () const
{
    return fCallbacks_;
}

void Capturer::SetMeasurementsCallbacks (const Collection<NewMeasurementsCallbackType>& callbacks)
{
    fCallbacks_ = callbacks;
}

void Capturer::AddMeasurementsCallback (const NewMeasurementsCallbackType& cb)
{
    fCallbacks_.rwget ()->Add (cb);
}

void Capturer::RemoveMeasurementsCallback (const NewMeasurementsCallbackType& cb)
{
    fCallbacks_.rwget ()->Remove (cb);
}

Collection<CaptureSet> Capturer::GetCaptureSets () const
{
    return fCaptureSets_;
}

void Capturer::SetCaptureSets (const Collection<CaptureSet>& captureSets)
{
    auto rwLock = fCaptureSets_.rwget ();
    rwLock.store (captureSets);
    fCaptureSetChangeCount_++;
    ManageRunner_ (not captureSets.empty ());
}

void Capturer::AddCaptureSet (const CaptureSet& cs)
{
    RunnerOnce_ (cs);
    auto rwLock = fCaptureSets_.rwget ();
    rwLock->Add (cs);
    fCaptureSetChangeCount_++;
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
    SortedMapping<DurationSecondsType, CaptureSet> runQueue;

    auto recomputeSortOrder = [&] () {
        DurationSecondsType now  = Time::GetTickCount ();
        auto                lock = fCaptureSets_.cget (); // hold lock until I've examined changeCount

        // it would be nice to be able to preserve the timing info in the runQueue for items not changed, but that would require
        // being able to tell which CaptureSets were new and old, which is not always possible given the current API (SetCaptureSet).
        // and updating the capture sets should be rare, so it should cause little problem to recompute it, and reset the counters.
        runQueue.clear ();
        SortedMapping<DurationSecondsType, CaptureSet> tmp;
        for (const auto& i : lock.load ()) {
            runQueue.Add (now + i.GetRunPeriod ().As<DurationSecondsType> (), i);
        }
        changeCountForCaptureSet = fCaptureSetChangeCount_;
    };

    // Note this runs in a threadpool which can be canceled as needed, so this need not check for a termination condition - it will be ended by a thread abort
    while (true) {
        if (changeCountForCaptureSet < fCaptureSetChangeCount_) {
            recomputeSortOrder (); // updates runQueue as a side-effect
        }

        Assert (not runQueue.empty ()); // because otherwise the thread would have been aborted, and we wouldn't get this far (race??? - maybe need to do ifcheck)

        // otehrwise pop the first item from the Q, and wait til then. Then process it, and push it back onto the Q with the appropriate
        // 'next' time.
        auto iterator = runQueue.begin ();
        Assert (iterator != runQueue.end ());
        KeyValuePair<DurationSecondsType, CaptureSet> runNext = *iterator;
        Execution::SleepUntil (runNext.fKey);
        RunnerOnce_ (runNext.fValue);
        runQueue.erase (iterator);
        runQueue.Add (runNext.fKey + runNext.fValue.GetRunPeriod ().As<DurationSecondsType> (), runNext.fValue); // interpret time offset as wrt leading edge
    }
}

void Capturer::RunnerOnce_ (const CaptureSet& cs)
{
    MeasurementSet measurements;
    for (Instrument i : cs.GetInstrumentSet ()) {
        try {
            measurements.MergeAdditions (i.Capture ());
        }
        catch (const Execution::Thread::AbortException&) {
            Execution::ReThrow ();
        }
        catch (...) {
            DbgTrace ("Eating exception in Capturer runner");
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
