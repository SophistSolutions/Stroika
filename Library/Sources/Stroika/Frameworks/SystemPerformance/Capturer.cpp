/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 ******************** SystemPerformance::Capturer *******************************
 ********************************************************************************
 */
MeasurementSet Capturer::GetMostRecentMeasurements () const
{
    return fCurrentMeasurementSet_;
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
            fThreadPool_.AddTask ([this]() { Runner_ (); });
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
    // SUPER MEGA OVERSIMPLIFED.
    // @todo - fix to submit each subtask as its own runnable so it can leverage threadpool
    // @todo real job waiting for jsut right time before starting next run through apturesets
    // @todo support more than one capture set.
    while (true) {
        //tmphack a race
        if (fCaptureSets_.cget ()->size () >= 1) {
            CaptureSet cs = *fCaptureSets_.cget ()->FindFirstThat ([](CaptureSet) { return true; });

            // @todo fix!!!
            // wrong - period should be from leading edge of last run!!!
            Execution::Sleep (cs.GetRunPeriod ());

            for (Instrument i : cs.GetInstrumentSet ()) {
                try {
                    MeasurementSet measurements = i.Capture ();
                    // AGAIN - WRONG - we need a way to update - UpdateMeasurementSet_ - where it combines the rihgt
                    // set o fmeasuremts, repalcing the others
                    UpdateMeasurementSet_ (measurements);
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
                    DbgTrace ("Eating exception in Capturer runner");
                }
            }
        }
    }
}

void Capturer::UpdateMeasurementSet_ (const MeasurementSet& ms)
{
    fCurrentMeasurementSet_ = ms;
    for (auto cb : fCallbacks_.load ()) {
        cb (ms);
    }
}
