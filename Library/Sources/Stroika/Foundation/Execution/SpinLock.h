/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_h_
#define _Stroika_Foundation_Execution_SpinLock_h_ 1

#include "../StroikaPreComp.h"

#include <atomic>

#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Time/Realtime.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   Since this lock is NOT recursive, we could add a capture of the current thread, and then
 *              ASSERT no calls come in on the same thread, as they would be guaranteed to deadlock!
 */

namespace Stroika::Foundation {
    namespace Execution {

        /**
         *  SpinLock and mutex can be nearly used interchangeably. Oftentimes, users will want to define a typedef which selects
         *  the faster implementation.
         *
         *  \note   Stroika 2.0a155 and earlier - Execution::kSpinLock_IsFasterThan_mutex was always true
         *
         *  \note   Stroika 2.0a156 and later - due to threadFence and https://stroika.atlassian.net/browse/STK-494 - SpinLock
         *          slowed slightly, but its still notably faster (with the default barrier style) on gcc/unix/windows (x86 only tested).
         */
        constexpr bool kSpinLock_IsFasterThan_mutex = true;

        /**
         *  Implementation based on
         *      http://en.cppreference.com/w/cpp/atomic/atomic_flag
         *
         *  About to run tests to compare performance numbers. But this maybe useful for at least some(many) cases.
         *
         * \note - SpinLock - though generally faster than most mutexes for short accesses, are not recursive mutexes!
         *
         * \note   ***Not Cancelation Point***
         *         SpinLock contains no cancelation points, and so can be used interchangeably with mutex - just for the performance differences
         *
         * \note   From http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3690.pdf
         *         1.10 Multi-threaded executions and data races [intro.multithread]
         * 
         *         "A synchronization operation without an associated memory location is a fence and
         *         can be either an acquire fence, a release fence, or both an acquire and release fence"
         *
         *         Since a spinlock once acquired - can be used to assume assocated data (the data protected by the spinlock mutex)
         *         is up to date with respect to other threads and acquire is needed on the lock. And to assure any changes made with
         *         the lock are seen in other threads a release atomic_fence() is required on the unlock.
         *
         *         This is the DEFAULT behavior we provide with the default BarrierFlag - eReleaseAcquire
         */
        class SpinLock {
        public:
            /**
             *  @see std::atomic_thread_fence ()
             *  @see std::memory_order
             *
             *  \note subtly - eReleaseAcquire means acquire on lock, and release on unlock. This is the natural default for a mutex.
             */
            enum class BarrierFlag {
                eNoBarrier,
                eReleaseAcquire,
                eMemoryTotalOrder,

                eDEFAULT = eReleaseAcquire,

                Stroika_Define_Enum_Bounds (eNoBarrier, eMemoryTotalOrder)
            };

        public:
            /**
             *  In typical usage, one would use a SpinLock as a mutex, and expect it to create a memory fence.
             *  However, sometimes you want to spinlock and handle the memory ordering yourself. So that feature
             *  is optional (defaulting to the safer, but slower - true).
             */
            SpinLock (BarrierFlag barrier = BarrierFlag::eDEFAULT);
            SpinLock (const SpinLock&) = delete;

#if qStroika_FeatureSupported_Valgrind
        public:
            ~SpinLock ();
#endif

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
            BarrierFlag fBarrierFlag_;

        private:
            atomic_flag fLock_ = ATOMIC_FLAG_INIT;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SpinLock.inl"

#endif /*_Stroika_Foundation_Execution_SpinLock_h_*/
