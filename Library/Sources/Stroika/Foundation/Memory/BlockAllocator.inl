/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

#include "Common.h"

/**
 *  \brief qStroika_Foundation_Memory_BlockAllocator_UseLockFree_ provides a lock-free implementation of BlockAllocator (which should be faster)
 *
 *  STILL EXPERIMENTAL< but appears to be working well so leave on by default
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
#include "../Execution/Exceptions.h"
#include <cstdlib>
#endif

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            namespace Private_ {

                /*
                 *  kUseSpinLock_ is probaly best true (empirical tests with the
                 *  performance regression test indicated that this helped considerably).
                 *
                 *  It should be generally pretty safe because the locks are very narrow (so threads shoudln't spend much time
                 *  spinning. And we could reduce this further during mallocs of new blocks.
                 */
                constexpr bool kUseSpinLock_ = !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_ and Execution::kSpinLock_IsFasterThan_mutex;

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
                struct BlockAllocator_ModuleInit_ {
                    BlockAllocator_ModuleInit_ ();
                    ~BlockAllocator_ModuleInit_ ();
                };

                using LockType_ = conditional_t<kUseSpinLock_, Execution::SpinLock, mutex>;
                extern LockType_* sLock_;

                inline LockType_& GetLock_ ()
                {
                    AssertNotNull (sLock_); // automatically initailized by BlockAllocated::Private_::ActualModuleInit
                    return *sLock_;
                }

                void DoDeleteHandlingLocksExceptionsEtc_ (void* p, void** staticNextLinkP) noexcept;
#endif
            }

            namespace Private_ {

                // not quite right - too much of a PITA to support both constexpr and non- just wait til all our compilers support constexpr and then fix!
                inline constexpr size_t BlockAllocation_Private_AdjustSizeForPool_ (size_t n)
                {
                    // when we really fix constexpr usage, we can use the below!
                    //return  Math::RoundUpTo (sizeof(T), sizeof (void*));
                    return (((n + sizeof (void*) - 1u) / sizeof (void*)) * sizeof (void*));
                }

                /*
                * Picked particular kTargetMallocSize since with malloc overhead likely to turn out to be
                * a chunk which memory allocator can do a good job on.
                */
                constexpr size_t kTargetMallocSize_ = 16360; // 16384 = 16K - leave a few bytes sluff...

                inline constexpr size_t BlockAllocation_Private_ComputeChunks_ (size_t poolElementSize)
                {
                    return max (static_cast<size_t> (kTargetMallocSize_ / poolElementSize), static_cast<size_t> (10));
                }

                // This must be included here to keep genclass happy, since the .cc file will not be included
                // in the genclassed .cc file....
                inline void** GetMem_Util_ (size_t sz)
                {
                    Require (sz >= sizeof (void*));

                    const size_t kChunks = BlockAllocation_Private_ComputeChunks_ (sz);
                    Assert (kChunks >= 1);

                    /*
                     * Please note that the following line is NOT a memory leak. Please look at the
                     * Led FAQ question#29 - "Does Led have any memory leaks?
                     * How does qAllowBlockAllocation affect memory leaks?"
                     */
#if qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_
                    void** newLinks = (void**)::malloc (kChunks * sz);
                    Execution::ThrowIfNull (newLinks);
#else
                    void**       newLinks = (void**)new char[kChunks * sz];
#endif
                    AssertNotNull (newLinks);
                    void** curLink = newLinks;
                    for (size_t i = 1; i < kChunks; i++) {
                        *curLink = &(((char*)newLinks)[i * sz]);
                        curLink  = (void**)*curLink;
                    }
                    *curLink = nullptr; // nullptr-terminate the link list
                    return newLinks;
                }

                /*
                 *       In order to reduce fragmentation, we allocate all chunks of common sizes out of
                 *   the same pool. Assuming the compiler is able to inline the references to these
                 *   below, there is really no cost in adding more. I picked the ones I thought most
                 *   likely to come up, but adding more should be just fine - strictly a win.
                 *
                 *       Don't bother implementing Block_Alloced_sizeof_N_GetMems() cuz flunging will
                 *   genericly give us the same code-sharing effect.
                 *
                 *  <<@todo - update docs -.... VERY OUT OF DATE>>>.
                 */
                template <size_t SIZE>
                class BlockAllocationPool_ {
                public:
                    static void* Allocate (size_t n);
                    static void  Deallocate (void* p) noexcept;
                    static void  Compact ();

                private:
// make op new inline for MOST important case
// were alloc is cheap linked list operation...
#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
                    static atomic<void*> sHeadLink_;
#else
                    static void* sHeadLink_;
#endif
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
                 *  Note - once we have stored Private_::kLockedSentinal_ in the sHeadLink_ and gotten back something other than that, we
                 *  effectively have a lock for the scope below (because nobody else can get other than Private_::kLockedSentinal_ from exchange).
                 */
            again:
                void* p = sHeadLink_.exchange (Private_::kLockedSentinal_, memory_order_acq_rel);
                if (p == Private_::kLockedSentinal_) {
                    // we stored and retrieved a sentinal. So no lock. Try again!
                    this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point.
                    goto again;
                }
                // if we got here, p contains the real head, and have a pseudo lock
                if (p == nullptr) {
                    p = GetMem_Util_ (SIZE);
                }
                void* result = p;
                AssertNotNull (result);
                /*
                 *  Treat this as a linked list, and make head point to next member.
                 *
                 *  Use atomic_load to guarantee the value not loaded from cache line not shared across threads.
                 */
                static_assert (sizeof (void*) == sizeof (atomic<void*>), "atomic doesn't change size");
                void* next = reinterpret_cast<const atomic<void*>*> (p)->load (memory_order_acquire);
                Verify (sHeadLink_.exchange (next, memory_order_acq_rel) == Private_::kLockedSentinal_); // must return Private_::kLockedSentinal_ cuz we owned lock, so Private_::kLockedSentinal_ must be there
                return result;
#else
                [[maybe_unused]] auto&& critSec = lock_guard{Private_::GetLock_ ()};
                /*
                 * To implement linked list of BlockAllocated(T)'s before they are
                 * actually alloced, re-use the begining of this as a link pointer.
                 */
                if (sHeadLink_ == nullptr) {
                    sHeadLink_ = GetMem_Util_ (SIZE);
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
                /*
                 *  Note - once we have stored Private_::kLockedSentinal_ in the sHeadLink_ and gotten back something other than that, we
                 *  effectively have a lock for the scope below (because nobody else can get other than Private_::kLockedSentinal_ from exchange).
                 */
            again:
                void* prevHead = sHeadLink_.exchange (Private_::kLockedSentinal_, memory_order_acq_rel);
                if (prevHead == Private_::kLockedSentinal_) {
                    // we stored and retrieved a sentinal. So no lock. Try again!
                    this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point.
                    goto again;
                }
                /*
                 *  Push p onto the head of linked free list.
                 *
                 *  Use atomic to guarantee the value not pushed to RAM so shared across threads.
                 */
                static_assert (sizeof (void*) == sizeof (atomic<void*>), "atomic doesn't change size");
                void* newHead = p;
                reinterpret_cast<atomic<void*>*> (newHead)->store (prevHead, memory_order_release);
                Verify (sHeadLink_.exchange (newHead, memory_order_acq_rel) == Private_::kLockedSentinal_); // must return Private_::kLockedSentinal_ cuz we owned lock, so Private_::kLockedSentinal_ must be there
#else
                Private_::DoDeleteHandlingLocksExceptionsEtc_ (p, &sHeadLink_);
#endif
#if qStroika_FeatureSupported_Valgrind
                VALGRIND_HG_CLEAN_MEMORY (p, SIZE);
#endif
            }
            template <size_t SIZE>
            void Private_::BlockAllocationPool_<SIZE>::Compact ()
            {
#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
// cannot compact lock-free - no biggie
#else
                [[maybe_unused]] auto&& critSec = lock_guard{Private_::GetLock_ ()};

                // step one: put all the links into a single, sorted vector
                const size_t kChunks = BlockAllocation_Private_ComputeChunks_ (SIZE);
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
                    Byte*  deleteCandidate = (Byte*)links[index];
                    bool   canDelete       = true;
                    size_t i               = 1;
                    for (; i < kChunks; ++i) {
                        if ((index + i) >= links.size ())
                            break;
                        Byte* next = (Byte*)links[index + i];
                        Byte* prev = (Byte*)links[index + i - 1];
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
#if qStroika_Foundation_Memory_BlockAllocator_UseMallocDirectly_
                        ::free ((void*)deleteCandidate);
#else
                        delete static_cast<Byte*> ((void*)deleteCandidate);
#endif
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
#if qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
            atomic<void*> Private_::BlockAllocationPool_<SIZE>::sHeadLink_{nullptr};
#else
            void* Private_::BlockAllocationPool_<SIZE>::sHeadLink_{nullptr};
#endif

            /*
             ********************************************************************************
             ********************************** BlockAllocator<T> ***************************
             ********************************************************************************
             */
            template <typename T>
            inline void* BlockAllocator<T>::Allocate ([[maybe_unused]] size_t n)
            {
                using Private_::BlockAllocation_Private_AdjustSizeForPool_;
                using Private_::BlockAllocationPool_;
                Require (n == sizeof (T));
#if qAllowBlockAllocation
                void* result = BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Allocate (n);
#else
                void* result = ::operator new (n);
#endif
                EnsureNotNull (result);
                Ensure (reinterpret_cast<ptrdiff_t> (result) % alignof (T) == 0); // see https://stroika.atlassian.net/browse/STK-511 - assure aligned
                return result;
            }
            template <typename T>
            inline void BlockAllocator<T>::Deallocate (void* p) noexcept
            {
                using Private_::BlockAllocation_Private_AdjustSizeForPool_;
                using Private_::BlockAllocationPool_;
#if qAllowBlockAllocation
                if (p != nullptr) {
                    BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Deallocate (p);
                }
#else
                ::operator delete (p);
#endif
            }
            template <typename T>
            void BlockAllocator<T>::Compact ()
            {
                using Private_::BlockAllocation_Private_AdjustSizeForPool_;
                using Private_::BlockAllocationPool_;
#if qAllowBlockAllocation
                BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Compact ();
#endif
            }
        }
    }
}
#if !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Memory::Private_::BlockAllocator_ModuleInit_> _Stroika_Foundation_Memory_BlockAllocator_ModuleInit_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif
#endif /*_Stroika_Foundation_Memory_BlockAllocator_inl_*/
