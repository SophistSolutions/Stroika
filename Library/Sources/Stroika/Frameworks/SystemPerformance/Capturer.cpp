/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

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
    fCaptureSets_.rwget ()->Add (cs);
    RunnerOnce_ ();
    ManageRunner_ (true);
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
    fCaptureSets_ = captureSets;
    ManageRunner_ (not fCaptureSets_.cget ()->empty ());
    Assert (fCaptureSets_.cget ()->size () <= 1); // only case we support so far
}

void Capturer::AddCaptureSet (const CaptureSet& cs)
{
    fCaptureSets_.rwget ()->Add (cs);
    ManageRunner_ (true);
    Assert (fCaptureSets_.cget ()->size () == 1); // only case we support so far
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
    while (true) {
        // wrong - period should be from leading edge of last run!!! - and we should probably have ... - probably one task in each threadpool...
        Duration d = fCaptureSets_.load ().Select<Duration> ([] (auto i) { return i.GetRunPeriod (); }).MinValue (30s);
        Execution::Sleep (d);
        RunnerOnce_ ();
    }
}

void Capturer::RunnerOnce_ ()
{
    // SUPER MEGA OVERSIMPLIFED.
    // @todo - fix to submit each subtask as its own runnable so it can leverage threadpool
    // @todo real job waiting for jsut right time before starting next run through capturesets
    // @todo support more than one capture set.

    MeasurementSet measurements;
    for (CaptureSet cs : fCaptureSets_.load ()) {
        // @todo fix!!!

        for (Instrument i : cs.GetInstrumentSet ()) {
            try {
                auto m = i.Capture ();
                measurements.fMeasurements += m.fMeasurements;
                measurements.fMeasuredAt = m.fMeasuredAt;       // weak way to merge but hopefully good enuf for now...
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (...) {
                DbgTrace ("Eating exception in Capturer runner");
            }
        }
    }
    UpdateMeasurementSet_ (measurements);
}

void Capturer::UpdateMeasurementSet_ (const MeasurementSet& ms)
{
    fCurrentMeasurementSet_ = ms;
    for (const auto& cb : fCallbacks_.load ()) {
        cb (ms);
    }
}
