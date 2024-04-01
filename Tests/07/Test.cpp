/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::LockFree
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Sleep.h"

#include "Stroika/Foundation/Containers/LockFreeDataStructures/forward_list.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::LockFreeDataStructures;

using namespace Stroika::Frameworks;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {
    double sTimeMultiplier_ =
        (qDebug ? 0.2 : 1) * ((Debug::IsRunningUnderValgrind () or Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer) ? .1 : 1.0);
}

#if qHasFeature_GoogleTest
namespace {

    namespace forward_list_tests_ {
        using Containers::LockFreeDataStructures::forward_list;

        void test_01 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_01"};
            forward_list<int>         a;
        }

        void test_02 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_02"};
            forward_list<int>         a;
            a.push_front (2);
            EXPECT_TRUE (a.pop_front () == 2);
            EXPECT_TRUE (a.empty ());
        }

        void test_03 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_03"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (5);
            EXPECT_TRUE (a.pop_front () == 5);
            EXPECT_TRUE (a.pop_front () == 2);
            EXPECT_TRUE (a.empty ());
        }

        void test_04 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_04"};
            forward_list<int>         a;
            std::vector<std::thread>  threads;
            int                       threadCount           = 5;
            int                       perThreadElementCount = 1000;
            for (int i = 0; i < threadCount; i++) {
                threads.emplace_back ([&] () {
                    for (int j = 0; j < perThreadElementCount; j++) {
                        a.push_front (j);
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            int totalElementCount = perThreadElementCount * threadCount;
            for (int k = 0; k < totalElementCount; k++) {
                EXPECT_TRUE (a.pop_front ().has_value ());
            }
            EXPECT_TRUE (a.empty ());
        }

        void test_05 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_05"};
            forward_list<int>         a;
            std::vector<std::thread>  threads;
            for (int i = 0; i < 5; i++) {
                threads.emplace_back ([&a] () {
                    for (int j = 0; j < 1000; j++) {
                        int y = rand ();
                        a.push_front (y);
                        std::this_thread::sleep_for (std::chrono::microseconds (rand () % 10));
                        [[maybe_unused]] optional<int> x = a.pop_front ();
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        if (x == y) {
                            DbgTrace ("y"_f);
                        }
                        else {
                            DbgTrace ("n"_f);
                        }
#endif
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            EXPECT_TRUE (a.empty ());
        }

        void test_06 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_06"};
            forward_list<int>         a;
            std::vector<std::thread>  threads;
            int                       threadCount           = 5;
            int                       perThreadElementCount = 1000;
            for (int i = 0; i < threadCount; i++) {
                threads.emplace_back ([&a, i, perThreadElementCount] () {
                    for (int j = 0; j < perThreadElementCount; j++) {
                        a.push_front (j + i * perThreadElementCount);
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            std::set<int> remainingNumbers;
            int           totalElementCount = perThreadElementCount * threadCount;
            for (int k = 0; k < totalElementCount; k++) {
                remainingNumbers.insert (k);
            }
            for (int k = 0; k < totalElementCount; k++) {
                optional<int> v = a.pop_front ();
                EXPECT_TRUE (v.has_value ());
                EXPECT_TRUE (remainingNumbers.erase (*v));
            }
            EXPECT_TRUE (remainingNumbers.empty ());
            EXPECT_TRUE (a.empty ());
        }

        void test_07 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_07"};
            forward_list<int>         a;
            std::vector<std::thread>  threads;
            int                       threadCount           = 5;
            int                       perThreadElementCount = 1000;
            int                       totalElementCount     = perThreadElementCount * threadCount;
            std::mutex                mutex;
            std::set<int>             remainingNumbers;
            for (int k = 0; k < totalElementCount; k++) {
                remainingNumbers.insert (k);
            }
            for (int i = 0; i < threadCount; i++) {
                threads.emplace_back ([&, i] () {
                    for (int j = 0; j < perThreadElementCount; j++) {
                        int y = j + i * perThreadElementCount;
                        a.push_front (y);
                        std::this_thread::sleep_for (chrono::microseconds{rand () % 50});
                        optional<int> x = a.pop_front ();
                        {
                            EXPECT_TRUE (x.has_value ());
                            std::unique_lock<std::mutex> lock{mutex};
                            EXPECT_TRUE (remainingNumbers.erase (*x));
                        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        if (x == y) {
                            DbgTrace ("y"_f);
                        }
                        else {
                            DbgTrace ("n"_f);
                        }
#endif
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            EXPECT_TRUE (a.empty ());
            EXPECT_TRUE (remainingNumbers.empty ());
        }

        void test_08 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_08"};
            forward_list<int>         a;
            std::vector<std::thread>  threads;
            int                       threadCount           = 5;
            int                       perThreadElementCount = int (sTimeMultiplier_ * 1000);
            int                       totalElementCount     = perThreadElementCount * threadCount;
            std::mutex                mutex;
            std::set<int>             remainingNumbers; // essentially 2D array, rows of perThreadElementCount elt, and threadCount rows
            for (int k = 0; k < totalElementCount; k++) {
                remainingNumbers.insert (k);
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("remainingNumbers filled with #s {} to {}"_f, 0, totalElementCount);
#endif
            for (int i = 0; i < threadCount; i++) {
                threads.emplace_back ([&, i] () {
                    for (int j = 0; j < perThreadElementCount; j++) {
                        int y = j + i * perThreadElementCount;
                        a.push_front (y);
                    }
                });
            }
            for (int i = 0; i < threadCount; i++) {
                threads.emplace_back ([&] () {
                    for (int j = 0; j < perThreadElementCount; j++) {
                    retry:
                        optional<int> x = a.pop_front ();
                        bool          r = x.has_value ();
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("popFront returned %d and value %d", r, x);
#endif
                        if (not r) {
                            // this loop could have gotten ahead of the other thread/loop and emptied the list, so retry
                            DbgTrace ("waiting for push_front loop"_f);
                            Execution::Sleep (5ms);
                            goto retry;
                        }
                        EXPECT_TRUE (r);
                        {
                            std::unique_lock<std::mutex> lock (mutex);
                            EXPECT_TRUE (remainingNumbers.erase (*x));
                        }
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            EXPECT_TRUE (a.empty ());
            EXPECT_TRUE (remainingNumbers.empty ());
        }

        void test_09 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_09"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (5);
            auto i = a.begin ();
            EXPECT_TRUE (*i == 5);
            ++i;
            EXPECT_TRUE (*i == 2);
            ++i;
            EXPECT_TRUE (i == a.end ());
        }

        void test_10 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_10"};
            forward_list<int>         a;
            a.push_front (2);
            auto i = a.begin ();
            EXPECT_TRUE (*i == 2);
            a.push_front (5);
            ++i;
            EXPECT_TRUE (i == a.end ());
        }

        void test_11 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_11"};
            forward_list<int>         a;
            a.push_front (2);
            auto                           i = a.begin ();
            [[maybe_unused]] optional<int> v = a.pop_front ();
            a.push_front (5);
            auto j = a.begin ();
            EXPECT_TRUE (*i == 2);
            EXPECT_TRUE (*j == 5);
            ++i;
            EXPECT_TRUE (i == a.end ());
            ++j;
            EXPECT_TRUE (j == a.end ());
        }

        void test_12 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_12"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (5);
            a.insert_after (a.begin (), 3);
            auto i = a.begin ();
            EXPECT_TRUE (*i == 5);
            ++i;
            EXPECT_TRUE (*i == 3);
            ++i;
            EXPECT_TRUE (*i == 2);
            ++i;
            EXPECT_TRUE (i == a.end ());
        }

        void test_13 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_13"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (3);
            a.push_front (5);
            auto i = a.begin ();
            EXPECT_TRUE (*i == 5);
            ++i;
            int v;
            a.erase_after (a.begin (), &v);
            EXPECT_TRUE (v == 3);
            EXPECT_TRUE (*i == 3);
            ++i;
            EXPECT_TRUE (i == a.end ());
            EXPECT_TRUE (*(++a.begin ()) == 2);
        }

        void test_14 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_14"};
            forward_list<int>         a;
            const int                 loopCount = int (sTimeMultiplier_ * 100000);
            for (int i = 0; i < loopCount; i++) {
                a.push_front (i);
            }
        }

        void test_15 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_15"};
            forward_list<int>         a;
            std::vector<std::thread>  threads1;
            std::vector<std::thread>  threads2;
            const int                 threadCount      = 5;
            const int                 perThreadOpCount = int (sTimeMultiplier_ * 100000);
            atomic<bool>              done             = false;
            if (Debug::IsRunningUnderValgrind ()) {
                // workaround hang using MEMCHECK  -- https://stroika.atlassian.net/browse/STK-728
                return;
            }
            for (int i = 0; i < threadCount; i++) {
                threads1.emplace_back ([&] () {
                    for (int j = 0; j < perThreadOpCount; j++) {
                        int op = rand () % (perThreadOpCount / 100);
                        if (op == 0) {
                            [[maybe_unused]] auto cleared = a.clear ();
                            DbgTrace ("cleared={}"_f, cleared);
                        }
                        else {
                            a.push_front (rand () % 20);
                        }
                    }
                });
            }
            for (int i = 0; i < threadCount; i++) {
                threads2.emplace_back ([&] () {
                    auto iterator = a.begin ();
                    while (!done) {
                        if (iterator != a.end ()) {
                            //std::cout << *iterator << " ";
                        }
                        if (iterator == a.end ()) {
                            iterator = a.begin ();
                        }
                        else {
                            ++iterator;
                        }
                    }
                });
            }
            for (auto& thread : threads1) {
                thread.join ();
            }
            done = true;
            for (auto& thread : threads2) {
                thread.join ();
            }
        }

        void test_all ()
        {
            constexpr int kMaxRepeat_{1}; // was 10
            for (int repeat = 0; repeat < kMaxRepeat_; repeat++) {
                test_01 ();
                test_02 ();
                test_03 ();
                test_04 ();
                test_05 ();
                test_06 ();
                test_07 ();
                test_08 ();
                test_09 ();
                test_10 ();
                test_11 ();
                test_12 ();
                test_13 ();
                test_14 ();
                test_15 ();
            }
        }
    };
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        forward_list_tests_::test_all ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
