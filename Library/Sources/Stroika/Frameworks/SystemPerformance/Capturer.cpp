/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../../Foundation/Execution/Sleep.h"

#include    "Capturer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;




Capturer::Capturer ()
    : fThreadPool_ (1)
    , fCaptureSets_ ()
    , fCallbacks_ ()
    , fCurrentMeasurementSet_ ()
{
    fThreadPool_.AddTask (Execution::mkIRunnablePtr ([this] () {
        Runner_ ();
    }));
}

MeasurementSet    Capturer::GetMostRecentMeasurements () const
{
    return fCurrentMeasurementSet_;
}

Collection<Capturer::NewMeasurementsCallbackType>    Capturer::GetMeasurementsCallbacks () const
{
    return fCallbacks_;
}

void        Capturer::SetMeasurementsCallbacks (const Collection<NewMeasurementsCallbackType>& callbacks)
{
    fCallbacks_ = callbacks;
}

void        Capturer::AddMeasurementsCallback (const NewMeasurementsCallbackType& cb)
{
    fCallbacks_.Add (cb);
}

Collection<CaptureSet>   Capturer::GetCaptureSets () const
{
    return fCaptureSets_;
}

void      Capturer::SetCaptureSets (const Collection<CaptureSet>& captureSets)
{
    fCaptureSets_ = captureSets;
    Assert (fCaptureSets_.size () <= 1);    // only case we support so far
}

void        Capturer::AddCaptureSet (const CaptureSet& cs)
{
    fCaptureSets_.Add (cs);
    Assert (fCaptureSets_.size () == 1);    // only case we support so far
}

void    Capturer::Runner_ ()
{
    // SUPER MEGA OVERSIMPLIFED.
    // @todo - fix to submit each subtask as its own runnable so it can leverage threadpool
    // @todo real job waiting for jsut right time before starting next run through apturesets
    // @todo support more than one capture set.
    while (true) {
        if (fCaptureSets_.empty ()) {
            Execution::Sleep (1);   //megahack - just dont add task when empty...
        }
        //tmphack a race
        if (fCaptureSets_.size () >= 1) {
            CaptureSet cs = *fCaptureSets_.FindFirstThat ([] (CaptureSet) { return true;});
            Execution::Sleep (cs.GetRunPeriod ());  // wrong - period should be from leading edge of last run!!!
            // @todo fix!!!

            for (Instrument i : cs.GetInstrumentSet ()) {
                try {
                    MeasurementSet  measurements    =   i.Capture ();
                    // AGAIN - WRONG - we need a way to update - UpdateMeasurementSet_ - where it combines the rihgt
                    // set o fmeasuremts, repalcing the others
                    UpdateMeasurementSet_ (measurements);
                }
                catch (Execution::ThreadAbortException&) {
                    Execution::DoReThrow ();
                }
                catch (...) {
                    DbgTrace ("Eating exception in Capturer runner");
                }
            }
        }
    }
}

void    Capturer::UpdateMeasurementSet_ (const MeasurementSet& ms)
{
    fCurrentMeasurementSet_ = ms;
    for (auto cb : fCallbacks_) {
        cb (ms);
    }
}

