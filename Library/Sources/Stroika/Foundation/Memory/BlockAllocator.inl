/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

#include "../Debug/Assertions.h"
#include "../Debug/Valgrind.h"
#include "../Execution/Common.h"
#include "../Execution/SpinLock.h"
#include "../Math/Common.h"

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
         *  Basic idea is to use a sentinal value to indicate LOCKED - and use atomic exchange, with
         *  the head of list pointer.
         *
         *  If we exchange and get the sentinal, we didnt get the lock so retry.
         *
         *  If don't get the sentinal, we have the lock (and stored the sentinal so nobody else will get the lock)
         *  so go ahead and complete the operation.
         *
         *  \note   I tried to make kLockedSentinal_ constexpr, but this generates errors, apparently because:
         *          http://en.cppreference.com/w/cpp/language/constant_expression
         *
         *          Address constant expression is a prvalue core constant expression (after conversions required by context)
         *          of type nullptr_t or of a pointer type, which points to an object with static storage duration, one past
         *          the end of an array with static storage duration, to a function, or is a null pointer
         *
         *          @todo COULD use UNION to workaround this...
         */
        static void* const kLockedSentinal_ = (void*)1; // any invalid pointer
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
            static inline
#if qCompilerAndStdLib_inline_static_align_Buggy
                alignas (void*)
#endif
                    conditional_t<qStroika_Foundation_Memory_BlockAllocator_UseLockFree_, atomic<void*>, void*> sHeadLink_{nullptr};

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
         *  NOTES ABOUT VALGRIND:
         *      We must notify valgrind, if its running, about the reads/writes to newHead
         *      and sHeadLink_, since these are modified without locks.
         *
         *      According to https://chromium.googlesource.com/chromium/src/base/+/a13f5e49df65bed29c205d939a70ba9925c4a971/dynamic_annotations.h
         *      "The race checker will create a happens-before arc from"...
         *      But this SEEMS to suggest I should call "HAPPENSBEFORE" before
         *      I change the location, and HAPPENSAFTER afterwards.
         *
         *      Dont use Stroika_Foundation_Debug_ValgrindMarkAddressAsAllocated, deallocated,
         *      since these cause false positive warnings about races (not sure - probably my misunderstanding
         *      about these macros).
         *
         *  Before doing this thoroughly we would occasionally get:

            )))- valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/g++-valgrind-debug-SSLPurify/Samples-WebServer/WebServer --quit-after 10...==2665543== ---Thread-Announcement------------------------------------------
            ==2665543== 
                ...
            ==2665543== 
            ==2665543== Possible data race during read of size 8 at 0x4BC5080 by thread #6
            ==2665543== Locks held: none
            ==2665543==    at 0x35AD59: load (atomic_base.h:740)
            ==2665543==    by 0x35AD59: std::atomic<void*>::load(std::memory_order) const (atomic:519)
            ==2665543==    by 0x3A578C: Stroika::Foundation::Memory::Private_::BlockAllocationPool_<64ul>::Allocate(unsigned long) (BlockAllocator.inl:166)
            ==2665543==    by 0x47A67C: Stroika::Foundation::Memory::BlockAllocator<Stroika::Foundation::Characters::Concrete::Private::BufferedStringRepBlock_<16ul> >::Allocate(unsigned long) (BlockAllocator.inl:359)
            ==2665543==    by 0x4794BA: Stroika::Foundation::Characters::Concrete::Private::BufferedStringRep::_Rep::_Rep(wchar_t const*, wchar_t const*, unsigned long) (String_BufferedStringRep.inl:141)
            ...
            ==2665543== 
            ==2665543== This conflicts with a previous write of size 8 by thread #1
            ==2665543== Locks held: none
            ==2665543==    at 0x350700: store (atomic_base.h:718)
            ==2665543==    by 0x350700: std::atomic<void*>::store(void*, std::memory_order) (atomic:510)
            ==2665543==    by 0x3A5A41: Stroika::Foundation::Memory::Private_::BlockAllocationPool_<64ul>::Deallocate(void*) (BlockAllocator.inl:213)
            ==2665543==    by 0x47A870: Stroika::Foundation::Memory::BlockAllocator<Stroika::Foundation::Characters::Concrete::Private::BufferedStringRepBlock_<16ul> >::Deallocate(void*) (BlockAllocator.inl:373)
            ==2665543==    by 0x479625: Stroika::Foundation::Characters::Concrete::Private::BufferedStringRep::_Rep::~_Rep() (String_BufferedStringRep.inl:172)
        *
        *       hopefully no more --LGP 2020-11-08
        */

        /*
         *  Note - once we have stored Private_::kLockedSentinal_ in the sHeadLink_ and gotten back something other than that, we
         *  effectively have a lock for the scope below (because nobody else can get other than Private_::kLockedSentinal_ from exchange).
         */
        void* p{};
        {
        again:
            Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE (&sHeadLink_);
            p = sHeadLink_.exchange (Private_::kLockedSentinal_, memory_order_acq_rel);
            Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER (&sHeadLink_);
            if (p == Private_::kLockedSentinal_) {
                // we stored and retrieved a sentinal. So no lock. Try again!
                this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point.
                goto again;
            }
        }
        // if we got here, p contains the real head, and have a pseudo lock
        if (p == nullptr) {
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
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE (p);
        void* next = reinterpret_cast<const atomic<void*>*> (p)->load (memory_order_acquire);
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER (p);
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE (&sHeadLink_);
        Verify (sHeadLink_.exchange (next, memory_order_acq_rel) == Private_::kLockedSentinal_); // must return Private_::kLockedSentinal_ cuz we owned lock, so Private_::kLockedSentinal_ must be there
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER (&sHeadLink_);
        //Stroika_Foundation_Debug_ValgrindMarkAddressAsAllocated (result, n);
        return result;
#else
        [[maybe_unused]] auto&& critSec = lock_guard{Private_::GetLock_ ()};
        /*
         * To implement linked list of BlockAllocated(T)'s before they are
         * actually alloced, re-use the begining of this as a link pointer.
         */
        if (sHeadLink_ == nullptr) {
            sHeadLink_ = GetMem_Util_ ();
        }
        void* result = sHeadLink_;
        AssertNotNull (result);
        /*
         * treat this as a linked list, and make head point to next member
         */
        sHeadLink_ = (*(void**)sHeadLink_);
        Stroika_Foundation_Debug_ValgrindMarkAddressAsAllocated (result, n);
        return result;
#endif
    }
    template <size_t SIZE>
    inline void Private_::BlockAllocationPool_<SIZE>::Deallocate (void* p) noexcept
    {
        static_assert (SIZE >= sizeof (void*), "SIZE >= sizeof (void*)");
        RequireNotNull (p);
#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
        /*
         *  Note - once we have stored Private_::kLockedSentinal_ in the sHeadLink_ and gotten back something other than that, we
         *  effectively have a lock for the scope below (because nobody else can get other than Private_::kLockedSentinal_ from exchange).
         */
        void* prevHead{};
        {
        again:
            Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE (&sHeadLink_);
            prevHead = sHeadLink_.exchange (Private_::kLockedSentinal_, memory_order_acq_rel);
            Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER (&sHeadLink_);
            if (prevHead == Private_::kLockedSentinal_) {
                // we stored and retrieved a sentinal. So no lock. Try again!
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
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE (newHead);
        reinterpret_cast<atomic<void*>*> (newHead)->store (prevHead, memory_order_release);
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER (newHead);
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE (&sHeadLink_);
        Verify (sHeadLink_.exchange (newHead, memory_order_acq_rel) == Private_::kLockedSentinal_); // must return Private_::kLockedSentinal_ cuz we owned lock, so Private_::kLockedSentinal_ must be there
        Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER (&sHeadLink_);
        //Stroika_Foundation_Debug_ValgrindMarkAddressAsDeAllocated (p, SIZE);
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
        using std::byte;
        [[maybe_unused]] auto&& critSec = lock_guard{Private_::GetLock_ ()};

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
            for (size_t i = 1; i < links.size (); i++) {
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
            newLinks = (void**)::malloc (kChunks * SIZE);
            Execution::ThrowIfNull (newLinks);
        }
        else {
            newLinks = (void**)new char[kChunks * SIZE];
        }
        AssertNotNull (newLinks);
        void** curLink = newLinks;
        for (size_t i = 1; i < kChunks; i++) {
            *curLink = &(((char*)newLinks)[i * SIZE]);
            curLink  = (void**)*curLink;
        }
        *curLink = nullptr; // nullptr-terminate the link list
        return newLinks;
    }

    /*
     ********************************************************************************
     ******************************** BlockAllocator<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    inline void* BlockAllocator<T>::Allocate ([[maybe_unused]] size_t n)
    {
        using Private_::BlockAllocationPool_;
        Require (n == sizeof (T));
#if qAllowBlockAllocation
        void* result = BlockAllocationPool_<AdjustSizeForPool_ ()>::Allocate (sizeof (T));
#else
        void* result = ::operator new (sizeof (T));
#endif
        EnsureNotNull (result);
        Ensure (reinterpret_cast<ptrdiff_t> (result) % alignof (T) == 0); // see https://stroika.atlassian.net/browse/STK-511 - assure aligned
        return result;
    }
    template <typename T>
    inline void BlockAllocator<T>::Deallocate (void* p) noexcept
    {
        using Private_::BlockAllocationPool_;
#if qAllowBlockAllocation
        if (p != nullptr) {
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
#if qAllowBlockAllocation
        BlockAllocationPool_<AdjustSizeForPool_ ()>::Compact ();
#endif
    }
    template <typename T>
    constexpr size_t BlockAllocator<T>::AdjustSizeForPool_ ()
    {
        return Math::RoundUpTo (sizeof (T), sizeof (void*));
    }
}

#endif /*_Stroika_Foundation_Memory_BlockAllocator_inl_*/
