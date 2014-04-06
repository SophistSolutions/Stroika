/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::ThreadSafetyBuiltinObject
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <mutex>

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Collection.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Containers/SortedSet.h"
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
            Debug::TraceContextBumper traceCtx (SDKSTR ("AssignAndIterateAtSameTimeTest_1_::DoIt ()"));
            DoItOnce_<String> (String (L"123456789"), String (L"abcdedfghijkqlmopqrstuvwxyz"), 1000);
            //DoItOnce_<Collection<int>> (Collection<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Collection<int> ({4, 5, 6, 33, 12, 13, 1, 3, 99, 33, 4, 5}), 1000);
            DoItOnce_<Sequence<int>> (Sequence<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Sequence<int> ({4, 5, 6, 33, 12, 13, 1, 3, 99, 33, 4, 5}), 1000);
            DoItOnce_<Set<int>> (Set<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Set<int> ({4, 5, 6, 33, 12, 13, 34, 388, 3, 99, 33, 4, 5}), 1000);
            DoItOnce_<SortedSet<int>> (SortedSet<int> ({1, 3, 4, 5, 6, 33, 12, 13}), SortedSet<int> ({4, 5, 6, 33, 12, 13, 34, 388, 3, 99, 33, 4, 5}), 1000);
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

            //no_lock_ lock;
            mutex lock;
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
