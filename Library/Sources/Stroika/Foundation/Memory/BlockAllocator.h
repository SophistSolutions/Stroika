/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocator_h_
#define _Stroika_Foundation_Memory_BlockAllocator_h_ 1

#include "../StroikaPreComp.h"

#include <cstddef>

#include "../Configuration/Common.h"

#include "../Execution/ModuleInit.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  @todo   Consider doing GetMem_Util_ code outside of the context of the lock-guard, and if
 *          we get multiple results, just patch them into the linked list. That way in case of
 *          multithreading (when we're paging in freepool) - we'll do less busy waiting. But, this would
 *          be at the cost of possibly allocating too many blocks when two threads both run out of memory
 *          in the same pool at the same time - though that could be undone).
 *
 *  @todo   Compact() method could be smart enough to move items in free pool that are from
 *          mostly used up pools to the head of the free list, and move mostly unused blocks
 *          to the end of the free pool, which over time should probably tend to defragment
 *          the pools.
 *
 *  @todo   BlockAllocator<T> could hugely benefit from some optimistic locking
 *          strategy - like in http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3341.pdf
 *
 *          Make sure when we experiment with that - we include it here as one of our first
 *          optimization points!
 *
 *          There is now a C++ semi-standard for this, and working implementations (I haven't tried). But my current
 *          lockfree approach maybe better.
 *
 *  @todo   Maybe use TRAITS on BlockAllocator to define some stuff
 *          about strategies (options) - like how to share pools?
 *
 *  @todo   BlockAllocator<>::Compact ()
 *              o   not sure this is useful, or worth the effort, but since
 *                  Sterl wrote it - leave it in for a while - til I get clearer experience.
 *
 *              o   BlockAllocationPool_<SIZE>::Compact () algorithm uses vector, which could fail if
 *                  we are so fragmented we cannot allocate the large contiguous block needed for the vector<>.
 *                  Catch-22. Maybe find a less costly strategy?
 *
 *              o   I make have broken something in Compact() routine when I transcribed it from Sterl's code.
 *                  never tested (by me).
 */

namespace Stroika::Foundation::Memory {

    /**
     *  Low-level tool to allocate and free memory from a fixed size/element pool. Very high performance since
     *  no searching or coalescing ever needed, but at the cost of creating some amount of fragmentation.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  This API operates at the level of malloc/free - just allocating fixed sized blocks and freeing them.
     *
     *  For easiser use, probably the best approach is to @see UseBlockAllocationIfAppropriate
     *
     *  \note   Design Note: alignas / alignemnt of allocated values
     *          https://stroika.atlassian.net/browse/STK-511
     *          We use sizeof(). We always allocate large blocks which (I htink are always) aligned to the largest
     *          alignemnt required by the system, and we that as an array.
     *
     *          But I think since sizeof(T) is the offset from one elemnet of an array[T] - our allocations will always be aligned
     *          if the first big block is aligned.
     *
     *          To double/triple check, we have an Ensure in BlockAllocator<T>::Allocate () to assure aligned allocations
     *
     *  \note
     *       Assuming the compiler is able to inline the mapping from type T, to which sized pool, there is really no cost in adding more. The reason
     *       to have differnt type T objects share the same BlockAllocationPool_ is to reduce fragmentation.
     *
     *  \note
     *      To make BlockAllocator more of a plug-replacement for the std-c++ free-pool allocator, neither Allocate nor Deallocate () are cancelation
     *      points.
     *
     *      Making them cancelation points led to too many subtle bugs were very rarely we would have 'terminate' called due to a memory allocation
     *      from a no-except method.
     *
     *  But also see:
     *      @see ManuallyBlockAllocated
     *      @see UseBlockAllocationIfAppropriate
     */
    template <typename T>
    class BlockAllocator {
    public:
        /**
         *  \req (n == sizeof (T))
         *
         *  \note - though this can throw due to memory exhaustion, it will not throw Thread::InterupptionException - it is not a cancelation point.
         */
        static void* Allocate (size_t n);

    public:
        /**
         *  \req (p allocated by BlockAllocator<T>::Allocate ());
         *  p can be nullptr
         */
        static void Deallocate (void* p) noexcept;

    public:
        /**
          * Return to the free store all deallocated blocks whcih can be returned.
          *
          * This takes O (N), where N is the total number of extant operator new allocations for this size.
          *
          * It also currently uses a large block of contiguous memory, which could fail.
          *
          * But it might sometimes return memory from a particular data structure (fixed element size pool) to
          * the general free store.
          *
          * Also - beware - this locks out other threads (from allocation/deallocation) during execution (which can take a while).
          */
        static void Compact ();

    private:
        // ensure return value is >= sizeof (T), and has legit alignment for T
        static constexpr size_t AdjustSizeForPool_ ();
    };

    /**
     *  This can be referenced in your ModuleInit<> to force correct inter-module construction order.
     */
    Execution::ModuleDependency MakeModuleDependency_BlockAllocator ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BlockAllocator.inl"

#endif /*_Stroika_Foundation_Memory_BlockAllocator_h_*/
