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

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Characters;
using   namespace   Containers;

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
    Thread  mkIterateOverThread_ (ITERABLE_TYPE* iterable, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        using ElementType   =   typename ITERABLE_TYPE::ElementType;
        return Thread ([iterable, lock, repeatCount] () {
            for (unsigned int i = 0; i < repeatCount; ++i) {
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
            no_lock_ lock ;
            //mutex lock;
            ITERABLE_TYPE   oneToKeepOverwriting = elt1;
            Thread  iterateThread   =   mkIterateOverThread_ (&oneToKeepOverwriting, &lock, repeatCount);
            Thread  overwriteThread =   mkOverwriteThread_ (&oneToKeepOverwriting, elt1, elt2, &lock, repeatCount);
            RunThreads_ ({iterateThread, overwriteThread});
        }
        void    DoIt ()
        {
            static const initializer_list<int>  kOrigValueInit_ = {1, 3, 4, 5, 6, 33, 12, 13};
            static const initializer_list<int>  kUpdateValueInit_ = {4, 5, 6, 33, 12, 34, 596, 13, 1, 3, 99, 33, 4, 5};
            static const initializer_list<pair<int, int>>  kOrigPairValueInit_ = {pair<int, int> (1, 3), pair<int, int> (4, 5), pair<int, int> (6, 33), pair<int, int> (12, 13)};
            static const initializer_list<pair<int, int>>  kUPairpdateValueInit_ = {pair<int, int> (4, 5), pair<int, int> (6, 33), pair<int, int> (12, 34), pair<int, int> (596, 13), pair<int, int> (1, 3), pair<int, int> (99, 33), pair<int, int> (4, 5)};
            Debug::TraceContextBumper traceCtx (SDKSTR ("AssignAndIterateAtSameTimeTest_1_::DoIt ()"));
            DoItOnce_<String> (String (L"123456789"), String (L"abcdedfghijkqlmopqrstuvwxyz"), 1000);
            DoItOnce_<Bijection<int, int>> (Bijection<int, int> (kOrigPairValueInit_), Bijection<int, int> (kUPairpdateValueInit_), 1000);
            DoItOnce_<Collection<int>> (Collection<int> (kOrigValueInit_), Collection<int> (kUpdateValueInit_), 1000);
            // Queue/Deque NYI here cuz of assign from initializer
            //DoItOnce_<Deque<int>> (Deque<int> (kOrigValueInit_), Deque<int> (kUpdateValueInit_), 1000);
            //DoItOnce_<Queue<int>> (Queue<int> (kOrigValueInit_), Queue<int> (kUpdateValueInit_), 1000);
            DoItOnce_<MultiSet<int>> (MultiSet<int> (kOrigValueInit_), MultiSet<int> (kUpdateValueInit_), 1000);
            DoItOnce_<Mapping<int, int>> (Mapping<int, int> (kOrigPairValueInit_), Mapping<int, int> (kUPairpdateValueInit_), 1000);
            DoItOnce_<Sequence<int>> (Sequence<int> (kOrigValueInit_), Sequence<int> (kUpdateValueInit_), 1000);
            DoItOnce_<Set<int>> (Set<int> (kOrigValueInit_), Set<int> (kUpdateValueInit_), 1000);
            DoItOnce_<SortedMapping<int, int>> (SortedMapping<int, int> (kOrigPairValueInit_), SortedMapping<int, int> (kUPairpdateValueInit_), 1000);
            DoItOnce_<SortedMultiSet<int>> (SortedMultiSet<int> (kOrigValueInit_), SortedMultiSet<int> (kUpdateValueInit_), 1000);
            DoItOnce_<SortedSet<int>> (SortedSet<int> (kOrigValueInit_), SortedSet<int> (kUpdateValueInit_), 1000);
            // Stack NYI cuz not enough of stack implemented (op=)
            //DoItOnce_<Stack<int>> (Stack<int> (kOrigValueInit_), Stack<int> (kUpdateValueInit_), 1000);
        }
    }
}










namespace   {
    namespace IterateWhileMutatingContainer_Test_2_ {
        template    <typename ITERABLE_TYPE, typename LOCK, typename MUTATE_FUNCTION>
        void    DoItOnce_ (LOCK* lock, ITERABLE_TYPE elt1, unsigned int repeatCount, MUTATE_FUNCTION baseMutateFunction)
        {
            ITERABLE_TYPE   oneToKeepOverwriting = elt1;
            auto mutateFunction =               [&oneToKeepOverwriting, lock, repeatCount, &baseMutateFunction] () {
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
            // This test demonstrates the need for qStroika_Foundation_Traveral_IteratorHoldsSharedPtr_
            Debug::TraceContextBumper traceCtx (SDKSTR ("IterateWhileMutatingContainer_Test_2_::DoIt ()"));

            // @TODO - DEBUG!!!!

            no_lock_ lock;
            //mutex lock;
#if 0
            // doesnt work with 'no_lock' but does with mutext lock
            // @todo - FIX - cuz of RACE CONDITION STILL
            DoItOnce_<Set<int>> (
                                 &lock,
                                 Set<int> ({1, 3, 4, 5, 6, 33, 12, 13}),
                                 1000,
            [&lock] (Set<int>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = Set<int> {3, 5};
                    }
                    else {
                        lock_guard<decltype(lock)> critSec (lock);
                        (*oneToKeepOverwriting) = Set<int> {3, 5};
                    }
                }
            });
#endif
            DoItOnce_<String> (
                &lock,
                String (L"123456789"),
                1000,
            [&lock] (String * oneToKeepOverwriting) {
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











namespace   {
    void    DoRegressionTests_ ()
    {
        AssignAndIterateAtSameTimeTest_1_::DoIt ();
        IterateWhileMutatingContainer_Test_2_::DoIt ();
    }
}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
