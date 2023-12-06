/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Execution::ThreadSafetyBuiltinObject
#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Cache/SynchronizedCallerStalenessCache.h"
#include "Stroika/Foundation/Cache/SynchronizedLRUCache.h"
#include "Stroika/Foundation/Cache/SynchronizedTimedCache.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Deque.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/MultiSet.h"
#include "Stroika/Foundation/Containers/Queue.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Containers/SortedCollection.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Containers/SortedMultiSet.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/Containers/Stack.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "../TestHarness/NotCopyable.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Characters;
using namespace Containers;
using namespace Execution;

using namespace Stroika::TestHarness;

namespace {
    bool kVerySlow_  = qDebug and (Debug::IsRunningUnderValgrind () or Debug::kBuiltWithThreadSanitizer);
    bool kSortaSlow_ = qDebug or Debug::IsRunningUnderValgrind () or Debug::kBuiltWithThreadSanitizer;
}

namespace {
    /*
     *  To REALLY this this code for thread-safety, use ExternallySynchronizedLock, but to verify it works
     *  without worrying about races, just use mutex.
     */
    struct no_lock_ {
        void lock ()
        {
        }
        void unlock ()
        {
        }
    };
}

namespace {
    template <typename ITERABLE_TYPE, typename LOCK_TYPE>
    Thread::Ptr mkIterateOverThread_ (Synchronized<ITERABLE_TYPE>* iterable, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        using value_type = typename ITERABLE_TYPE::value_type;
        return Thread::New ([iterable, lock, repeatCount] () {
            Debug::TraceContextBumper traceCtx{"{}IterateOverThread::MAIN..."};
            for (unsigned int i = 0; i < repeatCount; ++i) {
                //DbgTrace ("Iterate thread loop %d", i);
                lock_guard<decltype (*lock)> critSec{*lock};
                for (value_type e : iterable->load ()) {
                    [[maybe_unused]] value_type e2 = e; // do something
                }
            }
        });
    };
}

namespace {
    template <typename ITERABLE_TYPE, typename ITERABLE_TYPE2, typename LOCK_TYPE>
    Thread::Ptr mkOverwriteThread_ (ITERABLE_TYPE* oneToKeepOverwriting, ITERABLE_TYPE2 elt1, ITERABLE_TYPE2 elt2, LOCK_TYPE* lock, unsigned int repeatCount)
    {
        return Thread::New ([oneToKeepOverwriting, lock, repeatCount, elt1, elt2] () {
            Debug::TraceContextBumper traceCtx{"{}OverwriteThread::MAIN..."};
            for (unsigned int i = 0; i < repeatCount; ++i) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype (*lock)> critSec{*lock};
                        (*oneToKeepOverwriting) = elt1;
                    }
                    else {
                        lock_guard<decltype (*lock)> critSec{*lock};
                        (*oneToKeepOverwriting) = elt2;
                    }
                }
            }
        });
    };
}

namespace {
    namespace AssignAndIterateAtSameTimeTest_1_ {
        template <typename ITERABLE_TYPE>
        void DoItOnce_ (ITERABLE_TYPE elt1, ITERABLE_TYPE elt2, unsigned int repeatCount)
        {
            Debug::TraceContextBumper traceCtx{"{}::AssignAndIterateAtSameTimeTest_1_::DoIt::DoItOnce_ ()"};
            no_lock_                  lock;
            //mutex lock;
            Synchronized<ITERABLE_TYPE> oneToKeepOverwriting{elt1};
            Thread::Ptr                 iterateThread   = mkIterateOverThread_ (&oneToKeepOverwriting, &lock, repeatCount);
            Thread::Ptr                 overwriteThread = mkOverwriteThread_ (&oneToKeepOverwriting, elt1, elt2, &lock, repeatCount);
            Thread::Start ({iterateThread, overwriteThread});
            Thread::WaitForDone ({iterateThread, overwriteThread});
        }
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"AssignAndIterateAtSameTimeTest_1_::DoIt ()"};
            static const unsigned int kRepeatCount_ = kVerySlow_ ? 100u : 500u;
            //const unsigned int kRepeatCount_ = 1;
            static const initializer_list<int>            kOrigValueInit_       = {1, 3, 4, 5, 6, 33, 12, 13};
            static const initializer_list<int>            kUpdateValueInit_     = {4, 5, 6, 33, 12, 34, 596, 13, 1, 3, 99, 33, 4, 5};
            static const initializer_list<pair<int, int>> kOrigPairValueInit_   = {pair<int, int> (1, 3), pair<int, int> (4, 5),
                                                                                   pair<int, int> (6, 33), pair<int, int> (12, 13)};
            static const initializer_list<pair<int, int>> kUPairpdateValueInit_ = {
                pair<int, int> (4, 5), pair<int, int> (6, 33),  pair<int, int> (12, 34), pair<int, int> (596, 13),
                pair<int, int> (1, 3), pair<int, int> (99, 35), pair<int, int> (4, 5)};
            DoItOnce_<String> (L"123456789"_k, L"abcdedfghijkqlmopqrstuvwxyz", kRepeatCount_);
            DoItOnce_<Bijection<int, int>> (Bijection<int, int> (kOrigPairValueInit_), Bijection<int, int>{kUPairpdateValueInit_}, kRepeatCount_);
            DoItOnce_<Collection<int>> (Collection<int>{kOrigValueInit_}, Collection<int>{kUpdateValueInit_}, kRepeatCount_);
            // Queue/Deque NYI here cuz of assign from initializer
            //DoItOnce_<Deque<int>> (Deque<int> (kOrigValueInit_), Deque<int> (kUpdateValueInit_), kRepeatCount_);
            //DoItOnce_<Queue<int>> (Queue<int> (kOrigValueInit_), Queue<int> (kUpdateValueInit_), kRepeatCount_);
            DoItOnce_<MultiSet<int>> (MultiSet<int>{kOrigValueInit_}, MultiSet<int>{kUpdateValueInit_}, kRepeatCount_);
            DoItOnce_<Mapping<int, int>> (Mapping<int, int>{kOrigPairValueInit_}, Mapping<int, int>{kUPairpdateValueInit_}, kRepeatCount_);
            DoItOnce_<Sequence<int>> (Sequence<int>{kOrigValueInit_}, Sequence<int>{kUpdateValueInit_}, kRepeatCount_);
            DoItOnce_<Set<int>> (Set<int> (kOrigValueInit_), Set<int>{kUpdateValueInit_}, kRepeatCount_);
            DoItOnce_<SortedCollection<int>> (SortedCollection<int>{kOrigValueInit_}, SortedCollection<int>{kUpdateValueInit_}, kRepeatCount_);
            DoItOnce_<SortedMapping<int, int>> (SortedMapping<int, int>{kOrigPairValueInit_}, SortedMapping<int, int>{kUPairpdateValueInit_}, kRepeatCount_);
            DoItOnce_<SortedMultiSet<int>> (SortedMultiSet<int>{kOrigValueInit_}, SortedMultiSet<int>{kUpdateValueInit_}, kRepeatCount_);
            DoItOnce_<SortedSet<int>> (SortedSet<int>{kOrigValueInit_}, SortedSet<int>{kUpdateValueInit_}, kRepeatCount_);

            // Stack NYI cuz not enough of stack implemented (op=)
            //DoItOnce_<Stack<int>> (Stack<int> (kOrigValueInit_), Stack<int> (kUpdateValueInit_), kRepeatCount_);
        }
    }
}

namespace {
    namespace IterateWhileMutatingContainer_Test_2_ {
        template <typename ITERABLE_TYPE, typename LOCK, typename MUTATE_FUNCTION>
        void DoItOnce_ (LOCK* lock, ITERABLE_TYPE elt1, unsigned int repeatCount, MUTATE_FUNCTION baseMutateFunction)
        {
            Synchronized<ITERABLE_TYPE> oneToKeepOverwriting{elt1};
            auto                        mutateFunction = [&oneToKeepOverwriting, repeatCount, &baseMutateFunction] () {
                Debug::TraceContextBumper traceCtx{"{}::MutateFunction ()"};
                DbgTrace ("(type %s)", typeid (ITERABLE_TYPE).name ());
                for (unsigned int i = 0; i < repeatCount; ++i) {
                    baseMutateFunction (&oneToKeepOverwriting);
                }
            };
            Thread::Ptr iterateThread{mkIterateOverThread_ (&oneToKeepOverwriting, lock, repeatCount)};
            Thread::Ptr mutateThread{Thread::New (mutateFunction)};
            Thread::Start ({iterateThread, mutateThread});
            Thread::WaitForDone ({iterateThread, mutateThread});
        }
        void DoIt ()
        {
            // This test (used to) demonstrate the need for qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
            // but been fixed
            Debug::TraceContextBumper traceCtx{"IterateWhileMutatingContainer_Test_2_::DoIt ()"};

            const unsigned int kRepeatCount_ = kVerySlow_ ? 20 : 250;

            static constexpr initializer_list<int> kOrigValueInit_   = {1, 3, 4, 5, 6, 33, 12, 13};
            static constexpr initializer_list<int> kUpdateValueInit_ = {4, 5, 6, 33, 12, 34, 596, 13, 1, 3, 99, 33, 4, 5};

            no_lock_ lock;
            //mutex lock;

            DoItOnce_<Set<int>> (&lock, Set<int> (kOrigValueInit_), kRepeatCount_, [&lock] (Synchronized<Set<int>>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    //DbgTrace ("doing update loop %d", ii);
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype (lock)> critSec{lock};
                        (*oneToKeepOverwriting) = Set<int>{kUpdateValueInit_};
                    }
                    else {
                        lock_guard<decltype (lock)> critSec{lock};
                        (*oneToKeepOverwriting) = Set<int>{kUpdateValueInit_};
                    }
                }
            });

            DoItOnce_<Sequence<int>> (&lock, Sequence<int>{kOrigValueInit_}, kRepeatCount_, [&lock] (Synchronized<Sequence<int>>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype (lock)> critSec{lock};
                        (*oneToKeepOverwriting) = Sequence<int>{kUpdateValueInit_};
                    }
                    else {
                        lock_guard<decltype (lock)> critSec{lock};
                        (*oneToKeepOverwriting) = Sequence<int>{kUpdateValueInit_};
                    }
                }
            });

            DoItOnce_<String> (&lock, L"123456789", kRepeatCount_, [&lock] (Synchronized<String>* oneToKeepOverwriting) {
                for (int ii = 0; ii <= 100; ++ii) {
                    if (Math::IsOdd (ii)) {
                        lock_guard<decltype (lock)> critSec{lock};
                        (*oneToKeepOverwriting) = L"abc123"_k;
                    }
                    else {
                        lock_guard<decltype (lock)> critSec{lock};
                        (*oneToKeepOverwriting) = L"123abc"_k;
                    }
                }
            });
        }
    }
}

namespace {
    namespace Test3_SynchronizedOptional_ {
        void DoIt_ ()
        {
            Debug::TraceContextBumper traceCtx{"{}::Test3_SynchronizedOptional_::DoIt ()"};
            using namespace Memory;
            try {
                Synchronized<optional<int>> sharedValue{0};
                static const bool           kRunningValgrind_ = Debug::IsRunningUnderValgrind ();
                static const int            kMaxVal_          = kVerySlow_ ? 5 : 100000;
                Thread::Ptr                 reader            = Thread::New ([&sharedValue] () {
                    optional<int> prevValue;
                    unsigned int  repeatCount{};
                    while ((prevValue = sharedValue.load ()) < kMaxVal_) {
                        VerifyTestResult (sharedValue.load () <= kMaxVal_);
                        if (kRunningValgrind_) {
                            if (prevValue == sharedValue.load ()) {
                                repeatCount++;
                                if (repeatCount > 100) {
                                    Execution::Sleep (1ms); // avoid starvation under helgrind (seems to help) -- LGP 2017-12-20.
                                    repeatCount = 0;
                                }
                            }
                            else {
                                prevValue = nullopt;
                            }
                        }
                    }
                    VerifyTestResult (sharedValue.load () == kMaxVal_);
                });
                Thread::Ptr                 adder             = Thread::New ([&sharedValue] () {
                    while (sharedValue.load () < kMaxVal_) {
                        sharedValue.store (*sharedValue.load () + 1);
                    }
                    VerifyTestResult (sharedValue.load () == kMaxVal_);
                });
                Thread::Start ({reader, adder});
                [[maybe_unused]] auto&& cleanup = Execution::Finally ([&reader, &adder] () noexcept {
                    Thread::AbortAndWaitForDone ({reader, adder});
                });
                // wait long time cuz of debuggers (esp valgrind) etc
                Thread::WaitForDone ({reader, adder}, 15 * 60s);
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
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"{}::Test4_CvtOp_BehaviorNeededforSyncronize_::DoIt ()"};
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
    namespace Test5_SetSpecificSyncMethods {
        void DoIt ()
        {
            Debug::TraceContextBumper           traceCtx{"{}::Test5_SetSpecificSyncMethods::DoIt ()"};
            Set<int>                            sensorsToActuallyRead{2, 3};
            static const Synchronized<Set<int>> kACUSensors_{Set<int>{1, 2}};
            Set<int>                            acufpgaSensors1 = kACUSensors_ ^ sensorsToActuallyRead;
            Set<int>                            acufpgaSensors2 = sensorsToActuallyRead ^ kACUSensors_;
            VerifyTestResult ((acufpgaSensors1 == Set<int>{2}));
            VerifyTestResult ((acufpgaSensors2 == Set<int>{2}));
        }
    }
}

namespace {
    namespace Test6_OverloadsWithSyncMethods_ {
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"{}::Test6_OverloadsWithSyncMethods_::DoIt ()"};
            String                    xx;
            Synchronized<String>      yy;
            if (xx != yy) {
            }
            optional<String> xxo;
#if 0
            //////@todo FIX various operator overloads so this works more seemlessly. See
            //      template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = typename enable_if<is_constructible<optional<T, TRAITS>, RHS_CONVERTABLE_TO_OPTIONAL>::value >::type>
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
    namespace Test7_Synchronized_ {
        namespace Private_ {
            void TestBasics_ ()
            {
                using namespace Execution;
                {
                    Synchronized<int> tmp;
                    tmp = 4;
                    int a{tmp};
                    VerifyTestResult (a == 4);
                }
                {
                    Synchronized<int> tmp{4};
                    int               a{tmp};
                    VerifyTestResult (a == 4);
                }
                {
                    Synchronized<String> tmp{L"x"};
                    String               a{tmp};
                    VerifyTestResult (a == L"x");
                    VerifyTestResult (tmp.cget ()->find ('z') == string::npos);
                    VerifyTestResult (tmp.cget ()->find ('x') == 0);
                }
                {
                    static Synchronized<String> tmp{L"x"};
                    [[maybe_unused]] auto&&     critSec = lock_guard{tmp}; // make sure explicit locks work too
                    String                      a{tmp};
                    VerifyTestResult (a == L"x");
                    VerifyTestResult (tmp.cget ()->find ('z') == string::npos);
                    VerifyTestResult (tmp.cget ()->find ('x') == 0);
                }
            }
            template <typename INTISH_TYPE>
            void DoInterlocktest_ (function<void (INTISH_TYPE*)> increment, function<void (INTISH_TYPE*)> decrement)
            {
                using namespace Execution;
                static constexpr int kMaxTimes_  = 100 * 1000;
                INTISH_TYPE          s           = 0;
                Thread::Ptr          incrementer = Thread::New ([&s, increment] () {
                    for (int i = 0; i < kMaxTimes_; ++i) {
                        increment (&s);
                    }
                });
                Thread::Ptr          decrementer = Thread::New ([&s, decrement] () {
                    for (int i = 0; i < kMaxTimes_; ++i) {
                        decrement (&s);
                    }
                });
                incrementer.Start ();
                decrementer.Start ();
                incrementer.WaitForDone ();
                decrementer.WaitForDone ();
                VerifyTestResult (s == INTISH_TYPE (0));
            }
            void DoThreadTest_ ()
            {
                using namespace Execution;
                if (false) {
                    // Fails cuz no synchonization
                    DoInterlocktest_<int> ([] (int* i) { (*i)++; }, [] (int* i) { (*i)--; });
                }
                struct intish_object1 {
                    int fVal;
                    intish_object1 (int i)
                        : fVal{i}
                    {
                    }
                    bool operator== (const intish_object1& rhs) const
                    {
                        return rhs.fVal == fVal;
                    }
                };
                if (false) {
                    // Fails cuz no synchonization
                    DoInterlocktest_<intish_object1> ([] (intish_object1* i) { (i->fVal)++; }, [] (intish_object1* i) { (i->fVal)--; });
                }
                DoInterlocktest_<Synchronized<intish_object1>> ([] (Synchronized<intish_object1>* i) { (i->rwget ()->fVal)++; },
                                                                [] (Synchronized<intish_object1>* i) { (i->rwget ()->fVal)--; });
            }
            void TestSynchronizedNotCopyable_ ()
            {
                {
                    Synchronized<NotCopyable> x;
                    x.store (NotCopyable{});
                    x.rwget ().rwref ().method ();
                    x.cget ().cref ().const_method ();
                }
                {
                    Synchronized<unique_ptr<NotCopyable>> x;
                    x.store (make_unique<NotCopyable> ());
                    x.rwget ().rwref ()->method ();
                    x.cget ().cref ()->const_method ();
                }
                {
                    Synchronized<optional<NotCopyable>> x;
                    x.store (optional<NotCopyable>{NotCopyable{}});
                    x.rwget ().rwref ()->method ();
                    x.cget ().cref ()->const_method ();
                }
                {
                    Synchronized<optional<NotCopyable>> x;
                    auto                                l = x.rwget ();
                    l.store ({});
                }
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"{}::Test7_Synchronized_::DoIt ()"};
            Private_::TestBasics_ ();
            Private_::DoThreadTest_ ();
            Private_::TestSynchronizedNotCopyable_ ();
        }
    }
}

namespace {
    namespace Test8_AssertExternallySynchronized_ {
        namespace Private_ {
            void TestBasics_ ()
            {
                struct A {
                    int x;
                };
                struct APrime : Debug::AssertExternallySynchronizedMutex {
                    int x;
                };
#if !qDebug
                VerifyTestResult (sizeof (A) == sizeof (APrime));
#endif
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"{}::Test8_AssertExternallySynchronized_::DoIt ()"};
            Private_::TestBasics_ ();
        }
    }
}

namespace {
    namespace Test9_MutlipleThreadsReadingUnsynchronizedContainer_ {
        namespace Private_ {
            void TestBasics_ ()
            {
                static constexpr size_t kIOverallRepeatCount_{(qDebug or qStroika_FeatureSupported_Valgrind) ? 50 : 1000}; // tweak count cuz too slow
                Sequence<int> tmp{Traversal::DiscreteRange<int>{1, 1000}};
                Thread::Ptr   t1 = Thread::New ([&tmp] () {
                    for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                        for (int j : tmp) {
                            VerifyTestResult (1 <= j and j <= 1000);
                        }
                    }
                });
                Thread::Ptr   t2 = Thread::New ([&tmp] () {
                    for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                        for (int j : tmp) {
                            VerifyTestResult (1 <= j and j <= 1000);
                        }
                    }
                });
                Thread::Ptr   t3 = Thread::New ([&tmp] () {
                    for (int i = 1; i < kIOverallRepeatCount_; ++i) {
                        if (tmp.size () == 1000) {
                            VerifyTestResult (tmp.IndexOf (6) == 5u);
                            VerifyTestResult (*tmp.First () == 1);
                            VerifyTestResult (*tmp.Last () == 1000);
                        }
                    }
                });
                Thread::Start ({t1, t2, t3});
                Thread::WaitForDone ({t1, t2, t3});
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"{}::Test9_MutlipleThreadsReadingUnsynchronizedContainer_::DoIt ()"};
            Private_::TestBasics_ ();
        }
    }
}

namespace {
    namespace Test10_MutlipleThreadsReadingOneUpdateUsingSynchronizedContainer_ {
        namespace Private_ {
            template <typename CONTAINER, typename ADD_FUNCTION, typename REMOVE_FUNCTION, typename EXAMINE_FUNCTION, typename ITER_FUNCTION>
            void TestBasics_ (ADD_FUNCTION addF, REMOVE_FUNCTION remF, EXAMINE_FUNCTION examineF, ITER_FUNCTION iterF)
            {
                static const size_t     kIOverallRepeatCount_              = kVerySlow_ ? 10 : (kSortaSlow_ ? 50 : 1000);
                static const int        kInnerConstantForHowMuchStuffTodo_ = kVerySlow_ ? 10 : ((qDebug or kSortaSlow_) ? 100 : 1000);
                Synchronized<CONTAINER> syncObj;
                Thread::Ptr             adderThread = Thread::New (
                    [&syncObj, &addF] () {
                        for (size_t i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (int j : Traversal::DiscreteRange<int>{1, kInnerConstantForHowMuchStuffTodo_}) {
                                addF (&syncObj.rwget ().rwref (), j);
                            }
                        }
                    },
                    L"adderThread"_k);
                Thread::Ptr removerThread = Thread::New (
                    [&syncObj, &remF] () {
                        for (size_t i = 1; i < kIOverallRepeatCount_; ++i) {
                            for (int j : Traversal::DiscreteRange<int>{1, kInnerConstantForHowMuchStuffTodo_}) {
                                remF (&syncObj.rwget ().rwref (), j);
                            }
                        }
                    },
                    L"removerThread"_k);
                Thread::Ptr examineThread = Thread::New (
                    [&syncObj, &examineF] () {
                        constexpr size_t kMultiplierCuzThisTooFast_{10};
                        for (size_t i = 1; i < kIOverallRepeatCount_ * kMultiplierCuzThisTooFast_; ++i) {
                            examineF (&syncObj.cget ().cref ());
                        }
                    },
                    L"examineThread"_k);
                Thread::Ptr walkerThread = Thread::New (
                    [&syncObj, &iterF] () {
                        constexpr size_t kMultiplierCuzThisTooFast_{7};
                        for (size_t i = 1; i < kIOverallRepeatCount_ * kMultiplierCuzThisTooFast_; ++i) {

#if 1
                            /*
                             *  SADLY, even after major cleanups to container code, I still have this bug:
                             *      https://stroika.atlassian.net/browse/STK-700
                             * 
                             *  To reproduce:
                             *      +   Use Ubuntu 2004
                             *      +   configure xxx --compiler-driver g++ --apply-default-release-flags --trace2file enable --cppstd-version c++17 --sanitize none,thread --block-allocation disable
                             *          (much of the above just to make quick repro and easier to debug)
                             *      +   Only the below case 'TestBasics_<Sequence<int>>' is needed to reproduce the bug. Any of the other
                             *          Stroika ones reproduces it, but the vector one does NOT (strongly hinting the bug is with the container code not
                             *          with the Locking code).
                             *      +   useful to add DbgTrace (L"consructing new sharedimpl for %p", fSharedImpl_.get ()); etc in SharedByValue<T, TRAITS>::BreakReferences_
                             *      +   First note - performarnce issue described below (see ANTI-PATTERN below).
                             *      +   The storm of clones appears to trigger some bug in the cloning logic with respect to threads. MAYBE because
                             *          we really do need todo some sort of atomic support for the shared_ptr stuff (though I dont think so).
                             *          Anyhow, I spent hours tracking this down and didn't solve, so just make notes for a later attempt.
                             *              --LGP 2021-11-17
                             */
                            auto HACK_WORKAROUND_SERIOUS_BUG = syncObj.cget ();
#endif

                            /**
                             *  Note that this code is something of an ANTI-PATTERN (even once we fix https://stroika.atlassian.net/browse/STK-700).
                             *  Because the syncObj.load () upps the reference count to 2 during the iteration, the other threads will then be forced
                             *  (as part of their updates) to do a storm of Clones. It would be better performance wise - typically - do do 
                             *  a scoped_lock on syncObj before the load, since that way UPDATES will BLOCK (instead of going ahead and cloning). You might
                             *  want this behavior in some cases, but mostly probably not.
                             */
                            for (auto j : syncObj.load ()) {
                                iterF (j);
                            }
                        }
                    },
                    L"walkerThread"_k);
                Thread::Start ({adderThread, removerThread, examineThread, walkerThread});
                Thread::WaitForDone ({adderThread, removerThread, examineThread, walkerThread});
            }
        }
        void DoIt ()
        {
            //
            //  If you see hang here/TSAN issue - see
            //      https://stroika.atlassian.net/browse/STK-700
            //
            Debug::TraceContextBumper traceCtx{"{}::Test10_MutlipleThreadsReadingOneUpdateUsingSynchronizedContainer_::DoIt ()"};
            int64_t                   cnt{};
            {
                Debug::TraceContextBumper ctx1{"TestBasics_<vector<int>>"};
                Private_::TestBasics_<vector<int>> ([] (vector<int>* c, int i) { c->push_back (i); },
                                                    [] (vector<int>* c, [[maybe_unused]] int i) {
                                                        size_t n = c->size ();
                                                        if (n != 0)
                                                            c->erase (c->begin () + (n / 2));
                                                    },
                                                    [] (const vector<int>* c) { (void)std::find (c->begin (), c->end (), 3); },
                                                    [&cnt] (int v) { cnt += v; });
            }
            {
                Debug::TraceContextBumper ctx1{"...TestBasics_<Sequence<int>>"};
                Private_::TestBasics_<Sequence<int>> (
                    [] (Sequence<int>* c, int i) { c->Append (i); },
                    [] (Sequence<int>* c, [[maybe_unused]] int i) {
                        size_t n = c->size ();
                        if (n != 0)
                            c->Remove (n / 2);
                    },
                    [] (const Sequence<int>* c) { [[maybe_unused]] size_t n = Memory::NullCoalesce (c->IndexOf (3)); },
                    [&cnt] (int v) { cnt += v; });
            }
            {
                Debug::TraceContextBumper ctx1{"...TestBasics_<Set<int>>"};
                Private_::TestBasics_<Set<int>> ([] (Set<int>* c, int i) { c->Add (i); }, [] (Set<int>* c, int i) { c->RemoveIf (i); },
                                                 [] (const Set<int>* c) { [[maybe_unused]] bool b = c->Contains (5); },
                                                 [&cnt] (int v) { cnt += v; });
            }
            {
                Debug::TraceContextBumper ctx1{"...TestBasics_<Mapping<int, Time::DateTime>>"};
                Private_::TestBasics_<Mapping<int, Time::DateTime>> (
                    [] (Mapping<int, Time::DateTime>* c, int i) { c->Add (i, Time::DateTime::Now ()); },
                    [] (Mapping<int, Time::DateTime>* c, int i) { c->RemoveIf (i); },
                    [] (const Mapping<int, Time::DateTime>* c) { [[maybe_unused]] bool b = c->ContainsKey (5); },
                    [&cnt] (KeyValuePair<int, Time::DateTime> v) { cnt += v.fKey; });
            }
        }
    }
}

namespace {
    namespace Test11_SynchronizedCaches_ {
        namespace Private_ {
            static const size_t kIOverallRepeatCount_ = kVerySlow_ ? 10 : kSortaSlow_ ? 50 : 1000;
            void                SyncLRUCacheT1_ ()
            {
                Debug::TraceContextBumper traceCtx{"{}SyncLRUCacheT1_..."};
                using namespace Cache;
                SynchronizedLRUCache cache{pair<string, string>{}, 10u, 10u, hash<string>{}};
                Thread::Ptr          writerThread = Thread::New (
                    [&cache] () {
                        for (size_t i = 1; i < kIOverallRepeatCount_; ++i) {
                            cache.Add ("a", "1");
                            cache.Add ("b", "2");
                            cache.Add ("c", "3");
                            cache.Add ("d", "4");
                            auto oa = cache.Lookup ("a");
                            VerifyTestResult (not oa.has_value () or oa == "1"); // could be missing or found but if found same value
                            auto ob = cache.Lookup ("b");
                            VerifyTestResult (not ob.has_value () or ob == "2"); // ""
                            auto od = cache.Lookup ("d");
                            VerifyTestResult (od == "4");
                        }
                    },
                    L"writerThread"_k);
                Thread::Ptr copierThread = Thread::New (
                    [&cache] () {
                        for (size_t i = 1; i < kIOverallRepeatCount_; ++i) {
                            {
                                auto oa = cache.Lookup ("a");
                                VerifyTestResult (not oa.has_value () or oa == "1"); // could be missing or found but if found same value
                                auto ob = cache.Lookup ("b");
                                VerifyTestResult (not ob.has_value () or ob == "2"); // ""
                                auto od = cache.Lookup ("d");
                                VerifyTestResult (not od.has_value () or od == "4"); // ""
                            }
                            SynchronizedLRUCache tmp2 = cache;
                            auto                 oa   = tmp2.Lookup ("a");
                            VerifyTestResult (not oa.has_value () or oa == "1"); // could be missing or found but if found same value
                            auto ob = tmp2.Lookup ("b");
                            VerifyTestResult (not ob.has_value () or ob == "2"); // ""
                            auto od = tmp2.Lookup ("d");
                            VerifyTestResult (not od.has_value () or od == "4"); // ""
                        }
                    },
                    L"copierThread"_k);
                Thread::Start ({writerThread, copierThread});
                Thread::WaitForDone ({writerThread, copierThread});
            }
            void SyncCallerStalenessCacheT1_ ()
            {
                Debug::TraceContextBumper traceCtx{"{}SyncCallerStalenessCacheT1_..."};
                using namespace Cache;
                SynchronizedCallerStalenessCache<int, int> cache;
                auto mapValue = [&cache] (int value, optional<Time::DurationSeconds> allowedStaleness = {}) -> int {
                    return cache.LookupValue (value, cache.Ago (allowedStaleness.value_or (30s)), [=] (int v) {
                        return v; // could be more expensive computation
                    });
                };
                Thread::Ptr writerThread = Thread::New (
                    [&mapValue] () {
                        for (size_t i = 1; i < kIOverallRepeatCount_; ++i) {
                            VerifyTestResult (mapValue (static_cast<int> (i)) == static_cast<int> (i));
                        }
                    },
                    L"writerThread"_k);
                Thread::Ptr copierThread = Thread::New (
                    [&mapValue] () {
                        for (size_t i = 1; i < kIOverallRepeatCount_; ++i) {
                            VerifyTestResult (mapValue (static_cast<int> (i)) == static_cast<int> (i));
                        }
                    },
                    L"copierThread"_k);
                Thread::Start ({writerThread, copierThread});
                Thread::WaitForDone ({writerThread, copierThread});
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper traceCtx{"{}Test11_SynchronizedCaches_..."};
            Private_::SyncLRUCacheT1_ ();
            Private_::SyncCallerStalenessCacheT1_ ();
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
        [[maybe_unused]] auto&& cleanupReport = Execution::Finally ([] () {
            auto runningThreads = Execution::Thread::GetStatistics ().fRunningThreads;
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
        Test9_MutlipleThreadsReadingUnsynchronizedContainer_::DoIt ();
        Test10_MutlipleThreadsReadingOneUpdateUsingSynchronizedContainer_::DoIt ();
        Test11_SynchronizedCaches_::DoIt ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
