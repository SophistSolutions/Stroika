/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_h_
#define _Stroika_Foundation_Execution_SpinLock_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Time/Realtime.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Execution {

    /**
     *  SpinLock and mutex can be nearly used interchangeably. Oftentimes, users will want to define a typedef which selects
     *  the faster implementation.
     *
     *  How much faster than a mutex? Tests/52 carries that comparison as a threshold test.
     *  Untrustworthy (not super well tested) results maybe still helpful:
     *
     *      +--------------+------------+----------------------+-----------+-------+
     *      | OS           | CPU        | compiler             | std lib   | ratio |
     *      +--------------+------------+----------------------+-----------+-------+
     *      | Windows      | x86-64     | MSVC 14.51.36231     | MSVC STL  | 0.49  |
     *      | macOS 26.5   | arm64 (M1) | Apple clang 17.0.0   | libc++    | 0.72  |
     *      | Ubuntu 26.04 | x86-64     | g++ 16               | libstdc++ | 1.22  |
     *      | Ubuntu 26.04 | x86-64     | g++ 15 (C++23)       | libstdc++ | 1.25  |
     *      +--------------+------------+----------------------+-----------+-------+
     *          ratio = SpinLock time / mutex time, so BELOW 1.0 means SpinLock won. Tests/52 at
     *          -x 15, Release builds, pinned to one core where the OS allows it (macOS does not).
     *
     *  There is no clang Release configuration to run Tests/52 from, so the compiler-versus-standard-
     *  library question was separated with a standalone replica of the same loop, all three built and
     *  run on one Linux box:
     *
     *          g++ 16     + libstdc++    1.57      SpinLock's own time came out the SAME in all
     *          clang++ 22 + libstdc++    1.25      three (1.554 / 1.554 / 1.556s) - every bit of
     *          clang++ 22 + libc++       0.99      the spread is in how fast std::mutex is.
     *
     *  So this tracks the standard library rather than the compiler: libstdc++ has a very fast
     *  uncontended std::mutex and beats this spinlock whichever compiler builds it, libc++ on Linux
     *  is a wash, and MSVC STL and libc++-on-macOS lose to it clearly. Which is why
     *  kSpinLock_IsFasterThan_mutex below keys off the std-library macros. Beware comparing the two
     *  tables: the replica put g++/libstdc++ at 1.57 where Tests/52 said 1.22, so trust the SIGN of
     *  any of this and not the magnitude.
     *
     *  ***And read this caveat before believing any of it.*** That loop takes and releases the lock from a
     *  SINGLE thread, which is a spinlock's best case: no syscall, no futex, and never any actual waiting.
     *  Under real contention a spinlock burns CPU while it spins, and can be very much WORSE than a mutex -
     *  so treat these numbers as the ceiling rather than the expected value, and measure your own case if
     *  it is contended.
     *
     *  \note   Stroika 2.0a155 and earlier - Execution::kSpinLock_IsFasterThan_mutex was always true
     *
     *  \note   Stroika 2.0a156 through 3.0d24 - due to threadFence and
     *          https://github.com/SophistSolutions/Stroika/issues/628 (STK-494) - SpinLock slowed slightly, but its
     *          still notably faster than mutex on gcc/unix/windows (x86 only tested).
     *
     *  \note   Stroika 3.0d25 and later - the standalone fences, and the BarrierType knob that selected among
     *          them, were both removed (@see SpinLock::try_lock ()). Careful about attributing the resulting
     *          speedup: on x86-64 those fences emitted NO instruction at all (an acquire or release fence is only
     *          a compiler barrier there), so what actually shrank the generated code was dropping the runtime
     *          barrier dispatch - try_lock () went 11 instructions to 5, unlock () 8 to 3. The fences still
     *          mattered, just not for speed on x86: ThreadSanitizer cannot model them, and on weakly ordered
     *          hardware they are real instructions.
     */
    constexpr bool kSpinLock_IsFasterThan_mutex =
#if defined(_GLIBCXX_RELEASE)
        false // libstdc++ (whichever compiler builds it): measured 1.22 .. 1.57 - its uncontended std::mutex wins
#elif defined(_LIBCPP_VERSION) and qStroika_Foundation_Common_Platform_MacOS
        true // libc++ on macOS/arm64: measured 0.72
#elif defined(_LIBCPP_VERSION)
        false // libc++ elsewhere (Linux): measured 0.99, a tie - so prefer mutex, which degrades better under contention
#elif defined(_MSVC_STL_VERSION)
        true // MSVC STL: measured 0.49
#else
        false // unmeasured std library - same tie-break: prefer mutex
#endif
        ;

    /**
     *  Implementation based on
     *      http://en.cppreference.com/w/cpp/atomic/atomic_flag
     *
     * \note - SpinLock - generally faster than most mutexes for short accesses (see kSpinLock_IsFasterThan_mutex)
     *         BUT it can be degererate (due to spinning) and take LONGER if used to hold locks a long time.
     *
     * \note - not recursive mutex
     *
     * \note Satisfies Concepts:
     *      o   Common::StdCompat::BasicLockable<SpinLock>
     *      o   Common::StdCompat::Lockable<SpinLock>
     *      o   not movable<SpinLock>, not copyable<SpinLock>  (as std::mutex)
     *
     * \note   ***Not Cancelation Point***
     *         SpinLock contains no cancelation points, and so can be used interchangeably with mutex - just for the performance differences
     *
     * \note   ***Memory ordering***
     *         You use a mutex to protect some data structure. A thread that acquires the lock needs to see the writes
     *         made by whoever held it last (that is the acquire), and a thread that releases the lock needs its own
     *         writes published to whoever acquires it next (that is the release). So acquire on the test_and_set ()
     *         and release on the clear () is exactly - and entirely - what a mutex owes its caller.
     *
     *         That is not a preference, it is the contract: [thread.mutex.requirements.mutex] asks a mutex for just
     *         two things - that lock/unlock on a SINGLE mutex appear in one total order, which the modification order
     *         of the flag already provides at any memory order, and that "prior unlock () operations on the same
     *         object shall synchronize with this operation", which is precisely release paired with acquire. Nothing
     *         weaker satisfies it and nothing stronger adds to it. @see try_lock ().
     *
     * \note   v3.0d25 removed the BarrierType constructor argument (eNoBarrier / eReleaseAcquire / eMemoryTotalOrder),
     *         which had let a caller pick a different memory order for the flag. It went because neither alternative
     *         to the default was defensible:
     *          o   eNoBarrier never had an observable effect - the flag was read acquire and cleared release whatever
     *              you passed - and had it been made to mean memory_order_relaxed it would have been actively unsafe.
     *              Mutual exclusion would survive (RMW atomicity does not depend on memory order), but the protected
     *              data would no longer be published to the next holder, and the compiler would be free to move that
     *              data's accesses across the lock and the unlock. It would not even have been faster on x86-64,
     *              where test_and_set () emits the same LOCK-prefixed xchg for relaxed as for acquire, and clear ()
     *              the same plain store for relaxed as for release.
     *          o   eMemoryTotalOrder (memory_order_seq_cst) is distinguishable only by protocols reasoning about the
     *              global order of operations on SEVERAL DIFFERENT locks. The standard deliberately promises a total
     *              order per mutex and not across mutexes, so that is not a mutex guarantee anywhere - offering it
     *              here would have made SpinLock advertise something std::mutex does not, against the whole premise
     *              that the two are interchangeable. Code that truly needs it wants a seq_cst atomic, named as such
     *              at the point of use.
     *
     * \note   References, in the order they became useful:
     *          o   https://github.com/SophistSolutions/Stroika/issues/628 (STK-494) - where the reasoning above was
     *              worked out and settled (see the closing 2018-01-27 comment)
     *          o   https://www.boost.org/doc/libs/1_54_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_spinlock
     *          o   https://blog.regehr.org/archives/2173 - alleges that done properly you do not need the fence.
     *              That turned out to be right. It also suggests a fairer spinlock, which this still is not.
     */
    class SpinLock {
    public:
        SpinLock ()                = default;
        SpinLock (const SpinLock&) = delete;

    public:
        ~SpinLock () = default;

    public:
        nonvirtual SpinLock& operator= (const SpinLock&) = delete;

    public:
        /**
         */
        nonvirtual bool try_lock ();

    public:
        /**
         */
        nonvirtual void lock ();

    public:
        /**
         */
        nonvirtual void unlock ();

    private:
        atomic_flag fLock_;
    };

    /*
     *  See 'Satisfies Concepts' in the class docs above.
     */
    static_assert (Common::StdCompat::BasicLockable<SpinLock> and Common::StdCompat::Lockable<SpinLock>);
    static_assert (not movable<SpinLock> and not copyable<SpinLock>); // like std::mutex

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SpinLock.inl"

#endif /*_Stroika_Foundation_Execution_SpinLock_h_*/
