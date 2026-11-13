/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::SystemPerformance
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <atomic>
#include <iostream>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Stroika/Frameworks/SystemPerformance/Capturer.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

#if qStroika_HasComponent_googletest
namespace {
    /*
     *  A Capturer's callback that throws stops neither the other callbacks, nor the capturing: it ended the capturing thread's
     *  loop, so no measurements came after it at all.
     */
    GTEST_TEST (Frameworks_SystemPerformance, Capturer_CallbackThrows_)
    {
        Debug::TraceContextBumper ctx{"Capturer_CallbackThrows_"};
        atomic<unsigned int>      calls{0}; // before the capturer, so it outlives the capturing thread's calls
        Capturer                  capturer;
        capturer.AddMeasurementsCallback (
            [] ([[maybe_unused]] const MeasurementSet& ms) { Execution::Throw (Execution::Exception<>{"a test's callback, throwing"sv}); });
        capturer.AddMeasurementsCallback ([&] ([[maybe_unused]] const MeasurementSet& ms) { ++calls; });
        capturer.AddCaptureSet (CaptureSet{100ms}); // no instruments: just a MeasurementSet each 100ms, for the callbacks
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; calls < 3 and Time::GetTickCount () < giveUpAt;) {
            Execution::Sleep (50ms);
        }
        EXPECT_GE (calls.load (), 3u) << "the capturing stopped after a callback threw";
    }

    /*
     *  RemoveMeasurementsCallback, given the ID AddMeasurementsCallback returned: once it returns, that callback is not called
     *  again - while another goes on being called, as the capturing goes on.
     */
    GTEST_TEST (Frameworks_SystemPerformance, Capturer_RemoveMeasurementsCallback_)
    {
        Debug::TraceContextBumper ctx{"Capturer_RemoveMeasurementsCallback_"};
        atomic<unsigned int>      calls{0}; // these before the capturer, so they outlive the capturing thread's calls
        atomic<unsigned int>      othersCalls{0};
        Capturer                  capturer;
        Capturer::MeasurementsCallbackID id = capturer.AddMeasurementsCallback ([&] ([[maybe_unused]] const MeasurementSet& ms) { ++calls; });
        capturer.AddMeasurementsCallback ([&] ([[maybe_unused]] const MeasurementSet& ms) { ++othersCalls; });
        capturer.AddCaptureSet (CaptureSet{100ms}); // its first capture is made on this thread, before this returns
        EXPECT_GE (calls.load (), 1u);
        capturer.RemoveMeasurementsCallback (id);
        unsigned int callsThen       = calls;
        unsigned int othersCallsThen = othersCalls;
        Execution::Sleep (500ms); // five more captures
        EXPECT_EQ (calls.load (), callsThen) << "called after RemoveMeasurementsCallback returned";
        EXPECT_GT (othersCalls.load (), othersCallsThen) << "no capture since - so the test shows nothing";
    }
}

namespace {
    // an Instrument that only counts its captures (its copies too: a CaptureSet holds a copy)
    Instrument CountingInstrument_ (const Characters::String& name, atomic<unsigned int>& count)
    {
        struct Rep_ : Instrument::IRep {
            atomic<unsigned int>& fCount;
            Rep_ (atomic<unsigned int>& count)
                : fCount{count}
            {
            }
            virtual MeasurementSet Capture () override
            {
                ++fCount;
                MeasurementSet none; // not MeasurementSet{}: that copy-list-initializes its Range, whose default constructor is explicit
                return none;
            }
            virtual unique_ptr<IRep> Clone () const override
            {
                return make_unique<Rep_> (fCount);
            }
            virtual shared_ptr<Instrument::ICaptureContext> GetContext () const override
            {
                return nullptr;
            }
            virtual void SetContext ([[maybe_unused]] const shared_ptr<Instrument::ICaptureContext>& context) override
            {
            }
        };
        return Instrument{InstrumentNameType{name}, make_unique<Rep_> (count), {}, {}, DataExchange::ObjectVariantMapper{}};
    }
    // does count reach n within 10s
    bool Reaches_ (const atomic<unsigned int>& count, unsigned int n)
    {
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; count < n and Time::GetTickCount () < giveUpAt;) {
            Execution::Sleep (20ms);
        }
        return count >= n;
    }

    /*
     *  Capture sets due at the same time each run: the capturing thread's queue, keyed by when each is due, kept only one of them
     */
    GTEST_TEST (Frameworks_SystemPerformance, Capturer_SetsDueTogether_)
    {
        Debug::TraceContextBumper ctx{"Capturer_SetsDueTogether_"};
        atomic<unsigned int>      a{0}; // these before the capturer, so they outlive the capturing thread's calls
        atomic<unsigned int>      b{0};
        Capturer                  capturer;
        capturer.AddCaptureSet (CaptureSet{100ms, {CountingInstrument_ ("a"sv, a)}});
        capturer.AddCaptureSet (CaptureSet{100ms, {CountingInstrument_ ("b"sv, b)}});
        EXPECT_TRUE (Reaches_ (a, 3) and Reaches_ (b, 3)) << "a: " << a.load () << ", b: " << b.load ();
    }

    /*
     *  A capture set added while the capturing thread waits for another, due much later, runs on its own schedule: it waited
     *  for the other
     */
    GTEST_TEST (Frameworks_SystemPerformance, Capturer_AddedWhileWaiting_)
    {
        Debug::TraceContextBumper ctx{"Capturer_AddedWhileWaiting_"};
        atomic<unsigned int>      a{0}; // these before the capturer, so they outlive the capturing thread's calls
        atomic<unsigned int>      b{0};
        Capturer                  capturer;
        capturer.AddCaptureSet (CaptureSet{1h, {CountingInstrument_ ("a"sv, a)}});
        Execution::Sleep (100ms); // so the capturing thread is waiting for it
        capturer.AddCaptureSet (CaptureSet{100ms, {CountingInstrument_ ("b"sv, b)}});
        EXPECT_TRUE (Reaches_ (b, 3)) << "b: " << b.load ();
    }

    /*
     *  Setting captureSets starts capturing them, and setting none stops it: the capturing thread never saw a set
     */
    GTEST_TEST (Frameworks_SystemPerformance, Capturer_SetCaptureSets_)
    {
        Debug::TraceContextBumper ctx{"Capturer_SetCaptureSets_"};
        atomic<unsigned int>      a{0}; // before the capturer, so it outlives the capturing thread's calls
        Capturer                  capturer;
        capturer.captureSets = Containers::Collection<CaptureSet>{CaptureSet{100ms, {CountingInstrument_ ("a"sv, a)}}};
        EXPECT_TRUE (Reaches_ (a, 3)) << "a: " << a.load ();
        capturer.captureSets = Containers::Collection<CaptureSet>{};
        Execution::Sleep (50ms); // for a capture under way as it was set
        unsigned int aThen = a;
        Execution::Sleep (500ms); // five periods
        EXPECT_EQ (a.load (), aThen) << "still capturing a set no longer there";
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
