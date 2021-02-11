/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::LockFree
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#if 0
#ifdef _MSC_VER //for doing leak detection
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#define DUMP _CrtDumpMemoryLeaks ()
#else
#define DUMP
#endif
#endif

#include <cassert>
#include <iostream>
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



namespace {


    class concurrent_forward_list_tests {
    public:
        static void test_01 ()
        {
            {
                concurrent_forward_list<int> a;
            }
          //  DUMP;
        }

        static void test_02 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                int v = 0;
                assert (a.pop_front (&v));
                assert (v == 2);
                assert (a.empty ());
            }
         //   DUMP;
        }

        static void test_03 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                a.push_front (5);
                int v = 0;
                assert (a.pop_front (&v));
                assert (v == 5);
                assert (a.pop_front (&v));
                assert (v == 2);
                assert (a.empty ());
            }
       //     DUMP;
        }

        static void test_04 ()
        {
            {
                concurrent_forward_list<int> a;
                std::vector<std::thread>     threads;
                int                          threadCount           = 5;
                int                          perThreadElementCount = 1000;
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
                    assert (a.pop_front (&v));
                    std::cout << v << " ";
                }
                assert (a.empty ());
            }
         //   DUMP;
        }

        static void test_05 ()
        {
            {
                concurrent_forward_list<int> a;
                std::vector<std::thread>     threads;
                for (int i = 0; i < 5; i++) {
                    threads.emplace_back ([&a] () {
                        for (int j = 0; j < 1000; j++) {
                            int y = rand ();
                            a.push_front (y);
                            std::this_thread::sleep_for (std::chrono::microseconds (rand () % 10));
                            int x;
                            a.pop_front (&x);
                            if (x == y) {
                                std::cout << "y";
                            }
                            else {
                                std::cout << "n";
                            }
                        }
                    });
                }
                for (auto& thread : threads) {
                    thread.join ();
                }
                assert (a.empty ());
            }
         //   DUMP;
        }

        static void test_06 ()
        {
            {
                concurrent_forward_list<int> a;
                std::vector<std::thread>     threads;
                int                          threadCount           = 5;
                int                          perThreadElementCount = 1000;
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
                    assert (a.pop_front (&v));
                    std::cout << v << " ";
                    assert (remainingNumbers.erase (v));
                }
                assert (remainingNumbers.empty ());
                assert (a.empty ());
            }
      //      DUMP;
        }

        static void test_07 ()
        {
            {
                concurrent_forward_list<int> a;
                std::vector<std::thread>     threads;
                int                          threadCount           = 5;
                int                          perThreadElementCount = 1000;
                int                          totalElementCount     = perThreadElementCount * threadCount;
                std::mutex                   mutex;
                std::cout << "Initializing concurrent_forward_list_tests::test_07\n";
                std::set<int> remainingNumbers;
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
                                assert (remainingNumbers.erase (x));
                            }
                            if (x == y) {
                                std::cout << "y";
                            }
                            else {
                                std::cout << "n";
                            }
                        }
                    });
                }
                for (auto& thread : threads) {
                    thread.join ();
                }
                assert (a.empty ());
                assert (remainingNumbers.empty ());
            }
       //     DUMP;
        }

        static void test_08 ()
        {
            {
                concurrent_forward_list<int> a;
                std::vector<std::thread>     threads;
                int                          threadCount           = 5;
                int                          perThreadElementCount = 1000;
                int                          totalElementCount     = perThreadElementCount * threadCount;
                std::mutex                   mutex;
                std::set<int>                remainingNumbers;
                std::cout << "Initializing concurrent_forward_list_tests::test_08\n";
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
                    threads.emplace_back ([&, i] () {
                        for (int j = 0; j < perThreadElementCount; j++) {
                            int x;
                            a.pop_front (&x);
                            {
                                std::unique_lock<std::mutex> lock (mutex);
                                assert (remainingNumbers.erase (x));
                            }
                            std::cout << x << " ";
                        }
                    });
                }
                for (auto& thread : threads) {
                    thread.join ();
                }
                assert (a.empty ());
                assert (remainingNumbers.empty ());
            }
       //     DUMP;
        }

        static void test_09 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                a.push_front (5);
                auto i = a.begin ();
                assert (*i == 5);
                ++i;
                assert (*i == 2);
                ++i;
                assert (i == a.end ());
            }
         //   DUMP;
        }

        static void test_10 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                auto i = a.begin ();
                assert (*i == 2);
                a.push_front (5);
                ++i;
                assert (i == a.end ());
            }
       //     DUMP;
        }

        static void test_11 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                auto i = a.begin ();
                int  v;
                a.pop_front (&v);
                a.push_front (5);
                auto j = a.begin ();
                assert (*i == 2);
                assert (*j == 5);
                ++i;
                assert (i == a.end ());
                ++j;
                assert (j == a.end ());
            }
     //       DUMP;
        }

        static void test_12 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                a.push_front (5);
                a.insert_after (a.begin (), 3);
                auto i = a.begin ();
                assert (*i == 5);
                ++i;
                assert (*i == 3);
                ++i;
                assert (*i == 2);
                ++i;
                assert (i == a.end ());
            }
        //    DUMP;
        }

        static void test_13 ()
        {
            {
                concurrent_forward_list<int> a;
                a.push_front (2);
                a.push_front (3);
                a.push_front (5);
                auto i = a.begin ();
                assert (*i == 5);
                ++i;
                int v;
                a.erase_after (a.begin (), &v);
                assert (v == 3);
                assert (*i == 3);
                ++i;
                assert (i == a.end ());
                assert (*(++a.begin ()) == 2);
            }
       //     DUMP;
        }

        static void test_14 ()
        {
            {
                std::cout << "\ntest_14\n";
                concurrent_forward_list<int> a;
                for (int i = 0; i < 100000; i++) {
                    a.push_front (i);
                }
            }
     //       DUMP;
        }

        static void test_15 ()
        {
            {
                concurrent_forward_list<int> a;
                std::vector<std::thread>     threads1;
                std::vector<std::thread>     threads2;
                int const                    threadCount      = 5;
                int const                    perThreadOpCount = 100000;
                bool                         done             = false;
                for (int i = 0; i < threadCount; i++) {
                    threads1.emplace_back ([&, i] () {
                        for (int j = 0; j < perThreadOpCount; j++) {
                            int op = rand () % (perThreadOpCount / 100);
                            if (op == 0) {
                                std::cout << "\n"
                                          << a.clear () << "\n";
                            }
                            else {
                                a.push_front (rand () % 20);
                            }
                        }
                    });
                }
                for (int i = 0; i < threadCount; i++) {
                    threads2.emplace_back ([&, i] () {
                        auto iterator = a.begin ();
                        while (!done) {
                            if (iterator != a.end ()) {
                                std::cout << *iterator << " ";
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
        //    DUMP;
        }

        //static void test_() {
        //  {
        //      concurrent_forward_list<int> a;
        //  }
        //  DUMP;
        //}

        static void test_all ()
        {
            for (int repeat = 0; repeat < 10; repeat++) {
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
        concurrent_forward_list_tests::test_all ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
