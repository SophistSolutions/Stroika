/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocator_inl_
#define _Stroika_Foundation_Memory_BlockAllocator_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <algorithm>
#include <mutex>
#include <vector>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/Math/Common.h"

#include "Common.h"

/**
 *  \brief qStroika_Foundation_Memory_BlockAllocator_UseLockFree_ provides a lock-free 
 *         implementation of BlockAllocator (which should be faster)
 */
//#define qStroika_Foundation_Memory_BlockAllocator_UseLockFree_   0
#if !defined(qStroika_Foundation_Memory_BlockAllocator_UseLockFree_)
#define qStroika_Foundation_Memory_BlockAllocator_UseLockFree_ 1
#endif

/*
 *  qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_ is currently experimental as
 *  a test to see if its faster than calling operator new (for blocks of RAM).
 *
 *  operator new[] does very little for us, and could in principle have call overhead and
 *  overhead to track the number of entries in the array (which would be pointless).
 */
#if !defined(qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_)
#define qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_ 1
#endif

#if qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_
#include <cstdlib>

#include "../Execution/Throw.h"
#endif

namespace Stroika::Foundation::Memory {

    namespace Private_ {

#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
        /**
         *  Basic idea is to use a sentinel value to indicate LOCKED - and use atomic exchange, with
         *  the head of list pointer.
         *
         *  If we exchange and get the sentinel, we didnt get the lock so retry.
         *
         *  If don't get the sentinel, we have the lock (and stored the sentinel so nobody else will get the lock)
         *  so go ahead and complete the operation.
         *
         *  \note   I tried to make kLockedsentinel_ constexpr, but this generates errors, apparently because:
         *          http://en.cppreference.com/w/cpp/language/constant_expression
         *
         *          Address constant expression is a prvalue core constant expression (after conversions required by context)
         *          of type nullptr_t or of a pointer type, which points to an object with static storage duration, one past
         *          the end of an array with static storage duration, to a function, or is a null pointer
         *
         *          @todo COULD use UNION to workaround this...
         */
        static void* const kLockedSentinel_ = (void*)1; // any invalid pointer
#else
        /*
         *  kUseSpinLock_ is probaly best true (empirical tests with the
         *  performance regression test indicated that this helped considerably).
         *
         *  It should be generally pretty safe because the locks are very narrow (so threads shoudln't spend much time
         *  spinning. And we could reduce this further during mallocs of new blocks.
         */
        constexpr bool kUseSpinLock_ = !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_ and Execution::kSpinLock_IsFasterThan_mutex;

        using LockType_ = conditional_t<kUseSpinLock_, Execution::SpinLock, mutex>;

        inline LockType_& GetLock_ ()
        {
            static LockType_ sLock_;
            return sLock_;
        }

        void DoDeleteHandlingLocksExceptionsEtc_ (void* p, void** staticNextLinkP) noexcept;
#endif
    }

    namespace Private_ {
        /*
         *  BlockAllocationPool_ implements the core  logic for allocation/deallocation of a particular pool. These are organized
         *  by size rather than type to reduce fragmentation.
         */
        template <size_t SIZE>
        class BlockAllocationPool_ {
        public:
            // never returns nullptr - throws if memory exhausted
            static void* Allocate (size_t n);
            // require p != nullptr
            static void Deallocate (void* p) noexcept;
            static void Compact ();

        private:
            alignas (void*) static inline conditional_t<qStroika_Foundation_Memory_BlockAllocator_UseLockFree_, atomic<void*>, void*> sHeadLink_{nullptr};

        private:
            static constexpr size_t ComputeChunks_ ();
            static void**           GetMem_Util_ ();
        };
    }

    /*
     ********************************************************************************
     ***************** Private_::BlockAllocationPool_<SIZE> *************************
     ********************************************************************************
     */
    template <size_t SIZE>
    inline void* Private_::BlockAllocationPool_<SIZE>::Allocate ([[maybe_unused]] size_t n)
    {
        static_assert (SIZE >= sizeof (void*), "SIZE >= sizeof (void*)");
        Require (n <= SIZE);

#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
        /*
        */
        constexpr bool kTryMemoryOrderOptimizations_ = true; // experiment as of 2023-09-26

        /*
         *  Note - once we have stored Private_::kLockedSentinel_ in the sHeadLink_ and gotten back something other than that, we
         *  effectively have a lock for the scope below (because nobody else can get other than Private_::kLockedSentinel_ from exchange).
         */
        void* p{};
        {
        again:
            p = sHeadLink_.exchange (Private_::kLockedSentinel_, memory_order_acq_rel);
            if (p == Private_::kLockedSentinel_) [[unlikely]] {
                // we stored and retrieved a sentinel. So no lock. Try again!
                this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point.
                goto again;
            }
        }
        // if we got here, p contains the real head, and have a pseudo lock
        if (p == nullptr) [[unlikely]] {
            p = GetMem_Util_ ();
        }
        void* result = p;
        AssertNotNull (result);
        /*
         *  Treat this as a linked list, and make head point to next member.
         *
         *  Use atomic_load to guarantee the value not loaded from cache line not shared across threads.
         */
        static_assert (sizeof (void*) == sizeof (atomic<void*>), "atomic doesn't change size");
        // even though we have 'acquired' a logical lock, the memory at address 'p' may not be syned to our local processor/thread
        void* next = reinterpret_cast<const atomic<void*>*> (p)->load (memory_order_acquire);
        if constexpr (kTryMemoryOrderOptimizations_) {
            Verify (sHeadLink_.exchange (next, memory_order_release) == Private_::kLockedSentinel_); // must return Private_::kLockedSentinel_ cuz we owned lock, so Private_::kLockedSentinel_ must be there
        }
        else {
            Verify (sHeadLink_.exchange (next, memory_order_acq_rel) == Private_::kLockedSentinel_); // must return Private_::kLockedSentinel_ cuz we owned lock, so Private_::kLockedSentinel_ must be there
        }
        return result;
#else
        [[maybe_unused]] lock_guard critSec{Private_::GetLock_ ()};
        /*
         * To implement linked list of BlockAllocated(T)'s before they are
         * actually alloced, re-use the begining of this as a link pointer.
         */
        if (sHeadLink_ == nullptr) [[unlikely]] {
            sHeadLink_ = GetMem_Util_ ();
        }
        void* result = sHeadLink_;
        AssertNotNull (result);
        /*
         * treat this as a linked list, and make head point to next member
         */
        sHeadLink_ = (*(void**)sHeadLink_);
        return result;
#endif
    }
    template <size_t SIZE>
    inline void Private_::BlockAllocationPool_<SIZE>::Deallocate (void* p) noexcept
    {
        static_assert (SIZE >= sizeof (void*), "SIZE >= sizeof (void*)");
        RequireNotNull (p);
#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_

        constexpr bool kTryMemoryOrderOptimizations_ = true; // experiment as of 2023-09-26

        /*
         *  Note - once we have stored Private_::kLockedSentinel_ in the sHeadLink_ and gotten back something other than that, we
         *  effectively have a lock for the scope below (because nobody else can get other than Private_::kLockedSentinel_ from exchange).
         */
        void* prevHead{};
        {
        again:
            prevHead = sHeadLink_.exchange (Private_::kLockedSentinel_, memory_order_acq_rel);
            if (prevHead == Private_::kLockedSentinel_) [[unlikely]] {
                // we stored and retrieved a sentinel. So no lock. Try again!
                this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point.
                goto again;
            }
        }
        /*
         *  Push p onto the head of linked free list.
         *
         *  Use atomic to guarantee the value not pushed to RAM so shared across threads.
         */
        static_assert (sizeof (void*) == sizeof (atomic<void*>), "atomic doesn't change size");
        void* newHead = p;
        reinterpret_cast<atomic<void*>*> (newHead)->store (prevHead, memory_order_release);
        if constexpr (kTryMemoryOrderOptimizations_) {
            Verify (sHeadLink_.exchange (newHead, memory_order_release) == Private_::kLockedSentinel_); // must return Private_::kLockedSentinel_ cuz we owned lock, so Private_::kLockedSentinel_ must be there
        }
        else {
            Verify (sHeadLink_.exchange (newHead, memory_order_acq_rel) == Private_::kLockedSentinel_); // must return Private_::kLockedSentinel_ cuz we owned lock, so Private_::kLockedSentinel_ must be there
        }
#else
        Private_::DoDeleteHandlingLocksExceptionsEtc_ (p, &sHeadLink_);
#endif
    }
    template <size_t SIZE>
    void Private_::BlockAllocationPool_<SIZE>::Compact ()
    {
#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
        // cannot compact lock-free - no biggie
#else
        [[maybe_unused]] lock_guard critSec{Private_::GetLock_ ()};

        // step one: put all the links into a single, sorted vector
        const size_t kChunks = ComputeChunks_ ();
        Assert (kChunks >= 1);
        vector<void*> links;
        try {
            links.reserve (kChunks * 2);
            void* link = sHeadLink_;
            while (link != nullptr) {
                // probably should use Containers::ReserveSpeedTweekAddN - but want to avoid embrace of dependencies
                if (links.size () == links.capacity ()) {
                    links.reserve (links.size () * 2);
                }
                links.push_back (link);
                link = *(void**)link;
            }
        }
        catch (...) {
            return;
        }

        sort (links.begin (), links.end ());

        // now look for unused memory blocks. Since the vector is sorted by pointer value, the first pointer encountered is potentially the start of a block.
        // Look at the next N vector elements, where N is the amount of elements that would have been alloced (the chunk size). If they are all there, then the
        // block is unused can can be freed. Otherwise do the same thing to the first element found outside of the block.
        size_t originalSize = links.size ();
        size_t index        = 0;
        while (index < links.size ()) {
            byte*  deleteCandidate = (byte*)links[index];
            bool   canDelete       = true;
            size_t i               = 1;
            for (; i < kChunks; ++i) {
                if ((index + i) >= links.size ())
                    break;
                byte* next = (byte*)links[index + i];
                byte* prev = (byte*)links[index + i - 1];
                if (canDelete and ((next - prev) != SIZE)) {
                    canDelete = false; // don't break here, as have to find end up this block, which could be further on
                }
                if (static_cast<size_t> (abs (next - deleteCandidate) / SIZE) >= kChunks) {
                    Assert (not canDelete);
                    break;
                }
            }
            if (canDelete) {
                links.erase (links.begin () + index, links.begin () + index + kChunks);
                if constexpr (qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_) {
                    ::free ((void*)deleteCandidate);
                }
                else {
                    delete static_cast<byte*> ((void*)deleteCandidate);
                }
            }
            else {
                index += i;
            }
        }

        // finally, clean up the pool. The head link is probably wrong, and the elements are no longer linked up correctly
        sHeadLink_ = (links.size () == 0) ? nullptr : links[0];
        if (links.size () != originalSize and links.size () > 0) {
            // we delete some, which means that the next pointers are bad
            void** curLink = (void**)sHeadLink_;
            for (size_t i = 1; i < links.size (); ++i) {
                *curLink = links[i];
                curLink  = (void**)*curLink;
            }
            *curLink = nullptr;
        }
#endif
    }
    template <size_t SIZE>
    constexpr size_t Private_::BlockAllocationPool_<SIZE>::ComputeChunks_ ()
    {
        /*
         * Picked particular kTargetMallocSize since with malloc overhead likely to turn out to be
         * a chunk which memory allocator can do a good job on.
         */
        constexpr size_t kTargetMallocSize_ = 16360; // 16384 = 16K - leave a few bytes sluff...

        return Math::AtLeast (static_cast<size_t> (kTargetMallocSize_ / SIZE), static_cast<size_t> (10));
    }
    template <size_t SIZE>
    inline void** Private_::BlockAllocationPool_<SIZE>::GetMem_Util_ ()
    {
        Require (SIZE >= sizeof (void*));

        constexpr size_t kChunks = ComputeChunks_ ();
        Assert (kChunks >= 1);

        /*
         * Please note that the following line is NOT a memory leak. @see BlockAllocator<>
         */
        void** newLinks;
        if constexpr (qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_) {
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
            newLinks = (void**)::malloc (kChunks * SIZE);
            Execution::ThrowIfNull (newLinks);
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        }
        else {
            newLinks = (void**)new char[kChunks * SIZE];
        }
        AssertNotNull (newLinks);
        /*
         *  Create a 'linked list' of blocks - all of size SIZE. The first element of each block is a pointer to the next
         *  block. The last block must be NULL-terminated.
         * 
         *      @todo MAYBE make this a member of the class, and use it elsewhere for the linked-list logic
         */
        {
            struct linkedListElt {
                linkedListElt* fNext;
            };
            auto nextLinkAlias = reinterpret_cast<linkedListElt*> (newLinks);
            // the first kChunk-1 links point to the next guy
            for (size_t i = 1; i < kChunks; ++i) {
                auto nextNextLink    = reinterpret_cast<linkedListElt*> (reinterpret_cast<byte*> (nextLinkAlias) + SIZE);
                nextLinkAlias->fNext = nextNextLink;
                nextLinkAlias        = nextNextLink;
            }
            // The FINAL link must be NULL-terminated
            nextLinkAlias->fNext = nullptr;
        }
        return newLinks;
    }

    /*
     ********************************************************************************
     ******************************** BlockAllocator<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename U>
    constexpr BlockAllocator<T>::BlockAllocator (const BlockAllocator<U>&) noexcept
    {
    }
    template <typename T>
    [[nodiscard]] inline T* BlockAllocator<T>::allocate ([[maybe_unused]] size_t n)
    {
        using Private_::BlockAllocationPool_;
        Require (n == 1);
#if qStroika_Foundation_Memory_PreferBlockAllocation
        T* result = reinterpret_cast<T*> (BlockAllocationPool_<AdjustSizeForPool_ ()>::Allocate (sizeof (T)));
#else
        T* result = reinterpret_cast<T*> (::operator new (sizeof (T)));
#endif
        EnsureNotNull (result);
        Ensure (reinterpret_cast<ptrdiff_t> (result) % alignof (T) == 0); // see https://stroika.atlassian.net/browse/STK-511 - assure aligned
        return result;
    }
    template <typename T>
    inline void BlockAllocator<T>::deallocate (T* p, [[maybe_unused]] size_t n) noexcept
    {
        Require (n == 1);
        using Private_::BlockAllocationPool_;
#if qStroika_Foundation_Memory_PreferBlockAllocation
        if (p != nullptr) [[likely]] {
            BlockAllocationPool_<AdjustSizeForPool_ ()>::Deallocate (p);
        }
#else
        ::operator delete (p);
#endif
    }
    template <typename T>
    void BlockAllocator<T>::Compact ()
    {
        using Private_::BlockAllocationPool_;
#if qStroika_Foundation_Memory_PreferBlockAllocation
        BlockAllocationPool_<AdjustSizeForPool_ ()>::Compact ();
#endif
    }
    template <typename T>
    constexpr size_t BlockAllocator<T>::AdjustSizeForPool_ ()
    {
        return Math::RoundUpTo (sizeof (T), sizeof (void*));
    }
    template <typename T>
    inline bool BlockAllocator<T>::operator== (const BlockAllocator& rhs) const
    {
        return true;
    }

}

#endif /*_Stroika_Foundation_Memory_BlockAllocator_inl_*/
