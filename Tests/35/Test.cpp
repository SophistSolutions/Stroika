/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::ThreadSafetyBuiltinObject
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <mutex>

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Execution/Thread.h"
#include    "Stroika/Foundation/Math/Common.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Characters;
using   namespace   Containers;

using   Execution::Thread;
using   Execution::WaitableEvent;




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
    namespace AssignAndIterateAtSameTimeTest_ {

        template    <typename ITERABLE_TYPE>
        void    DoIt_ (ITERABLE_TYPE elt1, ITERABLE_TYPE elt2, unsigned int repeatCount)
        {
            /*
             *  To REALLY this this code for thread-safety, use ExternallySynchronizedLock, but to verify it works
             *  without worrying about races, just use mutex.
             */
            struct no_lock_ {
                void lock () {}
                void unlock () {}
            };
            no_lock_ lock ;
            //mutex lock;

            ITERABLE_TYPE   oneToKeepOverwriting = elt1;
            Thread  iterateThread   =   mkIterateOverThread_ (&oneToKeepOverwriting, &lock, repeatCount);
            Thread  overwriteThread =   mkOverwriteThread_ (&oneToKeepOverwriting, elt1, elt2, &lock, repeatCount);
            iterateThread.Start ();
            overwriteThread.Start ();
            iterateThread.WaitForDone ();
            overwriteThread.WaitForDone ();
        }
    }


    void    AssignAndIterateAtSameTimeTest_1_ ()
    {
        Debug::TraceContextBumper traceCtx (SDKSTR ("AssignAndIterateAtSameTimeTest_1_"));
        AssignAndIterateAtSameTimeTest_::DoIt_<String> (String (L"123456789"), String (L"abcdedfghijkqlmopqrstuvwxyz"), 500);
        AssignAndIterateAtSameTimeTest_::DoIt_<Sequence<int>> (Sequence<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Sequence<int> ({4, 5, 6, 33, 12, 13, 1, 3, 99, 33, 4, 5}), 500);
        AssignAndIterateAtSameTimeTest_::DoIt_<Set<int>> (Set<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Set<int> ({4, 5, 6, 33, 12, 13, 34, 388, 3, 99, 33, 4, 5}), 500);
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
        AssignAndIterateAtSameTimeTest_1_ ();
    }
}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
