/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_TestHarness_h_
#define _Stroika_TestHarness_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#if qStroika_HasComponent_googletest
#define GTEST_DONT_DEFINE_TEST 1
#if defined(__clang_major__) and (20 < __clang_major__)
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wcharacter-conversion\"") // gtest-printers.h:528 PrintTo(ImplicitCast_<char32_t>(c), os); mixes char8_t/char32_t - not our code to fix
#endif
#include <gtest/gtest.h>
#if defined(__clang_major__) and (20 < __clang_major__)
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wcharacter-conversion\"")
#endif
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Time/Realtime.h"

namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Frameworks::Test {

    /**
     *  \par Example Usage
     *      \code
     *          int main (int argc, const char* argv[])
     *          {
     *              Test::Setup (argc, argv);
     *              return Test::PrintPassOrFail (DoRegressionTests_);
     *          }
     *      \endcode
     * 
     *  if qStroika_HasComponent_googletest is enabled, this calls testing::InitGoogleTest(), which may remove some values from argc/argv.
     *  Use the value returned from Setup() to get the adjusted args list. Note - no character-set mapping is done in this routine. Just copying of the same
     *  starting raw data.
     */
    vector<string> Setup (int argc, const char* argv[]);

    /**
     * print succeeded if it completes, and failed if exception caught
     *     \see example usage in Setup()
     */
    [[deprecated ("Since Strokka v3.0d5 - just use gtest")]] int PrintPassOrFail (void (*regressionTest) ());

    namespace Private_ {
        /**
         * LIKE calling Assert but it will ALSO trigger a failure in NODEBUG builds
         */
        void Test_ (bool failIfFalse, bool isFailureElseWarning, const wchar_t* regressionTestText, const wchar_t* fileName, int lineNum);
        void VerifyTestResultWarning_ (bool failIfFalse, bool isFailureElseWarning, const wchar_t* regressionTestText,
                                       const wchar_t* fileName, int lineNum);
    }

    /**
     */
#define VerifyTestResult(c)                                                                                                                \
    Stroika::Frameworks::Test::Private_::Test_ (!!(c), true, Stroika_Foundation_Debug_Widen (#c), Stroika_Foundation_Debug_Widen (__FILE__), __LINE__)

    /**
     */
#define VerifyTestResultWarning(c)                                                                                                         \
    Stroika::Frameworks::Test::Private_::VerifyTestResultWarning_ (!!(c), false, Stroika_Foundation_Debug_Widen (#c),                      \
                                                                   Stroika_Foundation_Debug_Widen (__FILE__), __LINE__)

    /**
     *  Unlike most Stroika APIs, here 'char' refers to 'narrow SDK-char' codepage.
     */
    void WarnTestIssue (const char* issue);
    void WarnTestIssue (const wchar_t* issue);
    void WarnTestIssue (const Foundation::Characters::String& issue);

#if qStroika_HasComponent_googletest
    /**
     *  \brief Report that PART of the current test cannot run here, and why - so gtest reports the test SKIPPED, not PASSED
     *
     *  A check that silently does not run looks just like one that passed. To skip a whole test, use GTEST_SKIP () << reason,
     *  which returns. To skip just part of one, use this: it does NOT return, so the rest of the test still runs. Either way the
     *  test is reported SKIPPED, with the file:line and reason, and counted in gtest's '[  SKIPPED ] N tests' summary - and it
     *  is still reported FAILED if any of its checks fail.
     *
     *  \note Keep the reason clear of the words "failed", "error:" and "warning": Build/Scripts/RegressionTests counts every
     *        log line containing them. Put variable detail, such as an exception's text, in a WarnTestIssue () instead.
     *
     *  \par Example Usage
     *      \code
     *          #if qStroika_HasComponent_OpenSSL
     *              ... checks needing OpenSSL ...
     *          #else
     *              SkipTestPart ("built without OpenSSL");
     *          #endif
     *      \endcode
     */
#define SkipTestPart(reason) [&] { GTEST_SKIP () << (reason); }()
#endif

    /**
     *  Samples a steady clock on its own thread for this object's lifetime, and reports the largest
     *  gap it saw between consecutive samples.
     *
     *  A gap far larger than the sampling interval means the PROCESS did not run for that long - so
     *  any timing measured across it says nothing about the code under test. Use it to tell "this
     *  wait misbehaved" apart from "this machine stopped for half a minute", which look identical
     *  from a single elapsed-time number. CI runners do the latter routinely, ~30s at a stretch.
     *
     *  Deliberately std::thread, not Execution::Thread: this is used to judge Stroika's own
     *  thread/wait machinery, so it must not depend on it.
     *
     *  
ote It cannot say WHY the time was lost - VM paused, host oversubscribed, process
     *        descheduled all read the same. For deciding whether a measurement is trustworthy that
     *        distinction does not matter.
     *
     *  \par Example Usage
     *      \code
     *          ClockContinuitySampler clockCheck;
     *          ... time something ...
     *          if (clockCheck.GetMaxObservedGap () > 2s) {  // host stalled; measurement is junk
     *      \endcode
     */
    class ClockContinuitySampler {
    public:
        ClockContinuitySampler (Foundation::Time::DurationSeconds sampleEvery = Foundation::Time::DurationSeconds{0.05});
        ClockContinuitySampler (const ClockContinuitySampler&) = delete;
        ~ClockContinuitySampler ();
        ClockContinuitySampler& operator= (const ClockContinuitySampler&) = delete;

    public:
        /**
         *  Largest gap between consecutive samples so far. Safe to call while still running.
         */
        nonvirtual Foundation::Time::DurationSeconds GetMaxObservedGap () const;

    private:
        atomic<bool>   fDone_{false};
        atomic<double> fMaxGapSeconds_{0};
        thread         fSampler_;
    };

}
#endif /*_Stroika_TestHarness_h_*/
