/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::Execution::ThreadSafetyBuiltinObject
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <mutex>

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Bijection.h"
#include    "Stroika/Foundation/Containers/Collection.h"
#include    "Stroika/Foundation/Containers/Deque.h"
#include    "Stroika/Foundation/Containers/Queue.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Containers/MultiSet.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Containers/Stack.h"
#include    "Stroika/Foundation/Containers/SortedCollection.h"
#include    "Stroika/Foundation/Containers/SortedMapping.h"
#include    "Stroika/Foundation/Containers/SortedMultiSet.h"
#include    "Stroika/Foundation/Containers/SortedSet.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Execution/Thread.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Time/DateTime.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Characters;
using   namespace   Containers;

using   Execution::Synchronized;
using   Execution::Thread;
using   Execution::WaitableEvent;






namespace {

    /*
     *  To REALLY this this code for thread-safety, use ExternallySynchronizedLock, but to verify it works
     *  without worrying about races, just use mutex.
     */
    struct  no_lock_ {
        void lock () {}
        void unlock () {}
    };

}



namespace {
    void    RunThreads_ (const initializer_list<Thread>& threads)
    {
        for (Thread i : threads) {
            i.Start ();
        }
        for (Thread i : threads) {
            i.WaitForDone ();
        }
    }
}





namespace {

    template <typename ITERABLE_TYPE, typename LOCK_TYPE>
    Thread  mkIterateOverThread_ (Synchronized<ITERABLE_TYPE>* iterable, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        using value_type   =   typename ITERABLE_TYPE::value_type;
        return Thread ([iterable, lock, repeatCount] () {
            Debug::TraceContextBumper traceCtx ("{}IterateOverThread::MAIN...");
            for (unsigned int i = 0; i < repeatCount; ++i) {
                //DbgTrace ("Iterate thread loop %d", i);
                lock_guard<decltype(*lock)> critSec (*lock);
                for (value_type e :  iterable->load ()) {
                    value_type e2 = e; // do something
                }
            }
        });
    };

}



namespace {

    template <typename ITERABLE_TYPE, typename ITERABLE_TYPE2, typename LOCK_TYPE>
    Thread  mkOverwriteThread_ (ITERABLE_TYPE* oneToKeepOverwriting, ITERABLE_TYPE2 elt1, ITERABLE_TYPE2 elt2, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        return Thread ([oneToKeepOverwriting, lock, repeatCount, elt1, elt2] () {
            Debug::TraceContextBumper traceCtx ("{}OverwriteThread::MAIN...");
            for (unsigned int i = 0; i < repeatCount; ++i) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype(*lock)> critSec (*lock);
                        (*oneToKeepOverwriting) = elt1;
                    }
                    else {
                        lock_guard<decltype(*lock)> critSec (*lock);
                        (*oneToKeepOverwriting) = elt2;
                    }
                }
            }
        });
    };

}








namespace   {
    namespace AssignAndIterateAtSameTimeTest_1_ {
        template    <typename ITERABLE_TYPE>
        void    DoItOnce_ (ITERABLE_TYPE elt1, ITERABLE_TYPE elt2, unsigned int repeatCount)
        {
            Debug::TraceContextBumper traceCtx ("{}::AssignAndIterateAtSameTimeTest_1_::DoIt::DoItOnce_ ()");
            no_lock_ lock ;
            //mutex lock;
            Synchronized<ITERABLE_TYPE>  oneToKeepOverwriting { elt1 };
            Thread  iterateThread   =   mkIterateOverThread_ (&oneToKeepOverwriting, &lock, repeatCount);
            Thread  overwriteThread =   mkOverwriteThread_ (&oneToKeepOverwriting, elt1, elt2, &lock, repeatCount);
            RunThreads_ ({iterateThread, overwriteThread});
        }
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("AssignAndIterateAtSameTimeTest_1_::DoIt ()");
            const unsigned int kRepeatCount_ = 500;
            //const unsigned int kRepeatCount_ = 1;
            static const initializer_list<int>  kOrigValueInit_ = {1, 3, 4, 5, 6, 33, 12, 13};
            static const initializer_list<int>  kUpdateValueInit_ = {4, 5, 6, 33, 12, 34, 596, 13, 1, 3, 99, 33, 4, 5};
            static const initializer_list<pair<int, int>>  kOrigPairValueInit_ = {pair<int, int> (1, 3), pair<int, int> (4, 5), pair<int, int> (6, 33), pair<int, int> (12, 13)};
            static const initializer_list<pair<int, int>>  kUPairpdateValueInit_ = {pair<int, int> (4, 5), pair<int, int> (6, 33), pair<int, int> (12, 34), pair<int, int> (596, 13), pair<int, int> (1, 3), pair<int, int> (99, 33), pair<int, int> (4, 5)};
            DoItOnce_<String> (String (L"123456789"), String (L"abcdedfghijkqlmopqrstuvwxyz"), kRepeatCount_);
            DoItOnce_<Bijection<int, int>> (Bijection<int, int> (kOrigPairValueInit_), Bijection<int, int> (kUPairpdateValueInit_), kRepeatCount_);
            DoItOnce_<Collection<int>> (Collection<int> (kOrigValueInit_), Collection<int> (kUpdateValueInit_), kRepeatCount_);
            // Queue/Deque NYI here cuz of assign from initializer
            //DoItOnce_<Deque<int>> (Deque<int> (kOrigValueInit_), Deque<int> (kUpdateValueInit_), kRepeatCount_);
            //DoItOnce_<Queue<int>> (Queue<int> (kOrigValueInit_), Queue<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<MultiSet<int>> (MultiSet<int> (kOrigValueInit_), MultiSet<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<Mapping<int, int>> (Mapping<int, int> (kOrigPairValueInit_), Mapping<int, int> (kUPairpdateValueInit_), kRepeatCount_);
            DoItOnce_<Sequence<int>> (Sequence<int> (kOrigValueInit_), Sequence<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<Set<int>> (Set<int> (kOrigValueInit_), Set<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<SortedCollection<int>> (SortedCollection<int> (kOrigValueInit_), SortedCollection<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<SortedMapping<int, int>> (SortedMapping<int, int> (kOrigPairValueInit_), SortedMapping<int, int> (kUPairpdateValueInit_), kRepeatCount_);
            DoItOnce_<SortedMultiSet<int>> (SortedMultiSet<int> (kOrigValueInit_), SortedMultiSet<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<SortedSet<int>> (SortedSet<int> (kOrigValueInit_), SortedSet<int> (kUpdateValueInit_), kRepeatCount_);

            // Stack NYI cuz not enough of stack implemented (op=)
            //DoItOnce_<Stack<int>> (Stack<int> (kOrigValueInit_), Stack<int> (kUpdateValueInit_), kRepeatCount_);
        }
    }
}









namespace   {
    namespace IterateWhileMutatingContainer_Test_2_ {
        template    <typename ITERABLE_TYPE, typename LOCK, typename MUTATE_FUNCTION>
        void    DoItOnce_ (LOCK* lock, ITERABLE_TYPE elt1, unsigned int repeatCount, MUTATE_FUNCTION baseMutateFunction)
        {
            Synchronized<ITERABLE_TYPE>   oneToKeepOverwriting { elt1 };
            auto mutateFunction =               [&oneToKeepOverwriting, lock, repeatCount, &baseMutateFunction] () {
                Debug::TraceContextBumper traceCtx ("{}::MutateFunction ()");
                DbgTrace ("(type %s)", typeid (ITERABLE_TYPE).name());
                for (unsigned int i = 0; i < repeatCount; ++i) {
                    baseMutateFunction (&oneToKeepOverwriting);
                }
            };
            Thread  iterateThread   { mkIterateOverThread_ (&oneToKeepOverwriting, lock, repeatCount) };
            Thread  mutateThread    { mutateFunction };
            RunThreads_ ({iterateThread, mutateThread});
        }
        void    DoIt ()
        {
            // This test (used to) demonstrate the need for qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
            // but been fixed
            Debug::TraceContextBumper traceCtx ("IterateWhileMutatingContainer_Test_2_::DoIt ()");

            const unsigned int kRepeatCount_ = 250;

#if     qCompilerAndStdLib_constexpr_stdinitializer_Buggy
            static const        initializer_list<int>   kOrigValueInit_ = {1, 3, 4, 5, 6, 33, 12, 13};
#else
            static constexpr    initializer_list<int>   kOrigValueInit_ = {1, 3, 4, 5, 6, 33, 12, 13};
#endif
            static const        initializer_list<int>   kUpdateValueInit_ = {4, 5, 6, 33, 12, 34, 596, 13, 1, 3, 99, 33, 4, 5};

            no_lock_ lock;
            //mutex lock;

            DoItOnce_<Set<int>> (
                                 &lock,
                                 Set<int> (kOrigValueInit_),
                                 kRepeatCount_,
            [&lock] (Synchronized<Set<int>>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    //DbgTrace ("doing update loop %d", ii);
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = Set<int> { kUpdateValueInit_ };
                    }
                    else {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = Set<int> { kUpdateValueInit_ };
                    }
                }
            });

            DoItOnce_<Sequence<int>> (
                                      &lock,
                                      Sequence<int> (kOrigValueInit_),
                                      kRepeatCount_,
            [&lock] (Synchronized<Sequence<int>>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = Sequence<int> { kUpdateValueInit_ };
                    }
                    else {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = Sequence<int> { kUpdateValueInit_ };
                    }
                }
            });

            DoItOnce_<String> (
                &lock,
                String (L"123456789"),
                kRepeatCount_,
            [&lock] (Synchronized<String>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = String {L"abc123"};
                    }
                    else {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = String {L"123abc"};
                    }
                }
            });
        }
    }
}






namespace {
    namespace Test3_SynchronizedOptional_ {
        void    DoIt_ ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test3_SynchronizedOptional_::DoIt ()");
            using   namespace   Memory;
            try {
                Synchronized<Optional<int>> sharedValue { 0 };
                static  constexpr int kMaxVal_ = qStroika_FeatureSupported_Valgrind ? 10000 : 100000;
                Thread  reader {[&sharedValue] ()
                {
                    while (sharedValue.load () < kMaxVal_) {
                        VerifyTestResult (sharedValue.load () <= kMaxVal_);
                    }
                    VerifyTestResult (sharedValue.load () == kMaxVal_);
                }
                               };
                Thread  adder {[&sharedValue] ()
                {
                    while (sharedValue.load () < kMaxVal_) {
                        sharedValue.store (*sharedValue.load () + 1);
                    }
                    VerifyTestResult (sharedValue.load () == kMaxVal_);
                }
                              };
                reader.Start ();
                adder.Start ();
                auto&& cleanup  =   Execution::Finally ([&reader, &adder] () {
                    reader.AbortAndWaitForDone ();
                    adder.AbortAndWaitForDone ();
                }
                                                       );
                // wait long time cuz of debuggers (esp valgrind) etc
                adder.WaitForDone (15 * 60);
                reader.WaitForDone (15 * 60);
                VerifyTestResult (sharedValue.load () == kMaxVal_);
            }
            catch (...) {
                VerifyTestResult (false);
            }
        }
    }
}




namespace {
    namespace Test4_CvtOp_BehaviorNeededforSyncronize_ {
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test4_CvtOp_BehaviorNeededforSyncronize_::DoIt ()");
#if 0
            struct  Base {
                mutable bool    fCalledOp_ = false;
            };
            struct  Derived : Base {
                operator Base () const
                {
                    fCalledOp_ = true;
                }
            };
            Derived dd;
            Base bb = dd;
            // sadly this doesnt work
            // --LGP 2014-09-27
            VerifyTestResult (bb.fCalledOp_);
#endif
        }
    }
}




namespace {
    namespace   Test5_SetSpecificSyncMethods {
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test5_SetSpecificSyncMethods::DoIt ()");
            Set<int>                                sensorsToActuallyRead       { 2, 3 };
            static  const   Synchronized<Set<int>>  kACUSensors_                { Set<int> { 1, 2 } };
            Set<int>                                acufpgaSensors1     =   kACUSensors_ ^ sensorsToActuallyRead;
            Set<int>                                acufpgaSensors2     =   sensorsToActuallyRead ^ kACUSensors_;
            VerifyTestResult (acufpgaSensors1 == Set<int> ({ 2 }));
            VerifyTestResult (acufpgaSensors2 == Set<int> ({ 2 }));
        }
    }
}





namespace {
    namespace   Test6_OverloadsWithSyncMethods_ {
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test6_OverloadsWithSyncMethods_::DoIt ()");
            using   Memory::Optional;
            String xx;
            Synchronized<String> yy;
            if (xx != yy) {
            }
            Optional<String> xxo;
#if 0
            //////@todo FIX various operator overloads so this works more seemlessly. See
            //      template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = typename enable_if<is_constructible<Optional<T, TRAITS>, RHS_CONVERTABLE_TO_OPTIONAL>::value >::type>
            //      bool    operator!= (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            // and related failed attempts
            if (xxo != yy.load ()) {
            }
            if (xxo == yy.load ()) {
            }
#else
            if (xxo != yy) {
            }
            if (xxo == yy) {
            }
#endif
        }
    }
}





namespace {
    namespace   Test7_Synchronized_ {
        namespace Private_ {
            void    TestBasics_ ()
            {
                using   namespace Execution;
                {
                    Synchronized<int>    tmp;
                    tmp = 4;
                    int a  { tmp };
                    VerifyTestResult (a == 4);
                }
                {
                    Synchronized<int>    tmp { 4 };
                    int a { tmp };
                    VerifyTestResult (a == 4);
                }
                {
                    Synchronized<String>    tmp { L"x" };
                    String a { tmp };
                    VerifyTestResult (a == L"x");
                    VerifyTestResult (tmp.cget ()->find ('z') == string::npos);
                    VerifyTestResult (tmp.cget ()->find ('x') == 0);
                }
                {
                    static  Synchronized<String>    tmp { L"x" };
                    auto    critSec { Execution::make_unique_lock (tmp) };    // make sure explicit locks work too
                    String a { tmp };
                    VerifyTestResult (a == L"x");
                    VerifyTestResult (tmp.cget ()->find ('z') == string::npos);
                    VerifyTestResult (tmp.cget ()->find ('x') == 0);
                }
            }
            template <typename INTISH_TYPE>
            void    DoInterlocktest_ (function<void(INTISH_TYPE*)> increment, function<void(INTISH_TYPE*)> decrement)
            {
                using   namespace Execution;
                constexpr   int     kMaxTimes_ = 100 * 1000;
                INTISH_TYPE s   =   0;
                Thread incrementer ([&s, increment, kMaxTimes_] () {
                    for (int i = 0; i < kMaxTimes_; ++i) {
                        increment (&s);
                    }
                });
                Thread decrementer ([&s, decrement, kMaxTimes_] () {
                    for (int i = 0; i < kMaxTimes_; ++i) {
                        decrement (&s);
                    }
                });
                incrementer.Start();
                decrementer.Start();
                incrementer.WaitForDone();
                decrementer.WaitForDone();
                VerifyTestResult (s == INTISH_TYPE (0));
            }
            void    DoThreadTest_ ()
            {
                using   namespace Execution;
                if (false) {
                    // Fails cuz no synchonization
                    DoInterlocktest_<int> ([] (int* i) {(*i)++;} , [] (int* i) {(*i)--;});
                }
                struct intish_object1 {
                    int fVal;
                    intish_object1 (int i) : fVal (i) {}
                    bool operator == (const intish_object1& rhs) const { return rhs.fVal == fVal; }
                };
                if (false) {
                    // Fails cuz no synchonization
                    DoInterlocktest_<intish_object1> ([] (intish_object1 * i) {(i->fVal)++;} , [] (intish_object1 * i) {(i->fVal)--;});
                }
                DoInterlocktest_<Synchronized<intish_object1>> ([] (Synchronized<intish_object1>* i) {(i->rwget ()->fVal)++;} , [] (Synchronized<intish_object1>* i) {(i->rwget ()->fVal)--;});
            }
        }
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test7_Synchronized_::DoIt ()");
            Private_::TestBasics_ ();
            Private_::DoThreadTest_ ();
        }
    }
}





namespace {
    namespace   Test8_AssertExternallySynchronized_ {
        namespace Private_ {
            void    TestBasics_ ()
            {
                struct A {
                    int x;
                };
                struct APrime : Debug::AssertExternallySynchronizedLock {
                    int x;
                };
#if !qDebug
                VerifyTestResult (sizeof (A) == sizeof (APrime));
#endif
            }
        }
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test8_AssertExternallySynchronized_::DoIt ()");
            Private_::TestBasics_ ();
        }
    }
}









namespace {
    namespace   Test9_MutlipleThreadsReadingUnsynchonizedContainer_ {
        namespace Private_ {
            void    TestBasics_ ()
            {
                static  constexpr   size_t  kIOverallRepeatCount_                   { (qDebug or qStroika_FeatureSupported_Valgrind) ? 100 : 1000 };    // tweak count cuz too slow
                Sequence<int>   tmp = Traversal::DiscreteRange<int> { 1, 1000 };
                Thread  t1 {
                    [&tmp] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (int j : tmp) {
                                VerifyTestResult (1 <= j and j <= 1000);
                            }
                        }
                    }
                };
                Thread  t2 {
                    [&tmp] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (int j : tmp) {
                                VerifyTestResult (1 <= j and j <= 1000);
                            }
                        }
                    }
                };
                Thread  t3 {
                    [&tmp] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            if (tmp.GetLength () == 1000) {
                                VerifyTestResult (tmp.IndexOf (6) == 5);
                                VerifyTestResult (tmp.GetFirst () == 1);
                                VerifyTestResult (tmp.GetLast () == 1000);
                            }
                        }
                    }
                };
                Thread::Start ({ t1, t2, t3 });
                Thread::WaitForDone ({ t1, t2, t3 });
            }
        }
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx ("{}::Test9_MutlipleThreadsReadingUnsynchonizedContainer_::DoIt ()");
            Private_::TestBasics_ ();
        }
    }
}













namespace {
    namespace   Test10_MutlipleThreadsReadingOneUpdateUsingSynchonizedContainer_ {
        namespace Private_ {
            template    <typename CONTAINER, typename ADD_FUNCTION, typename REMOVE_FUNCTION, typename EXAMINE_FUNCTION, typename ITER_FUNCTION>
            void    TestBasics_ (ADD_FUNCTION addF, REMOVE_FUNCTION remF, EXAMINE_FUNCTION examineF, ITER_FUNCTION iterF)
            {
                static  constexpr   size_t  kIOverallRepeatCount_                   { (qDebug or qStroika_FeatureSupported_Valgrind) ? 100 : 1000 };
                static  constexpr   int     kInnterConstantForHowMuchStuffTodo_     { (qDebug or qStroika_FeatureSupported_Valgrind) ? 250 : 1000 };
                Synchronized<CONTAINER>   syncObj;
                Thread  adderThread {
                    [&syncObj, &addF] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (int j : Traversal::DiscreteRange<int> { 1, kInnterConstantForHowMuchStuffTodo_ }) {
#if 1
                                auto l = syncObj.rwget ();
                                addF (&l.rwref (), j);
#else
                                addF (&syncObj.rwget ().rwref (), j);
#endif
                            }
                        }
                    }
                };
                Thread  removerThread {
                    [&syncObj, &remF] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (int j : Traversal::DiscreteRange<int> { 1, kInnterConstantForHowMuchStuffTodo_ }) {
#if 1
                                auto l = syncObj.rwget ();
                                remF (&l.rwref (), j);
#else
                                remF (&syncObj.rwget ().rwref (), j);
#endif
                            }
                        }
                    }
                };
                Thread  examineThread {
                    [&syncObj, &examineF] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            examineF (&syncObj.cget ().cref ());
                        }
                    }
                };
                Thread  walkerThread {
                    [&syncObj, &iterF] ()
                    {
                        for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (auto j : syncObj.load ()) {
                                iterF (j);
                            }
                        }
                    }
                };
                Thread::Start ({ adderThread, removerThread, examineThread, walkerThread });
                Thread::WaitForDone ({ adderThread, removerThread, examineThread, walkerThread });
            }
        }
        void    DoIt ()
        {
            //
            //Test to capture regression caused by incomplete fix in
            //      https://stroika.atlassian.net/browse/STK-525 -- qContainersPrivateSyncrhonizationPolicy_
            //
            Debug::TraceContextBumper traceCtx ("{}::Test10_MutlipleThreadsReadingOneUpdateUsingSynchonizedContainer_::DoIt ()");
            int64_t cnt {};
            Private_::TestBasics_<Sequence<int>> (
            [] (Sequence<int>* c, size_t i) { c->Append (i); },
            [] (Sequence<int>* c, size_t i) { size_t n = c->GetLength (); if (n != 0) c->Remove (n / 2); },
            [] (const Sequence<int>* c) { size_t n = c->IndexOf (3); },
            [&cnt] (int v) { cnt += v; }
                                              );
            Private_::TestBasics_<Set<int>> (
            [] (Set<int>* c, size_t i) { c->Add (i); },
            [] (Set<int>* c, size_t i) { c->Remove (i); },
            [] (const Set<int>* c) { bool b = c->Contains (5); },
            [&cnt] (int v) { cnt += v; }
                                         );
            Private_::TestBasics_<Mapping<int, Time::DateTime>> (
            [] (Mapping<int, Time::DateTime>* c, size_t i) { c->Add (i, Time::DateTime::Now ()); },
            [] (Mapping<int, Time::DateTime>* c, size_t i) { c->Remove (i); },
            [] (const Mapping<int, Time::DateTime>* c) { bool b = c->ContainsKey (5); },
            [&cnt] (KeyValuePair<int, Time::DateTime> v) { cnt += v.fKey; }
                    );
        }
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
#if     qStroika_Foundation_Exection_Thread_SupportThreadStatistics
        auto&& cleanupReport  =   Execution::Finally ([] () {
            auto runningThreads =   Execution::Thread::GetStatistics ().fRunningThreads;
            DbgTrace (L"Total Running threads at end: %d", runningThreads.size ());
            for (Execution::Thread::IDType threadID : runningThreads) {
                DbgTrace (L"Exiting main with thread %s running", Characters::ToString (threadID).c_str ());
            }
            VerifyTestResult (runningThreads.size () == 0);
        });
#endif
        AssignAndIterateAtSameTimeTest_1_::DoIt ();
        IterateWhileMutatingContainer_Test_2_::DoIt ();
        Test3_SynchronizedOptional_::DoIt_ ();
        Test4_CvtOp_BehaviorNeededforSyncronize_::DoIt ();
        Test5_SetSpecificSyncMethods::DoIt ();
        Test6_OverloadsWithSyncMethods_::DoIt ();
        Test7_Synchronized_::DoIt ();
        Test8_AssertExternallySynchronized_::DoIt ();
        Test9_MutlipleThreadsReadingUnsynchonizedContainer_::DoIt ();
        //constexpr bool kIsEnabled_ { !qStroika_FeatureSupported_Valgrind };
        constexpr bool kIsEnabled_ { true };
        if (kIsEnabled_) {
            Test10_MutlipleThreadsReadingOneUpdateUsingSynchonizedContainer_::DoIt ();
        }
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
