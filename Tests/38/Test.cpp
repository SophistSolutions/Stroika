/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
//  TEST    Foundation::Execution::Threads
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Execution/BlockingQueue.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;

using Characters::String;
using Containers::Sequence;
using Execution::BlockingQueue;
using Execution::Finally;
using Execution::RWSynchronized;
#if __has_include(<boost/thread/shared_mutex.hpp>)
using Execution::UpgradableRWSynchronized;
#endif
using Execution::SpinLock;
using Execution::Synchronized;
using Execution::Thread;
using Execution::ThreadPool;
using Execution::WaitableEvent;

namespace {
    void RegressionTest1_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest1_");
        struct FRED {
            static void DoIt ([[maybe_unused]] void* ignored)
            {
                for (int i = 1; i < 10; i++) {
                    Execution::Sleep (1ms);
                }
            }
        };

        Thread::Ptr thread = Thread::New (bind (&FRED::DoIt, const_cast<char*> ("foo")));
        thread.Start ();
        thread.WaitForDone ();
    }
}

namespace {
    recursive_mutex sharedCriticalSection_;
    void            RegressionTest2_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest2_");

        // Make 2 concurrent threads, which share a critical section object to take turns updating a variable
        struct FRED {
            static void DoIt (void* ignored)
            {
                int* argP = reinterpret_cast<int*> (ignored);
                for (int i = 0; i < 10; i++) {
                    lock_guard<recursive_mutex> critSect (sharedCriticalSection_);
                    int                         tmp = *argP;
                    Execution::Sleep (.001);
                    //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                    *argP = tmp + 1;
                }
            }
        };

        int         updaterValue = 0;
        Thread::Ptr thread1      = Thread::New (bind (&FRED::DoIt, &updaterValue));
        Thread::Ptr thread2      = Thread::New (bind (&FRED::DoIt, &updaterValue));
        thread1.Start ();
        thread2.Start ();
        thread1.WaitForDone ();
        thread2.WaitForDone ();
        VerifyTestResult (updaterValue == 2 * 10);
    }
}

namespace {
    WaitableEvent sRegTest3Event_T1_{WaitableEvent::eAutoReset};
    WaitableEvent sRegTest3Event_T2_{WaitableEvent::eAutoReset};
    namespace WAITABLE_EVENTS_ {
        void NOTIMEOUTS_ ()
        {
            Debug::TraceContextBumper traceCtx ("pingpong threads with event.wait(NOTIMEOUTS)");
            // Make 2 concurrent threads, which share 2 events to synchonize taking turns updating a variable
            struct FRED1 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T1_.Wait ();
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        // Since fred1/fred2 always take turns, and Fred1 always goes first...
                        VerifyTestResult (tmp % 2 == 0);
                        //DbgTrace ("FRED1: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T2_.Set ();
                    }
                }
            };
            struct FRED2 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T2_.Wait ();
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        //DbgTrace ("FRED2: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T1_.Set ();
                    }
                }
            };

            sRegTest3Event_T1_.Reset ();
            sRegTest3Event_T2_.Reset ();
            int         updaterValue = 0;
            Thread::Ptr thread1      = Thread::New (bind (&FRED1::DoIt, &updaterValue));
            Thread::Ptr thread2      = Thread::New (bind (&FRED2::DoIt, &updaterValue));
            thread1.Start ();
            thread2.Start ();
            // Both threads start out waiting - until we get things rolling telling one to start.
            // Then they pingpong back and forther
            sRegTest3Event_T1_.Set ();
            thread1.WaitForDone ();
            thread2.WaitForDone ();
            //DbgTrace ("Test3 - updaterValue = %d", updaterValue);
            // If there was a race - its unlikely you'd end up with exact 20 as your result
            VerifyTestResult (updaterValue == 2 * 10);
        }
        void PingBackAndForthWithSimpleTimeouts_ ()
        {
            Debug::TraceContextBumper traceCtx ("pingpong threads with event.wait(WITHTIMEOUT)");
            // Make 2 concurrent threads, which share 2 events to synchonize taking turns updating a variable
            struct FRED1 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T1_.Wait (5.0);
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        // Since fred1/fred2 always take turns, and Fred1 always goes first...
                        VerifyTestResult (tmp % 2 == 0);
                        //DbgTrace ("FRED1: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T2_.Set ();
                    }
                }
            };
            struct FRED2 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T2_.Wait (5.0);
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        //DbgTrace ("FRED2: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T1_.Set ();
                    }
                }
            };

            sRegTest3Event_T1_.Reset ();
            sRegTest3Event_T2_.Reset ();
            int         updaterValue = 0;
            Thread::Ptr thread1      = Thread::New (bind (&FRED1::DoIt, &updaterValue));
            Thread::Ptr thread2      = Thread::New (bind (&FRED2::DoIt, &updaterValue));
            thread1.Start ();
            thread2.Start ();
            // Both threads start out waiting - until we get things rolling telling one to start.
            // Then they pingpong back and forther
            sRegTest3Event_T1_.Set ();
            thread1.WaitForDone ();
            thread2.WaitForDone ();
            //DbgTrace ("Test3 - updaterValue = %d", updaterValue);
            // If there was a race - its unlikely you'd end up with exact 20 as your result
            VerifyTestResult (updaterValue == 2 * 10);
        }
        void TEST_TIMEOUT_EXECPETIONS_ ()
        {
            Debug::TraceContextBumper traceCtx ("Event wait timeouts");
            bool                      passed = false;
            sRegTest3Event_T1_.Reset ();
            try {
                sRegTest3Event_T1_.Wait (0.5); // should timeout
            }
            catch (const Execution::TimeOutException&) {
                passed = true;
            }
            catch (...) {
            }
            VerifyTestResult (passed);
        }
        void TEST_ThreadCancelationOnAThreadWhichIsWaitingOnAnEvent_ ()
        {
            Debug::TraceContextBumper traceCtx{"Deadlock block on waitable event and abort thread (thread cancelation)"};
            // Make a thread to wait a 'LONG TIME' on a single event, and verify it gets cancelled reasonably
            static constexpr Time::DurationSecondsType kLONGTimeForThread2Wait_{60.0}; // just has to be much more than the waits below
            static WaitableEvent                       s_autoResetEvent_{WaitableEvent::eAutoReset};
            auto                                       myWaitingThreadProc = [] () {
                Debug::TraceContextBumper innerThreadLoopCtx{"innerThreadLoop"};
                s_autoResetEvent_.Wait (kLONGTimeForThread2Wait_);
            };

            s_autoResetEvent_.Reset ();
            Thread::Ptr t = Thread::New (myWaitingThreadProc, Thread::eAutoStart);

            // At this point the thread 't' SHOULD block and wait kLONGTimeForThread2Wait_ seconds
            // So we wait a shorter time for it, and that should fail
            {
                Debug::TraceContextBumper           ctx1{L"expect-failed-wait"};
                constexpr Time::DurationSecondsType kMarginOfErrorLo_       = .5;
                constexpr Time::DurationSecondsType kMarginOfErrorHi_Warn_  = 5.0;  // if sys busy, thread could be put to sleep almost any amount of time
                constexpr Time::DurationSecondsType kMarginOfErrorHi_Error_ = 10.0; // ""
                constexpr Time::DurationSecondsType kWaitOnAbortFor         = 1.0;
                Time::DurationSecondsType           startTestAt             = Time::GetTickCount ();
                Time::DurationSecondsType           caughtExceptAt          = 0;

                try {
                    t.WaitForDone (kWaitOnAbortFor);
                }
                catch (const Execution::TimeOutException&) {
                    caughtExceptAt = Time::GetTickCount ();
                }
                Time::DurationSecondsType expectedEndAt = startTestAt + kWaitOnAbortFor;
                if (not(expectedEndAt - kMarginOfErrorLo_ <= caughtExceptAt and caughtExceptAt <= expectedEndAt + kMarginOfErrorHi_Warn_)) {
                    DbgTrace (L"expectedEndAt=%f, caughtExceptAt=%f", double (expectedEndAt), double (caughtExceptAt));
                }
                VerifyTestResult (expectedEndAt - kMarginOfErrorLo_ <= caughtExceptAt);
                // FAILURE:
                //      2.0a208x release - in regtests on raspberrypi-gcc-5, regtests  - (caughtExceptAt - expectedEndAt) was 4.1,
                //      so may need to be much larger occasionally (on slow raspberry pi) - but rarely fails.
                //      But failed with kMarginOfErrorHi_=2.0, so from 2.5 5.0 for future releases
                //
                // FAILURE:
                //      2.0a222x release (raspberrypi-gcc-6 config (a debug build), failed with (caughtExceptAt - expectedEndAt) about 5.1
                //      so set kMarginOfErrorHi_ to 6.0 -- LGP 2017-11-13
                //
                // FAILURE:
                //      2.1d18x release (raspberrypi-g++-8-debug-sanitize_undefined config (a debug/sanitize build), failed with (caughtExceptAt - expectedEndAt) about 6.2
                //      so set kMarginOfErrorHi_ to 7.0 -- LGP 2019-02-27
                //
                // Got another failure 2019-04-17 on raspberrypi - so change limit of kMarginOfErrorHi_==7, to kMarginOfErrorHi_Warn_ = 5.0, kMarginOfErrorHi_Error_ = 10.0
                // and use VerifyTestResultWarning -- LGP 2019-04-17
                //
                // Got another warning 2019-08-12 on raspberrypi - but no change cuz about to upgrade to faster raspberrypi
                //
                VerifyTestResult (caughtExceptAt <= expectedEndAt + kMarginOfErrorHi_Error_);
                VerifyTestResultWarning (caughtExceptAt <= expectedEndAt + kMarginOfErrorHi_Warn_);
            }

            // Now ABORT and WAITFORDONE - that should kill it nearly immediately
            {
                Debug::TraceContextBumper           ctx1{L"expect-abort-to-work-and-wait-to-succceed"};
                constexpr Time::DurationSecondsType kMarginOfError_ = 10;  // larger margin of error cuz sometimes fails on raspberrypi (esp with asan)
                constexpr Time::DurationSecondsType kWaitOnAbortFor = 3.0; // use such a long timeout cuz we run this on 'debug' builds,
                                                                           // with asan, valgrind, and on small arm devices. Upped from 2.0 to 2.5 seconds
                                                                           // due to timeout on raspberrypi (rare even there)
                                                                           //
                                                                           // Upped from 2.5 to 3.0 because failed twice between July and August 2017 on
                                                                           // raspberrypi -- LGP 2017-08-23
                Time::DurationSecondsType startTestAt = Time::GetTickCount ();
                try {
                    t.AbortAndWaitForDone (kWaitOnAbortFor);
                }
                catch (const Execution::TimeOutException&) {
                    VerifyTestResult (false); // shouldn't fail to wait cuz we did abort
                    // Note - saw this fail once on raspberry pi but appears the machine was just being slow - nothing looked other than slow - wrong in
                    // the tracelog - so don't worry unless we see again. That machine can be quite slow
                    //  -- LGP 2017-07-05
                    //
                    //  NOTE - saw this a second time on raspberrypi - with:
                    //      "This should ALMOST NEVER happen - where we did an abort but it came BEFORE the system call and so needs to be called again to re-interrupt: tries: 1." in the log
                    //      about 6 seconds into wait
                    //      SIGNAL to abort other thread set at 1.6 TICKCOUNT, and it got it and started aborting at 2.1
                    //      and 0x762ff450][0007.556]       In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (InterruptException) for thread: {id: 0x762ff450, status: Aborting}
                    // and this failed at 0007.583, so just wait a little longer
                    // CHANGE NEXT RELEASE A BIT MORE
                    //
                    /// @todo CHANGED TIME FROM 2.5 to 3 2017-08-23 - so if we see again react, and if not, clear out these warnings/docs
                    //
                    //  @todo SAW AGAIN - 2017-10-07
                    //      [---MAIN---][0000.628]          <Thread::WaitForDoneUntil (*this={id: 0x762ff450, status: Running}, timeoutAt=1.627011e+00)> {
                    //      [---MAIN---][0001.647]              Throwing exception: Timeout Expired from /tmp/Test38(Stroika::Foundation::Debug::BackTrace[abi:cxx11](unsigned int)+0x51) [0x4e9f7e]; /tmp/Test38(Stroika::Foundation::Execution::Private_::GetBT_ws[abi:cxx11]()+0x23) [0x4f0558]; /tmp/Test38(Stroika::Foundation::Execution::Private_::GetBT_s[abi:cxx11]()+0x1d) [0x4f0496]; /tmp/Test38(void Stroika::Foundation::Execution::Throw<Stroika::Foundation::Execution::TimeOutException>(Stroika::Foundation::Execution::TimeOutException const&)+0x3d) [0x47449e]; /tmp/Test38(Stroika::Foundation::Execution::Thread::Ptr::WaitForDoneUntil(double) const+0x91) [0x5352c6]; /tmp/Test38(Stroika::Foundation::Execution::Thread::Ptr::WaitForDone(double) const+0x3f) [0x46eca8]; /tmp/Test38() [0x45f152]; /tmp/Test38() [0x45f500]; /tmp/Test38() [0x463464]; /tmp/Test38(Stroika::TestHarness::PrintPassOrFail(void (*)())+0x19) [0x4a7442]; /tmp/Test38(main+0x25) [0x4634fe]; /lib/arm-linux-gnueabihf/libc.so.6(__libc_start_main+0x114) [0x76cca678];
                    //      [---MAIN---][0001.648]          } </Thread::WaitForDoneUntil>
                    //      [---MAIN---][0001.649]          <Thread::AbortAndWaitForDoneUntil (*this={id: 0x762ff450, status: Running}, timeoutAt=4.647912e+00)> {
                    //      [---MAIN---][0001.649]              <Thread::Abort (*this={id: 0x762ff450, status: Running})> {
                    //      [---MAIN---][0001.651]                  <Stroika::Foundation::Execution::SignalHandlerRegistry::{}::SetSignalHandlers (signal: SIGUSR2, handlers: [ {type: Direct, target: 0xfa8e50} ])/>
                    //      [---MAIN---][0001.653]                  <Stroika::Foundation::Execution::Signals::Execution::SendSignal (target = 0x762ff450, signal = SIGUSR2)/>
                    //      [---MAIN---][0001.653]              } </Thread::Abort>
                    //      [0x762ff450][0002.155]      Throwing exception: Thread Abort from /tmp/Test38(Stroika::Foundation::Debug::BackTrace[abi:cxx11](unsigned int)+0x51) [0x4e9f7e]; /tmp/Test38(Stroika::Foundation::Execution::Private_::GetBT_ws[abi:cxx11]()+0x23) [0x4f0558]; /tmp/Test38(Stroika::Foundation::Execution::Private_::GetBT_s[abi:cxx11]()+0x1d) [0x4f0496]; /tmp/Test38(void Stroika::Foundation::Execution::Throw<Stroika::Foundation::Execution::Thread::AbortException>(Stroika::Foundation::Execution::Thread::AbortException const&)+0x3d) [0x53b9ee]; /tmp/Test38(Stroika::Foundation::Execution::CheckForThreadInterruption()+0x91) [0x535eee]; /tmp/Test38(Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)+0x5d) [0x57297a]; /tmp/Test38(Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntil(double)+0x21) [0x5728ba]; /tmp/Test38(Stroika::Foundation::Execution::WaitableEvent::Wait(double)+0x33) [0x46f198]; /tmp/Test38() [0x45f01c]; /tmp/Test38() [0x466066]; /tmp/Test38(std::function<void ()>::operator()() c
                    //      onst+0x2f) [0x4726e0]; /tmp/Test38(void Stroika::Foundation::Execution::Function<void ()>::operator()<>() const+0x5b) [0x53a16c]; /tmp/Test38(Stroika::Foundation::Execution::Thread::Rep_::Run_()+0x1b) [0x53308c]; /tmp/Test38(Stroika::Foundation::Execution::Thread::Rep_::ThreadMain_(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_> const*)+0x3cd) [0x533da6];
                    //      [---MAIN---][0006.378]              This should ALMOST NEVER happen - where we did an abort but it came BEFORE the system call and so needs to be called again to re-interrupt: tries: 1.
                    //      [---MAIN---][0006.379]              <Thread::Abort (*this={id: 0x762ff450, status: Aborting})> {
                    //      [0x762ff450][0006.379]      In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (InterruptException) for thread: {id: 0x762ff450, status: Aborting}
                    //      [---MAIN---][0006.379]                  <Stroika::Foundation::Execution::Signals::Execution::SendSignal (target = 0x762ff450, signal = SIGUSR2)/>
                    //      [---MAIN---][0006.380]              } </Thread::Abort>
                    //      [0x762ff450][0006.380]      removing thread id 0x762ff450 from sRunningThreads_ ([ 0x762ff450 ])
                    //
                    //  So issue is, why the delay from [0002.155] to [0006.378]..."This should ALMOST NEVER happen..."
                    //
                    // @todo SAW AGAIN (but only when running with ASAN)
                    //      [---MAIN---][0001.659]          <expect-abort-to-work-and-wait-to-succceed> {
                    //      [---MAIN---][0001.660]              <Thread::AbortAndWaitForDoneUntil (*this={id: 0x763ff450, status: Running}, timeoutAt=4.659056e+00)> {
                    //      [---MAIN---][0001.661]                  <Thread::Abort (*this={id: 0x763ff450, status: Running})> {
                    //      [---MAIN---][0001.663]                      <Stroika::Foundation::Execution::SignalHandlerRegistry::{}::SetSignalHandlers (signal: SIGUSR2, handlers: [ {type: Direct, target: 0x57112c60} ])/>
                    //      [---MAIN---][0001.665]                      <Stroika::Foundation::Execution::Signals::Execution::SendSignal (target = 0x763ff450, signal = SIGUSR2)/>
                    //      [---MAIN---][0001.665]                  } </Thread::Abort>
                    //      [---MAIN---][0002.666]                  This should ALMOST NEVER happen - where we did an abort but it came BEFORE the system call and so needs to be called again to re-interrupt: tries: 1.
                    //      [0x763ff450][0010.359]          Throwing exception: Thread Abort from /tmp/Test38(Stroika::Foundation::Debug::BackTrace[abi:cxx11](unsigned int)+0x51) [0x54fef58a]; /tmp/Test38(Stroika::Foundation::Execution::Private_::GetBT_ws[abi:cxx11]()+0x23) [0x54ff5f80]; /tmp/Test38(Stroika::Foundation::Execution::Private_::GetBT_s[abi:cxx11]()+0x1d) [0x54ff5eba]; /tmp/Test38(void Stroika::Foundation::Execution::Throw<Stroika::Foundation::Execution::Thread::AbortException>(Stroika::Foundation::Execution::Thread::AbortException const&)+0x3d) [0x55043ebe]; /tmp/Test38(Stroika::Foundation::Execution::CheckForThreadInterruption()+0x8b) [0x5503ed60]; /tmp/Test38(Stroika::Foundation::Execution::ConditionVariable<std::mutex, std::condition_variable>::wait_until(std::unique_lock<std::mutex>&, double)+0x43) [0x54f7503c]; /tmp/Test38(+0x535260) [0x5507e260]; /tmp/Test38(Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)+0x41) [0x5507e012]; /tmp/Test38(Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUnti
                    //      l(double)+0x21) [0x5507df2a]; /tmp/Test38(Stroika::Foundation::Execution::WaitableEvent::Wait(double)+0x33) [0x54f6ce30]; /tmp/Test38(+0x41158e) [0x54f5a58e]; /tmp/Test38(+0x4195ae) [0x54f625ae]; /tmp/Test38(std::function<void ()>::operator()() const+0x2f) [0x54f70578]; /tmp/Test38(void Stroika::Foundation::Execution::Function<void ()>::operator()<>() const+0x4b) [0x550425f0]; /tmp/Test38(Stroika::Foundation::Execution::Thread::Rep_::Run_()+0x1b) [0x5503bd58]; /tmp/Test38(Stroika::Foundation::Execution::Thread::Rep_::ThreadMain_(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_> const*)+0x37d) [0x5503ca32];
                    //      [0x763ff450][0010.359]      } </innerThreadLoop>
                    //      [---MAIN---][0010.359]                  <Thread::Abort (*this={id: 0x763ff450, status: Aborting})> {
                    //      [---MAIN---][0010.360]                      <Stroika::Foundation::Execution::Signals::Execution::SendSignal (target = 0x763ff450, signal = SIGUSR2)> {
                    //      [0x763ff450][0010.361]      In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (InterruptException) for thread: {id: 0x763ff450, status: Aborting}
                    //      [---MAIN---][0010.361]                      } </Stroika::Foundation::Execution::Signals::Execution::SendSignal>
                    //      [---MAIN---][0010.361]                  } </Thread::Abort>
                    //      [0x763ff450][0010.361]      removing thread id 0x763ff450 from sRunningThreads_ ([ 0x763ff450 ])
                    //      [0x763ff450][0010.362]  } </Thread::Rep_::ThreadMain_>
                    //      [---MAIN---][0010.362]                  <Thread::WaitForDoneUntil (*this={id: 0x763ff450, status: Completed}, timeoutAt=4.659056e+00)/>
                    //      [---MAIN---][0010.363]              } </Thread::AbortAndWaitForDoneUntil>
                    //      [---MAIN---][0010.363]              startTestAt=1.659040, doneAt=10.362815, expectedEndAt=4.659040
                    //      [---MAIN---][0010.363]              FAILED: RegressionTestFailure; startTestAt <= doneAt and doneAt <= expectedEndAt + kMarginOfError_; ; Test.cpp; 311
                    //  So upped kMarginOfError_ from 3.5 to 10 seconds.
                    //
                    // Very rare - but saw this again on Raspberrypi - raspberrypi-g++-8-debug-sanitize_undefined- 2.1a2x -- LGP 2019-10-11
                }
                Time::DurationSecondsType doneAt        = Time::GetTickCount ();
                Time::DurationSecondsType expectedEndAt = startTestAt + kWaitOnAbortFor;
                if (not(startTestAt <= doneAt and doneAt <= expectedEndAt + kMarginOfError_)) {
                    DbgTrace (L"startTestAt=%f, doneAt=%f, expectedEndAt=%f", double (startTestAt), double (doneAt), double (expectedEndAt));
                }
                VerifyTestResult (startTestAt <= doneAt and doneAt <= expectedEndAt + kMarginOfError_);
            }

            // Thread MUST be done/terminated by this point
            VerifyTestResult (t.GetStatus () == Thread::Status::eCompleted);
        }
    }
    void RegressionTest3_WaitableEvents_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest3_WaitableEvents_");
        WAITABLE_EVENTS_::NOTIMEOUTS_ ();
        WAITABLE_EVENTS_::PingBackAndForthWithSimpleTimeouts_ ();
        WAITABLE_EVENTS_::TEST_TIMEOUT_EXECPETIONS_ ();
        WAITABLE_EVENTS_::TEST_ThreadCancelationOnAThreadWhichIsWaitingOnAnEvent_ ();
    }
}

namespace {
    namespace RegressionTest4_Synchronized_ {
        namespace Private_ {
            struct data_ {
            };
            void Test1_ ()
            {
                using namespace Execution;
                using syncofdata = Synchronized<data_, Synchronized_Traits<recursive_mutex>>;
                using syncofint  = Synchronized<int, Synchronized_Traits<recursive_mutex>>;

                Debug::TraceContextBumper traceCtx ("Test1_");
                {
                    syncofdata                  x;
                    [[maybe_unused]] syncofdata y = data_ ();
                    x                             = data_ ();
                }
                {
                    syncofint                  x;
                    [[maybe_unused]] syncofint y = 3;
                    x                            = 4;
                }
                {
                    // Make 2 concurrent threads, which update a lynchronized variable
                    struct FRED {
                        static void DoIt (void* ignored)
                        {
                            syncofint* argP = reinterpret_cast<syncofint*> (ignored);
                            for (int i = 0; i < 10; i++) {
                                syncofint::WritableReference r   = argP->rwget ();
                                int                          tmp = r;
                                Execution::Sleep (10ms);
                                //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                                r = tmp + 1;
#if 0
                                lock_guard<recursive_mutex> critSect (*argP);
                                int tmp = *argP;
                                Execution::Sleep (.01);
                                //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                                *argP = tmp + 1;
#endif
                            }
                        }
                    };
                    syncofint   updaterValue = 0;
                    Thread::Ptr thread1      = Thread::New (bind (&FRED::DoIt, &updaterValue));
                    Thread::Ptr thread2      = Thread::New (bind (&FRED::DoIt, &updaterValue));
                    thread1.Start ();
                    thread2.Start ();
                    thread1.WaitForDone ();
                    thread2.WaitForDone ();
                    VerifyTestResult (updaterValue == 2 * 10);
                }
            }
            void Test2_LongWritesBlock_ ()
            {
                Debug::TraceContextBumper                  ctx{"Test2_LongWritesBlock_"};
                static constexpr int                       kBaseRepititionCount_ = 100;
                static constexpr Time::DurationSecondsType kBaseSleepTime_       = 0.001;
                Synchronized<int>                          syncData{0};
                atomic<bool>                               writerDone{false};
                Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic (writerDone);
                atomic<unsigned int> readsDoneAfterWriterDone{0};
                Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic (readsDoneAfterWriterDone);
                Thread::Ptr readerThread = Thread::New ([&] () {
                    Debug::TraceContextBumper ctx{"readerThread"};
                    // Do 10x more reads than writer loop, but sleep 1/10th as long
                    for (int i = 0; i < kBaseRepititionCount_ * 10; ++i) {
                        if (writerDone) {
                            readsDoneAfterWriterDone++;
                        }
                        VerifyTestResult (syncData.cget ().load () % 2 == 0);
                        Execution::Sleep (kBaseSleepTime_ / 10.0); // hold the lock kBaseSleepTime_ / 10.0 (note - on ubuntu 1804 and fast host, inside vm, median sleep time here is really about 2ms despite division - LGP 2018-06-20)
                    }
                });
                Thread::Ptr writerThread = Thread::New ([&] () {
                    Debug::TraceContextBumper ctx{"writerThread"};
                    for (int i = 0; i < kBaseRepititionCount_; ++i) {
                        auto rwLock = syncData.rwget ();
                        rwLock.store (rwLock.load () + 1);  // set to a value that will cause reader thread to fail
                        Execution::Sleep (kBaseSleepTime_); // hold the lock kBaseSleepTime_
                        VerifyTestResult (rwLock.load () % 2 == 1);
                        rwLock.store (rwLock.load () + 1); // set to a safe value
                    }
                    VerifyTestResult (syncData.cget ().load () == kBaseRepititionCount_ * 2);
                    writerDone = true;
                });
                Thread::Start ({readerThread, writerThread});
                Thread::WaitForDone ({readerThread, writerThread});
                DbgTrace ("readsDoneAfterWriterDone = %d", readsDoneAfterWriterDone.load ()); // make sure we do some reads during writes - scheduling doesn't guarnatee
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest4_Synchronized_"};
            Private_::Test1_ ();
            Private_::Test2_LongWritesBlock_ ();
        }
    }
}

namespace {
    void RegressionTest5_Aborting_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest5_Aborting_");
        {
            struct FRED {
                static void DoIt ()
                {
                    while (true) {
                        Execution::CheckForThreadInterruption ();
                    }
                }
            };
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
            try {
                thread.WaitForDone (0.3); // should timeout
                VerifyTestResult (false);
            }
            catch (const Execution::TimeOutException&) {
                // GOOD
            }
            catch (...) {
                VerifyTestResult (false);
            }
            // Now - abort it, and wait
            thread.AbortAndWaitForDone ();
        }
#if qPlatform_Windows
        {
            Thread::Ptr thread = Thread::New ([] () {
                while (true) {
                    // test alertable sleep
                    Execution::Sleep (10000);
                }
            });
            thread.Start ();
            try {
                thread.WaitForDone (0.3); // should timeout
                VerifyTestResult (false);
            }
            catch (const Execution::TimeOutException&) {
                // GOOD
            }
            catch (...) {
                VerifyTestResult (false);
            }
            // Now - abort it, and wait
            thread.AbortAndWaitForDone ();
        }
#endif
    }
}

namespace {
    void RegressionTest6_ThreadWaiting_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest6_ThreadWaiting_");
#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
        // if this triggers - add waits to end of procedure - so we assure no 'side effects' moving on to next test...
        [[maybe_unused]] auto&& cleanupReport = Finally (
            [] () {
                again:
                    auto runningThreads = Execution::Thread::GetStatistics ().fRunningThreads;
                    DbgTrace (L"Total Running threads at end: %d", runningThreads.size ());
                    for (Execution::Thread::IDType threadID : runningThreads) {
                        DbgTrace (L"Exiting main with thread %s running", Characters::ToString (threadID).c_str ());
                    }
                    if (not runningThreads.empty ()) {
                        Execution::Sleep (1.0);
                        DbgTrace ("trying again...");
                        goto again;
                    }
                    VerifyTestResult (runningThreads.size () == 0);
            });
#endif
        struct FRED {
            static void DoIt ()
            {
                Execution::Sleep (0.01);
            }
        };

        // Normal usage
        {
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
            thread.WaitForDone ();
        }

        // OK to never wait
        for (int i = 0; i < 100; ++i) {
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
        }

        // OK to wait and wait
        {
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
            thread.WaitForDone ();
            thread.WaitForDone (1.0); // doesn't matter how long cuz its already DONE
            thread.WaitForDone ();
            thread.WaitForDone ();
        }
    }
}

namespace {
    void RegressionTest7_SimpleThreadPool_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest7_SimpleThreadPool_");
        {
            ThreadPool p;
            p.SetPoolSize (1);
        }
        {
            ThreadPool p;
            p.SetPoolSize (1);
            int                  intVal = 3;
            ThreadPool::TaskType task{
                [&intVal] () {
                    intVal++;
                }};
            p.AddTask (task);
            p.WaitForTask (task);
            VerifyTestResult (intVal == 4);
        }
    }
}

namespace {
    void RegressionTest8_ThreadPool_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest8_ThreadPool_");
        // Make 2 concurrent tasks, which share a critical section object to take turns updating a variable
        auto doIt = [] (int* argP) {
            for (int i = 0; i < 10; i++) {
                [[maybe_unused]] auto&& critSect = lock_guard (sharedCriticalSection_);
                int                     tmp      = *argP;
                Execution::Sleep (.002);
                //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                *argP = tmp + 1;
            }
        };
        for (unsigned int threadPoolSize = 1; threadPoolSize < 10; ++threadPoolSize) {
            ThreadPool p;
            p.SetPoolSize (threadPoolSize);
            int                  updaterValue = 0;
            ThreadPool::TaskType task1{[&updaterValue, &doIt] () { doIt (&updaterValue); }};
            ThreadPool::TaskType task2{[&updaterValue, &doIt] () { doIt (&updaterValue); }};
            p.AddTask (task1);
            p.AddTask (task2);
            p.WaitForTask (task1);
            p.WaitForTask (task2);
            VerifyTestResult (updaterValue == 2 * 10);
        }
    }
}

namespace {
    void RegressionTest9_ThreadsAbortingEarly_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest9_ThreadsAbortingEarly_");
        // I was seeing SOME rare thread bug - trying to abort a thread which was itself trying to create a new thread - and was
        // between the create of thread and Abort
        Containers::Collection<Thread::Ptr> innerThreads;
        auto                                DoItInnerThread = [] () {
            Execution::Sleep (.01);
        };
        auto DoOuterThread = [DoItInnerThread, &innerThreads] () {
            while (true) {
                Thread::Ptr t = Thread::New (DoItInnerThread);
                innerThreads.Add (t);
                Execution::Sleep (.02);
                t.Start ();
            }
        };
        Thread::Ptr thread = Thread::New (DoOuterThread);
        thread.Start ();
        Execution::Sleep (.5);
        thread.AbortAndWaitForDone ();
        // NB: we must call AbortAndWaitForDone on innerThreads because we could have created the thread but not started it, so
        // wait for done will never terminate
        innerThreads.Apply ([] (Thread::Ptr t) { t.AbortAndWaitForDone (); }); // assure subthreads  complete before the text exits (else valgrind may report leak)
    }
}

namespace {
    void RegressionTest10_BlockingQueue_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest10_BlockingQueue_"};
        enum { START = 0,
               END   = 100 };
        int                              expectedValue = (START + END) * (END - START + 1) / 2;
        int                              counter       = 0;
        BlockingQueue<function<void ()>> q;

        Verify (q.GetLength () == 0);

        Thread::Ptr producerThread = Thread::New (
            [&q, &counter] () {
                for (int incBy = START; incBy <= END; ++incBy) {
                    q.AddTail ([&counter, incBy] () { counter += incBy; });
                }
                q.EndOfInput ();
            },
            Thread::eAutoStart,
            String{L"Producer"});
        Thread::Ptr consumerThread = Thread::New (
            [&q] () {
                // Since we call EndOfInput () - the RemoveHead () will eventually timeout
                while (true) {
                    function<void ()> f = q.RemoveHead ();
                    f ();
                }
            },
            Thread::eAutoStart,
            String{L"Consumer"});
        // producer already set to run off the end...
        // consumer will end due to exception reading from end
        Thread::WaitForDone ({producerThread, consumerThread});
        Verify (counter == expectedValue);
    }
}

namespace {
    void RegressionTest11_AbortSubAbort_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest11_AbortSubAbort_"};
        auto                      testFailToProperlyAbort = [] () {
            Thread::Ptr innerThread = Thread::New ([] () {
                Execution::Sleep (1000);
            });
            innerThread.SetThreadName (L"innerThread");
            Thread::Ptr testThread = Thread::New ([&innerThread] () {
                innerThread.Start ();
                Execution::Sleep (1000);
                innerThread.AbortAndWaitForDone ();
            });
            testThread.SetThreadName (L"testThread");
            testThread.Start ();
            Execution::Sleep (1); // wait til both threads running and blocked in sleeps
            testThread.AbortAndWaitForDone ();
            // This is the BUG SuppressInterruptionInContext was meant to solve!
            VerifyTestResult (innerThread.GetStatus () == Thread::Status::eRunning);
            innerThread.AbortAndWaitForDone ();
        };
        auto testInnerThreadProperlyShutDownByOuterThread = [] () {
            Thread::Ptr innerThread = Thread::New ([] () {
                Execution::Sleep (1000);
            });
            innerThread.SetThreadName (L"innerThread");
            Thread::Ptr testThread = Thread::New ([&innerThread] () {
                innerThread.Start ();
                [[maybe_unused]] auto&& cleanup = Finally (
                    [&innerThread] () noexcept {
                        Thread::SuppressInterruptionInContext suppressInterruptions;
                        innerThread.AbortAndWaitForDone ();
                    });
                Execution::Sleep (1000);
            });
            testThread.SetThreadName (L"testThread");
            testThread.Start ();
            Execution::Sleep (1); // wait til both threads running and blocked in sleeps
            // This is the BUG SuppressInterruptionInContext was meant to solve!
            testThread.AbortAndWaitForDone ();
            VerifyTestResult (innerThread.GetStatus () == Thread::Status::eCompleted);
        };
        testFailToProperlyAbort ();
        testInnerThreadProperlyShutDownByOuterThread ();
    }
}

namespace {
    void RegressionTest12_WaitAny_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest12_WaitAny_"};
        // EXPERIMENTAL
        WaitableEvent                              we1{WaitableEvent::eAutoReset};
        WaitableEvent                              we2{WaitableEvent::eAutoReset};
        static constexpr Time::DurationSecondsType kMaxWaitTime_{5.0};
        Thread::Ptr                                t1      = Thread::New ([&we1] () {
            Execution::Sleep (kMaxWaitTime_); // wait long enough that we are pretty sure t2 will always trigger before we do
            we1.Set ();
        });
        Thread::Ptr                                t2      = Thread::New ([&we2] () {
            Execution::Sleep (0.1);
            we2.Set ();
        });
        Time::DurationSecondsType                  startAt = Time::GetTickCount ();
        t1.Start ();
        t2.Start ();
        /*
         *  Saw this: FAILED: RegressionTestFailure; WaitableEvent::WaitForAny (Sequence<WaitableEvent*> ({&we1, &we2})) == set<WaitableEvent*> ({&we2});;..\..\..\38\Test.cpp: 712
         *      2017-10-10 - but just once and not since (and on loaded machine so that could have caused queer scheduling) - Windows ONLY
         *      2018-03-10 - saw happen on Linux, but still very rare
         *      2018-12-21 - saw happen on Linux (WSL), but still very rare
         *      2019-08-28 - saw happen on Linux (WSL), but still very rare
         */
        VerifyTestResult (WaitableEvent::WaitForAny (Sequence<WaitableEvent*> ({&we1, &we2})) == set<WaitableEvent*> ({&we2})); // may not indicate a real problem if triggered rarely - just threads ran in queer order, but can happen
        Time::DurationSecondsType timeTaken = Time::GetTickCount () - startAt;
        VerifyTestResult (timeTaken <= kMaxWaitTime_); // make sure we didnt wait for the 1.0 second on first thread
        // They capture so must wait for them to complete
        t1.AbortAndWaitForDone ();
        t2.AbortAndWaitForDone ();
    }
}

namespace {
    void RegressionTest13_WaitAll_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest13_WaitAll_"};
        // EXPERIMENTAL
        WaitableEvent we1{WaitableEvent::eAutoReset};
        WaitableEvent we2{WaitableEvent::eAutoReset};
        bool          w1Fired = false;
        bool          w2Fired = false;
        Thread::Ptr   t1      = Thread::New ([&we1, &w1Fired] () {
            Execution::Sleep (0.5);
            w1Fired = true;
            we1.Set ();
        });
        Thread::Ptr   t2      = Thread::New ([&we2, &w2Fired] () {
            Execution::Sleep (0.1);
            w2Fired = true;
            we2.Set ();
        });
        t2.Start ();
        t1.Start ();
        WaitableEvent::WaitForAll (Sequence<WaitableEvent*> ({&we1, &we2}));
        Stroika_Foundation_Debug_ValgrindDisableHelgrind (w1Fired); // tecnically a race
        Stroika_Foundation_Debug_ValgrindDisableHelgrind (w2Fired); // tecnically a race
        VerifyTestResult (w1Fired and w2Fired);
        // They capture so must wait for them to complete
        t1.AbortAndWaitForDone ();
        t2.AbortAndWaitForDone ();
    }
}

namespace {
    void RegressionTest14_SpinLock_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest14_SpinLock_"};
        SpinLock                  lock;
        int                       sum = 0;
        Thread::Ptr               t1  = Thread::New ([&lock, &sum] () {
            for (int i = 0; i < 100; ++i) {
                Execution::Sleep (0.001);
                lock_guard<SpinLock> critSec (lock);
                sum += i;
            }
        });
        Thread::Ptr               t2  = Thread::New ([&lock, &sum] () {
            for (int i = 0; i < 100; ++i) {
                Execution::Sleep (0.001);
                lock_guard<SpinLock> critSec (lock);
                sum -= i;
            }
        });
        t1.Start ();
        t2.Start ();
        t1.WaitForDone ();
        t2.WaitForDone ();
        Stroika_Foundation_Debug_ValgrindDisableHelgrind_START (sum);
        VerifyTestResult (sum == 0);
        Stroika_Foundation_Debug_ValgrindDisableHelgrind_END (sum);
    }
}

namespace {
    void RegressionTest15_ThreadPoolStarvationBug_ ()
    {
        //?? DO WE NEED TO ADD
        //#if 0
        //      //fTasksAdded_.WaitQuietly (0.1);
        //      fTasksAdded_.Wait ();
        //#endif
        // Maybe no bug??? BUt tried to reproduce what looked like it MIGHT be a bug/issue based on behavior in
        // BLKQCL...--LGP 2015-10-05
        //
        Debug::TraceContextBumper traceCtx ("RegressionTest15_ThreadPoolStarvationBug_");
        {
            Time::DurationSecondsType                  testStartedAt       = Time::GetTickCount ();
            static constexpr unsigned                  kThreadPoolSize_    = 10;
            static constexpr unsigned                  kStepsToGetTrouble_ = 100 * kThreadPoolSize_; // wag - should go through each thread pretty quickly
            static constexpr Time::DurationSecondsType kTime2WaitPerTask_{0.01};
            static constexpr Time::DurationSecondsType kRoughEstimateOfTime2Run_ = kTime2WaitPerTask_ * kStepsToGetTrouble_ / kThreadPoolSize_;
            ThreadPool                                 p;
            p.SetPoolSize (kThreadPoolSize_);
            auto doItHandler = [] () { Execution::Sleep (kTime2WaitPerTask_); }; // sb pretty quick

            for (int i = 0; i < kStepsToGetTrouble_; ++i) {
                p.AddTask (doItHandler);
            }

            const double              kBigSafetyMultiplierIncaseRunningUnderValgrind_{10000}; // valgrind not speedy ;-)
            Time::DurationSecondsType betterFinishBy = Time::GetTickCount () + kBigSafetyMultiplierIncaseRunningUnderValgrind_ * kRoughEstimateOfTime2Run_;
            while (Time::GetTickCount () <= betterFinishBy) {
                if (p.GetTasksCount () == 0) {
                    break;
                }
                Execution::Sleep (.5); // dont spin too aggressively.
            }
            VerifyTestResult (p.GetTasksCount () == 0);
            Time::DurationSecondsType totalTestTime = Time::GetTickCount () - testStartedAt;
            Verify (totalTestTime < kBigSafetyMultiplierIncaseRunningUnderValgrind_ * kRoughEstimateOfTime2Run_);
        }
    }
}

namespace {
    namespace RegressionTest16_SimpleThreadConstructDestructLeak_ {
        void RunTests ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest16_SimpleThreadConstructDestructLeak_::RunTests"};
            // This test doesn't do a lot by itself, but we run this test under valgrind to look for leaks
            {
                Thread::Ptr t;
            }
            {
                Thread::Ptr t = Thread::New ([] () {});
            }
        }
    }
}

namespace {
    namespace RegressionTest17_ThreadInterruption_ {
        void RunTests ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest17_ThreadInterruption_::RunTests"};
            atomic<unsigned>          interruptCnt{};
            WaitableEvent             we{WaitableEvent::eManualReset};
            Thread::Ptr               t = Thread::New (
                [&] () {
                    while (true) {
                        try {
                            Execution::Sleep (10);
                        }
                        catch (const Thread::AbortException&) {
                            Execution::ReThrow ();
                        }
                        catch (const Thread::InterruptException&) {
                            interruptCnt++;
                        }
                        we.Set ();
                    }
                },
                Thread::eAutoStart);
            constexpr unsigned int kInteruptCnt_{10};
            for (int i = 0; i < kInteruptCnt_; ++i) {
                Execution::Sleep (0.1); // if we interrupt too fast, the thread may only get one or two
                t.Interrupt ();
            }
            we.Wait ();            // so we get at least one interruption
            Execution::Sleep (.1); // so we get a few - not needed
            VerifyTestResult (interruptCnt >= 1);
            VerifyTestResult (interruptCnt <= kInteruptCnt_);
            t.AbortAndWaitForDone ();
        }
    }
}

namespace {
    namespace RegressionTest18_RWSynchronized_ {
        namespace Private_ {
            template <typename SYNCRHONIZED_INT>
            void Test1_MultipleConcurrentReaders (bool mustBeEqualToZero, unsigned int repeatCount, double sleepTime)
            {
                Debug::TraceContextBumper ctx{"...Test1_MultipleConcurrentReaders"};
                /**
                 *  Verify that both threads are maintaining the lock at the same time.
                 */
                // NOTE - CRITICALLY - IF YOU CHANGE RWSynchronized to Synchronized the VerifyTestResult about countWhereTwoHoldingRead below will fail!
                SYNCRHONIZED_INT     sharedData{0};
                atomic<unsigned int> countMaybeHoldingReadLock{0}; // if >0, definitely holding lock, if 0, maybe holding lock (cuz we decremenent before losing lock)
                atomic<unsigned int> countWhereTwoHoldingRead{0};
                atomic<unsigned int> sum1{};
                auto                 lambda = [&] () {
                    Debug::TraceContextBumper ctx{"...lambda"};
                    for (unsigned int i = 0; i < repeatCount; i++) {
                        auto holdReadOnlyLock = sharedData.cget ();
                        countMaybeHoldingReadLock++;
                        Execution::Sleep (sleepTime);
                        sum1 += holdReadOnlyLock.load ();
                        if (countMaybeHoldingReadLock >= 2) {
                            countWhereTwoHoldingRead++;
                        }
                        countMaybeHoldingReadLock--;
                    }
                };
                Thread::Ptr t1 = Thread::New (lambda);
                Thread::Ptr t2 = Thread::New (lambda);
                Thread::Start ({t1, t2});
                Thread::WaitForDone ({t1, t2});
                if (mustBeEqualToZero) {
                    VerifyTestResult (countWhereTwoHoldingRead == 0);
                }
                else {
                    // This failed once under helgrind && docker, perhaps while other tests running, so not worrisome unless we see again -- LGP 2018-12-14
                    VerifyTestResult (countWhereTwoHoldingRead >= 1 or sleepTime <= 0); // not logically true, but a good test.. (if sleepTime == 0, this is less likely to be true - so dont fail test because of it)
                }
                DbgTrace (L"countWhereTwoHoldingRead=%u (percent=%f)", countWhereTwoHoldingRead.load (), 100.0 * double (countWhereTwoHoldingRead.load ()) / (2 * repeatCount));
            }
            void Test2_LongWritesBlock_ ()
            {
                Debug::TraceContextBumper                  ctx{"Test2_LongWritesBlock_"};
                static constexpr int                       kBaseRepititionCount_ = 500;
                static constexpr Time::DurationSecondsType kBaseSleepTime_       = 0.001;
                RWSynchronized<int>                        syncData{0};
                Thread::Ptr                                readerThread = Thread::New ([&] () {
                    Debug::TraceContextBumper ctx{"readerThread"};
                    // Do 10x more reads than writer loop, but sleep 1/10th as long
                    for (int i = 0; i < kBaseRepititionCount_ * 10; ++i) {
                        VerifyTestResult (syncData.cget ().load () % 2 == 0);
                        // occasional sleep so the reader doesn't get ahead of writer, but rarely cuz this is very slow on linux (ubuntu 1804) - often taking > 2ms, even for sleep of 100us) -- LGP 2018-06-20
                        if (i % 100 == 0) {
                            Execution::Sleep (kBaseSleepTime_ / 10.0); // hold the lock kBaseSleepTime_ / 10.0
                        }
                    }
                });
                Thread::Ptr                                writerThread = Thread::New ([&] () {
                    Debug::TraceContextBumper ctx{"writerThread"};
                    for (int i = 0; i < kBaseRepititionCount_; ++i) {
                        auto rwLock = syncData.rwget ();
                        rwLock.store (rwLock.load () + 1);  // set to a value that will cause reader thread to fail
                        Execution::Sleep (kBaseSleepTime_); // hold the lock kBaseSleepTime_
                        VerifyTestResult (rwLock.load () % 2 == 1);
                        rwLock.store (rwLock.load () + 1); // set to a safe value
                    }
                    VerifyTestResult (syncData.cget ().load () == kBaseRepititionCount_ * 2);
                });
                Thread::Start ({readerThread, writerThread});
                Thread::WaitForDone ({readerThread, writerThread});
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest18_RWSynchronized_"};
            static const bool         kRunningValgrind_ = Debug::IsRunningUnderValgrind ();

            // https://stroika.atlassian.net/browse/STK-632
            // Most likely some sort of memory corruption, and given notes in https://stroika.atlassian.net/browse/STK-632 - seems
            // most likely helgrind bug - hopefully fixed soon.
            bool hasBug632AndRunningHelgrind = kRunningValgrind_; // not easy to check
            if (not hasBug632AndRunningHelgrind) {
                // if using RWSynchonized, we must get overlap, and if using Synchonized<> (no shared lock) - we must not get overlap (first arg to test function)
                Private_::Test1_MultipleConcurrentReaders<RWSynchronized<int>> (false, kRunningValgrind_ ? 1000u : 10000u, 0.0);
                Private_::Test1_MultipleConcurrentReaders<Synchronized<int>> (true, kRunningValgrind_ ? 1000u : 10000u, 0.0);
                Private_::Test1_MultipleConcurrentReaders<RWSynchronized<int>> (false, kRunningValgrind_ ? 100u : 1000u, 0.001);
                Private_::Test1_MultipleConcurrentReaders<Synchronized<int>> (true, kRunningValgrind_ ? 100u : 250u, 0.001);
                Private_::Test2_LongWritesBlock_ ();
            }
        }
    }
}

namespace {
    namespace RegressionTest19_ThreadPoolAndBlockingQueue_ {
        namespace Private_ {
            void TEST_ ()
            {
                static const bool kRunningValgrind_ = Debug::IsRunningUnderValgrind ();

                static const unsigned int kThreadPoolSize_{kRunningValgrind_ ? 5u : 10u};
                ThreadPool                consumerThreadPool{kThreadPoolSize_, L"consumers"};
                ThreadPool                producerThreadPool{kThreadPoolSize_, L"producers"};

                enum {
                    START = 0,
                    END   = 100
                };
                atomic<uint64_t>                 counter{};
                BlockingQueue<function<void ()>> q;

                Verify (q.GetLength () == 0);

                static const size_t kTaskCounts_ = kRunningValgrind_ ? (kThreadPoolSize_ * 5) : (kThreadPoolSize_ * 10);

                for (size_t i = 0; i < kTaskCounts_; ++i) {
                    producerThreadPool.AddTask (
                        [&q, &counter] () {
                            for (int incBy = START; incBy <= END; ++incBy) {
                                q.AddTail ([&counter, incBy] () {
                                    counter += incBy;
                                });
                            }
                        });
                    consumerThreadPool.AddTask (
                        [&q] () {
                            while (true) {
                                function<void ()> f = q.RemoveHead ();
                                f ();
                            }
                        });
                }

                // wait for all producers to be done, and then mark the input Q with EOF
                producerThreadPool.WaitForTasksDone ();
                q.EndOfInput ();

                // Wait for consumers to finish, and validate their side-effect - count - is correct.
                consumerThreadPool.WaitForTasksDone ();
                int expectedValue = ((START + END) * (END - START + 1) / 2) * (int)kTaskCounts_;
                Verify (counter == expectedValue);
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest19_ThreadPoolAndBlockingQueue_"};
            Private_::TEST_ ();
        }
    }
}

namespace {
    void RegressionTest20_BlockingQueueWithRemoveHeadIfPossible_ ()
    {
        // For fixed bug - https://stroika.atlassian.net/browse/STK-622
        Debug::TraceContextBumper ctx{"RegressionTest20_BlockingQueueWithRemoveHeadIfPossible_"};
        enum {
            START = 0,
            END   = 100
        };
        int                              expectedValue = (START + END) * (END - START + 1) / 2;
        int                              counter       = 0;
        BlockingQueue<function<void ()>> q;

        Verify (q.GetLength () == 0);

        Thread::Ptr consumerThread = Thread::New (
            [&q] () {
                while (not q.QAtEOF ()) {
                    if (optional<function<void ()>> of = q.RemoveHeadIfPossible ()) {
                        function<void ()> f = *of;
                        f ();
                    }
                    else {
                        Execution::Sleep (.1); // give time for producer to catch up
                    }
                }
            },
            Thread::eAutoStart,
            String{L"Consumer"});
        Execution::Sleep (0.1); // so consume gets a chance to fail removehead at least once...
        Thread::Ptr producerThread = Thread::New (
            [&q, &counter] () {
                for (int incBy = START; incBy <= END; ++incBy) {
                    q.AddTail ([&counter, incBy] () { counter += incBy; });
                    if (incBy == (END - START) / 2) {
                        Execution::Sleep (100ms); // illogical in real app, but give time for consumer to catch up to help check no race
                    }
                }
                q.EndOfInput ();
            },
            Thread::eAutoStart,
            String{L"Producer"});
        // producer already set to run off the end...
        // consumer will end due to exception reading from end
        Thread::WaitForDone ({producerThread, consumerThread});
        Verify (counter == expectedValue);
    }
}

namespace {
    void RegressionTest21_BlockingQueueAbortWhileBlockedWaiting_ ()
    {
        // For fixed bug - https://stroika.atlassian.net/browse/STK-622
        Debug::TraceContextBumper        ctx{"RegressionTest21_BlockingQueueAbortWhileBlockedWaiting_"};
        BlockingQueue<function<void ()>> q;
        Verify (q.GetLength () == 0);
        Thread::Ptr consumerThread = Thread::New (
            [&q] () {
                while (true) {
                    function<void ()> f = q.RemoveHead ();
                    f ();
                }
            },
            Thread::eAutoStart,
            String{L"Consumer"});
        Execution::Sleep (0.5);
        // make sure we can interrupt a blocking read on the BlockingQueue
        consumerThread.AbortAndWaitForDone ();
    }
}

namespace {
    void RegressionTest22_SycnhonizedUpgradeLock_ ()
    {
        /*
         *  The idea here is we want most of our threads to only open readonly lock, and only occasionally upgrade the lock.
         *
         *  BUt there can still be one special thread that always write locks
         */
        Debug::TraceContextBumper ctx{"RegressionTest22_SycnhonizedUpgradeLock_"};

        static const bool kRunningValgrind_ = Debug::IsRunningUnderValgrind ();

        // https://stroika.atlassian.net/browse/STK-632
        // This helgrind bug ONLY happens when we run this at the end. If we run this as the only test it works fine.
        // Most likely some sort of memory corruption, and given notes in https://stroika.atlassian.net/browse/STK-632 - seems
        // most likely helgrind bug - hopefully fixed soon.
        bool hasBug632AndRunningHelgrind = kRunningValgrind_; // not easy to check
        if (not hasBug632AndRunningHelgrind) {

            auto testUpgradeLockNonAtomically1 = [] (auto& isEven) {
                while (true) {
                    Execution::CheckForThreadInterruption ();
                    auto rLock = isEven.cget ();
                    if (rLock.load ()) {
                        isEven.UpgradeLockNonAtomically (&rLock, [&] (auto&& writeLock) {
                            // MUST RECHECK writeLock.load () for now because UpgradeLockNonAtomically () unlocks first and lets others get a crack
                            if (writeLock.load ()) {
                                writeLock.store (false);
                            }
                        });
                        // WE CANNOT test this - because UpgradeLockNonAtomically () releases lock before re-acuqitring readlock - but should fix that soon
                        // so we can test this!!!
                        //VerifyTestResult (not isEven.cget ());
                    }
                }
            };
            auto testUpgradeLockNonAtomically2 = [] (auto& isEven) {
                while (true) {
                    Execution::CheckForThreadInterruption ();
                    auto rLock = isEven.cget ();
                    if (rLock.load ()) {
                        isEven.UpgradeLockNonAtomically (&rLock, [&] (auto&& writeLock, bool interveningWriteLock) {
                            if (interveningWriteLock) {
                                // MUST RECHECK writeLock.load () for now because UpgradeLockNonAtomically () unlocks first and lets others get a crack
                                if (writeLock.load ()) {
                                    writeLock.store (false);
                                }
                            }
                            else {
                                // in this case we effectively did an atomic upgrade, because no intervening writers
#if qCompilerAndStdLib_GenericLambdaInsideGenericLambdaAssertCall_Buggy
                                bool t = writeLock.load ();
                                if (not t) {
                                    DbgTrace ("***assert false");
                                }
#else
                                Assert (writeLock.load ());
#endif
                                writeLock.store (false);
                            }
                        });
                    }
                }
            };
#if __has_include(<boost/thread/shared_mutex.hpp>)
            auto testUpgradeLockAtomically1 = [&] (auto& isEven) {
                while (true) {
                    Execution::CheckForThreadInterruption ();
                    auto rLock = isEven.cget ();
                    if (rLock.load ()) {
                        try {
                            isEven.UpgradeLockAtomically (
                                &rLock,
#if qCompilerAndStdLib_template_GenericLambdaInsideGenericLambdaDeductionInternalError_Buggy
                                [&] (UpgradableRWSynchronized<bool>::WritableReference&& writeLock) {
                                    Assert (writeLock.load ());
                                    writeLock.store (false);
                                }
#else
                                [&] (auto&& writeLock) {
                                    Assert (writeLock.load ());
                                    writeLock.store (false);
                                }
#endif
                                ,
                                1s);
                        }
                        catch (const Execution::TimeOutException&) {
                            DbgTrace ("Got timeout in testUpgradeLockAtomically - expected and ignored... (retry)"); // deadlock avoidance
                            continue;
                        }
                        VerifyTestResult (not isEven.cget ()); // must be true because we upgraded lock atomically, and then we still have readlock so nobody else can update it
                    }
                }
            };
            auto testUpgradeLockAtomically2 = [&] (auto& isEven) {
                while (true) {
                    unsigned int tries = 0;
                    unsigned int fails = 0;
                again:
                    Execution::CheckForThreadInterruption ();
                    Execution::Sleep (10ms);
                    auto rLock = isEven.cget ();
                    if (rLock.load ()) {
                        tries++;
                        if (not isEven.UpgradeLockAtomicallyQuietly (
                                &rLock,
#if qCompilerAndStdLib_template_GenericLambdaInsideGenericLambdaDeductionInternalError_Buggy
                                [&] (UpgradableRWSynchronized<bool>::WritableReference&& writeLock) {
                                    Assert (writeLock.load ());
                                    writeLock.store (false);
                                }
#else
                                [&] (auto&& writeLock) {
                                    Assert (writeLock.load ());
                                    writeLock.store (false);
                                }
#endif
                                ,
                                100ms)) {
                            fails++;
                            DbgTrace (L"fails/tries = %d/%d", fails, tries);
                            goto again; // retry the lock
                        }
                        DbgTrace (L"fails/tries! = %d/%d", fails, tries);
                        VerifyTestResult (not isEven.cget ()); // must be true because we upgraded lock atomically, and then we still have readlock so nobody else can update it
                    }
                }
            };
#endif

            auto runSyncTest = [] (auto& isEven, auto readerFun) {
                Thread::Ptr writerThread                     = Thread::New ([&] () {
                    while (true) {
                        Execution::Sleep (50ms);
                        Execution::CheckForThreadInterruption ();
                        auto rwLock = isEven.rwget ();
                        rwLock.store (not rwLock.load ()); // toggle back and forth
                    }
                },
                                                        Thread::eAutoStart);
                Thread::Ptr readerThatSometimesWritesThread1 = Thread::New (readerFun, Thread::eAutoStart);
                Thread::Ptr readerThatSometimesWritesThread2 = Thread::New (readerFun, Thread::eAutoStart);
                Execution::Sleep (3s);
                Thread::AbortAndWaitForDone ({writerThread, readerThatSometimesWritesThread1, readerThatSometimesWritesThread2});
            };

            {
                Debug::TraceContextBumper ctx1{"run-test (1) RWSynchronized NonAtomically"};
                RWSynchronized<bool>      isEven{true};
                runSyncTest (isEven, [&] () { testUpgradeLockNonAtomically1 (isEven); });
            }
            {
                Debug::TraceContextBumper ctx1{"run-test (2) RWSynchronized NonAtomically"};
                RWSynchronized<bool>      isEven{true};
                runSyncTest (isEven, [&] () { testUpgradeLockNonAtomically2 (isEven); });
            }
#if __has_include(<boost/thread/shared_mutex.hpp>)
            {
                Debug::TraceContextBumper      ctx1{"run-test (1) UpgradableRWSynchronized NonAtomically"};
                UpgradableRWSynchronized<bool> isEven{true};
                runSyncTest (isEven, [&] () { testUpgradeLockNonAtomically1 (isEven); });
            }
            {
                Debug::TraceContextBumper      ctx1{"run-test (2) UpgradableRWSynchronized NonAtomically"};
                UpgradableRWSynchronized<bool> isEven{true};
                runSyncTest (isEven, [&] () { testUpgradeLockNonAtomically2 (isEven); });
            }
            {
                Debug::TraceContextBumper      ctx1{"run-test UpgradableRWSynchronized Atomically"};
                UpgradableRWSynchronized<bool> isEven{true};
                runSyncTest (isEven, [&] () { testUpgradeLockAtomically1 (isEven); });
            }
            {
                Debug::TraceContextBumper      ctx1{"run-test UpgradableRWSynchronized Atomically (quietly)"};
                UpgradableRWSynchronized<bool> isEven{true};
                runSyncTest (isEven, [&] () { testUpgradeLockAtomically2 (isEven); });
            }
#endif
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
        [[maybe_unused]] auto&& cleanupReport = Finally (
            [] () {
                auto runningThreads = Execution::Thread::GetStatistics ().fRunningThreads;
                DbgTrace (L"Total Running threads at end: %d", runningThreads.size ());
                for (Execution::Thread::IDType threadID : runningThreads) {
                    DbgTrace (L"Exiting main with thread %s running", Characters::ToString (threadID).c_str ());
                }
                VerifyTestResult (runningThreads.size () == 0);
            });
#endif
        RegressionTest1_ ();
        RegressionTest2_ ();
        RegressionTest3_WaitableEvents_ ();
        RegressionTest4_Synchronized_::DoIt ();
        RegressionTest5_Aborting_ ();
        RegressionTest6_ThreadWaiting_ ();
        RegressionTest7_SimpleThreadPool_ ();
        RegressionTest8_ThreadPool_ ();
        RegressionTest9_ThreadsAbortingEarly_ ();
        RegressionTest10_BlockingQueue_ ();
        RegressionTest11_AbortSubAbort_ ();
        RegressionTest12_WaitAny_ ();
        RegressionTest13_WaitAll_ ();
        RegressionTest14_SpinLock_ ();
        RegressionTest15_ThreadPoolStarvationBug_ ();
        RegressionTest16_SimpleThreadConstructDestructLeak_::RunTests ();
        RegressionTest17_ThreadInterruption_::RunTests ();
        RegressionTest18_RWSynchronized_::DoIt ();
        RegressionTest19_ThreadPoolAndBlockingQueue_::DoIt ();
        RegressionTest20_BlockingQueueWithRemoveHeadIfPossible_ ();
        RegressionTest21_BlockingQueueAbortWhileBlockedWaiting_ ();
        RegressionTest22_SycnhonizedUpgradeLock_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
