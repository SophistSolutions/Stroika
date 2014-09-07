/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Characters;
using   namespace   Containers;

using   Execution::Thread;
using   Execution::WaitableEvent;

template    <typename T>
using   Synchronized    =   Execution::Synchronized<T>;





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
    Thread  mkIterateOverThread_ (ITERABLE_TYPE* iterable, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        using ElementType   =   typename ITERABLE_TYPE::ElementType;
        return Thread ([iterable, lock, repeatCount] () {
            Debug::TraceContextBumper traceCtx (SDKSTR ("{}IterateOverThread::MAIN..."));
            for (unsigned int i = 0; i < repeatCount; ++i) {
                //DbgTrace ("Iterate thread loop %d", i);
                lock_guard<decltype(*lock)> critSec (*lock);
                for (ElementType e :  *iterable) {
                    ElementType e2 = e; // do something
                }
            }
        });
    };

}



namespace {

    template <typename ITERABLE_TYPE, typename LOCK_TYPE>
    Thread  mkOverwriteThread_ (ITERABLE_TYPE* oneToKeepOverwriting, ITERABLE_TYPE elt1, ITERABLE_TYPE elt2, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        return Thread ([oneToKeepOverwriting, lock, repeatCount, elt1, elt2] () {
            Debug::TraceContextBumper traceCtx (SDKSTR ("{}OverwriteThread::MAIN..."));
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
            Debug::TraceContextBumper traceCtx (SDKSTR ("{}::AssignAndIterateAtSameTimeTest_1_::DoIt::DoItOnce_ ()"));
            no_lock_ lock ;
            //mutex lock;
            Synchronized<ITERABLE_TYPE>  oneToKeepOverwriting { elt1 };
            Thread  iterateThread   =   mkIterateOverThread_ (&oneToKeepOverwriting, &lock, repeatCount);
            Thread  overwriteThread =   mkOverwriteThread_ (&oneToKeepOverwriting, Synchronized<ITERABLE_TYPE> (elt1), Synchronized<ITERABLE_TYPE> (elt2), &lock, repeatCount);
            RunThreads_ ({iterateThread, overwriteThread});
        }
        void    DoIt ()
        {
            Debug::TraceContextBumper traceCtx (SDKSTR ("AssignAndIterateAtSameTimeTest_1_::DoIt ()"));
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
                Debug::TraceContextBumper traceCtx (SDKSTR ("{}::MutateFunction ()"));
                DbgTrace ("(type %s)", typeid (ITERABLE_TYPE).name());
                for (unsigned int i = 0; i < repeatCount; ++i) {
                    baseMutateFunction (&oneToKeepOverwriting);
                }
            };
            Thread  iterateThread   =   mkIterateOverThread_ (&oneToKeepOverwriting, lock, repeatCount);
            Thread  mutateThread =   mutateFunction;
            RunThreads_ ({iterateThread, mutateThread});
        }
        void    DoIt ()
        {
            // This test demonstrates the need for qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
            Debug::TraceContextBumper traceCtx (SDKSTR ("IterateWhileMutatingContainer_Test_2_::DoIt ()"));

            const unsigned int kRepeatCount_ = 250;

            static const initializer_list<int>  kOrigValueInit_ = {1, 3, 4, 5, 6, 33, 12, 13};
            static const initializer_list<int>  kUpdateValueInit_ = {4, 5, 6, 33, 12, 34, 596, 13, 1, 3, 99, 33, 4, 5};

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
    namespace Test3_SynchonizedOptional_ {
        void    DoIt_ ()
        {
            using   namespace   Memory;
            try {
                Synchronized<Optional<int>> sharedValue { 0 };
                static  const int kMaxVal_ = 100000;
                Thread  reader = [&sharedValue] () {
                    while (sharedValue < kMaxVal_) {
                        VerifyTestResult (sharedValue <= kMaxVal_);
                    }
                    VerifyTestResult (sharedValue == kMaxVal_);
                };
                Thread  adder = [&sharedValue] () {
                    while (*sharedValue < kMaxVal_) {
                        sharedValue = *sharedValue + 1;
                    }
                    VerifyTestResult (sharedValue == kMaxVal_);
                };
                reader.Start ();
                adder.Start ();
                Execution::Finally cleanup { [reader, adder] () mutable {
                        reader.AbortAndWaitForDone ();
                        adder.AbortAndWaitForDone ();
                    }
                };
                // wait long time cuz of debuggers etc
                adder.WaitForDone(100);
                reader.WaitForDone(100);
                VerifyTestResult (sharedValue == kMaxVal_);
            }
            catch (...) {
                VerifyTestResult (false);
            }
        }
    }
}






namespace   {
    void    DoRegressionTests_ ()
    {
        AssignAndIterateAtSameTimeTest_1_::DoIt ();
        IterateWhileMutatingContainer_Test_2_::DoIt ();
        Test3_SynchonizedOptional_::DoIt_ ();
    }
}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
