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
