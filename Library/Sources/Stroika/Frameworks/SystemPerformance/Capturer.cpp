/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/SortedAssociation.h"
#include "Stroika/Foundation/Debug/Main.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "Capturer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

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
    , captureSets{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Collection<CaptureSet> {
                      const Capturer* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::captureSets);
                      return thisObj->fCaptureSets_.load ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& captureSets) {
                      Capturer* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Capturer::captureSets);
                      auto      lock    = thisObj->fCaptureSets_.rwget ();
                      lock.store (captureSets);
                      thisObj->fCaptureSetsChanged_.Set ();
                      if (not captureSets.empty ()) {
                          thisObj->StartRunner_ (); // holding the lock, so two callers cannot both start one
                      }
                  }}
{
    Require (Debug::AppearsDuringMainLifetime ());
}

Capturer::Capturer (const CaptureSet& cs)
    : Capturer{}
{
    AddCaptureSet (cs);
}

auto Capturer::AddMeasurementsCallback (const NewMeasurementsCallbackType& cb) -> MeasurementsCallbackID
{
    return fCallbacks_.Add (cb);
}

void Capturer::RemoveMeasurementsCallback (MeasurementsCallbackID cb)
{
    fCallbacks_.Remove (cb);
}

void Capturer::AddCaptureSet (const CaptureSet& cs)
{
    RunnerOnce_ (cs);
    auto lock = fCaptureSets_.rwget ();
    lock->Add (cs);
    fCaptureSetsChanged_.Set ();
    StartRunner_ (); // holding the lock, so two callers cannot both start one
}

void Capturer::StartRunner_ ()
{
    if (fThreadPool_.GetTasksCount () == 0) {
        fThreadPool_.AddTask ([this] () { Runner_ (); });
        fThreadPool_.SetPoolSize (1);
    }
}

void Capturer::Runner_ ()
{
    // One thread runs every capture set, each on its own schedule: runQueue holds each one's next run time, soonest first - a
    // SortedAssociation, since two can be due at once.
    //
    // When the capture sets change, every schedule starts over: keeping the unchanged ones' would mean telling which sets are new,
    // which the captureSets property cannot - and changes are rare.
    SortedAssociation<TimePointSeconds, CaptureSet> runQueue;

    // runs in a threadpool, which aborts it when the Capturer goes - so no other termination condition
    while (true) {
        if (fCaptureSetsChanged_.PeekIsSet ()) {
            fCaptureSetsChanged_.Reset (); // before reading them: a change from here on sets it again
            TimePointSeconds now = Time::GetTickCount ();
            runQueue.RemoveAll ();
            for (const CaptureSet& cs : fCaptureSets_.load ()) {
                runQueue.Add (now + cs.runPeriod (), cs);
            }
        }
        if (runQueue.empty ()) {
            fCaptureSetsChanged_.Wait ();
            continue;
        }
        KeyValuePair<TimePointSeconds, CaptureSet> runNext = *runQueue.begin ();
        if (fCaptureSetsChanged_.WaitUntilQuietly (runNext.fKey) == WaitableEvent::WaitStatus::eTriggered) {
            continue; // the capture sets changed while waiting
        }
        runQueue.Remove (runQueue.begin ());
        RunnerOnce_ (runNext.fValue);
        runQueue.Add (runNext.fKey + runNext.fValue.runPeriod (), runNext.fValue); // a period after it was due, not after it finished
    }
}

void Capturer::RunnerOnce_ (const CaptureSet& cs)
{
    MeasurementSet measurements;
    for (Instrument i : cs.instruments ()) {
        try {
            measurements.MergeAdditions (i.Capture ());
        }
        catch (const Thread::AbortException&) {
            ReThrow ();
        }
        catch (...) {
            using namespace Characters::Literals;
            DbgTrace ("Eating exception '{}' in Capturer runner"_f, current_exception ());
        }
    }
    UpdateMeasurementSet_ (measurements);
}

void Capturer::UpdateMeasurementSet_ (const MeasurementSet& ms)
{
    fCurrentMeasurementSet_.rwget ().rwref ().MergeAdditions (ms);
    fCallbacks_.Call (ms);
}
