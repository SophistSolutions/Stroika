/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::LockFree
#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Foundation/Containers/LockFreeDataStructures/forward_list.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::LockFreeDataStructures;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {

    namespace forward_list_tests_ {

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
            int v = 0;
            VerifyTestResult (a.pop_front (&v));
            VerifyTestResult (v == 2);
            VerifyTestResult (a.empty ());
        }

        void test_03 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_03"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (5);
            int v = 0;
            VerifyTestResult (a.pop_front (&v));
            VerifyTestResult (v == 5);
            VerifyTestResult (a.pop_front (&v));
            VerifyTestResult (v == 2);
            VerifyTestResult (a.empty ());
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
                int v = 0;
                VerifyTestResult (a.pop_front (&v));
            }
            VerifyTestResult (a.empty ());
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
                        int x;
                        a.pop_front (&x);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        if (x == y) {
                            DbgTrace (L"y");
                        }
                        else {
                            DbgTrace (L"n");
                        }
#endif
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            VerifyTestResult (a.empty ());
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
                int v;
                VerifyTestResult (a.pop_front (&v));
                VerifyTestResult (remainingNumbers.erase (v));
            }
            VerifyTestResult (remainingNumbers.empty ());
            VerifyTestResult (a.empty ());
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
                        std::this_thread::sleep_for (std::chrono::microseconds (rand () % 50));
                        int x;
                        a.pop_front (&x);
                        {
                            std::unique_lock<std::mutex> lock (mutex);
                            VerifyTestResult (remainingNumbers.erase (x));
                        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        if (x == y) {
                            DbgTrace (L"y");
                        }
                        else {
                            DbgTrace (L"n");
                        }
#endif
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            VerifyTestResult (a.empty ());
            VerifyTestResult (remainingNumbers.empty ());
        }

        void test_08 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_08"};
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
                    }
                });
            }
            for (int i = 0; i < threadCount; i++) {
                threads.emplace_back ([&] () {
                    for (int j = 0; j < perThreadElementCount; j++) {
                        int x;
                        a.pop_front (&x);
                        {
                            std::unique_lock<std::mutex> lock (mutex);
                            VerifyTestResult (remainingNumbers.erase (x));
                        }
                    }
                });
            }
            for (auto& thread : threads) {
                thread.join ();
            }
            VerifyTestResult (a.empty ());
            VerifyTestResult (remainingNumbers.empty ());
        }

        void test_09 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_09"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (5);
            auto i = a.begin ();
            VerifyTestResult (*i == 5);
            ++i;
            VerifyTestResult (*i == 2);
            ++i;
            VerifyTestResult (i == a.end ());
        }

        void test_10 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_10"};
            forward_list<int>         a;
            a.push_front (2);
            auto i = a.begin ();
            VerifyTestResult (*i == 2);
            a.push_front (5);
            ++i;
            VerifyTestResult (i == a.end ());
        }

        void test_11 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_11"};
            forward_list<int>         a;
            a.push_front (2);
            auto i = a.begin ();
            int  v;
            a.pop_front (&v);
            a.push_front (5);
            auto j = a.begin ();
            VerifyTestResult (*i == 2);
            VerifyTestResult (*j == 5);
            ++i;
            VerifyTestResult (i == a.end ());
            ++j;
            VerifyTestResult (j == a.end ());
        }

        void test_12 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_12"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (5);
            a.insert_after (a.begin (), 3);
            auto i = a.begin ();
            VerifyTestResult (*i == 5);
            ++i;
            VerifyTestResult (*i == 3);
            ++i;
            VerifyTestResult (*i == 2);
            ++i;
            VerifyTestResult (i == a.end ());
        }

        void test_13 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_13"};
            forward_list<int>         a;
            a.push_front (2);
            a.push_front (3);
            a.push_front (5);
            auto i = a.begin ();
            VerifyTestResult (*i == 5);
            ++i;
            int v;
            a.erase_after (a.begin (), &v);
            VerifyTestResult (v == 3);
            VerifyTestResult (*i == 3);
            ++i;
            VerifyTestResult (i == a.end ());
            VerifyTestResult (*(++a.begin ()) == 2);
        }

        void test_14 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_14"};
            forward_list<int>         a;
            for (int i = 0; i < 100000; i++) {
                a.push_front (i);
            }
        }

        void test_15 ()
        {
            Debug::TraceContextBumper ctx{"{}::test_15"};
            forward_list<int>         a;
            std::vector<std::thread>  threads1;
            std::vector<std::thread>  threads2;
            int const                 threadCount      = 5;
            int const                 perThreadOpCount = 100000;
            atomic<bool>              done             = false;
            for (int i = 0; i < threadCount; i++) {
                threads1.emplace_back ([&] () {
                    for (int j = 0; j < perThreadOpCount; j++) {
                        int op = rand () % (perThreadOpCount / 100);
                        if (op == 0) {
                            [[maybe_unused]] auto cleared = a.clear ();
                            DbgTrace ("cleared=%d", cleared);
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
    void DoRegressionTests_ ()
    {
        forward_list_tests_::test_all ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
