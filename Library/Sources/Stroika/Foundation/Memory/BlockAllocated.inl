/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_inl_
#define _Stroika_Foundation_Memory_BlockAllocated_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <algorithm>
#include    <mutex>
#include    <vector>

#include    "../Debug/Assertions.h"
#include    "../Memory/LeakChecker.h"

#include    "Common.h"



// Private IMPL define - may allow another way to specify eventually, but for now just to test
#if     !defined (qBlockAllocation_UseSpinLock_)
#define qBlockAllocation_UseSpinLock_   1
#endif

#if    qBlockAllocation_UseSpinLock_
#include    "../Execution/SpinLock.h"
#endif




namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            namespace   Private {

                struct  BlockAllocation_ModuleInit_ {
                    BlockAllocation_ModuleInit_ ();
                    ~BlockAllocation_ModuleInit_ ();
                };

#if    qBlockAllocation_UseSpinLock_
                using LockType_ = Execution::SpinLock;
#else
                using LockType_ = mutex;
#endif
                extern  LockType_*    sLock_;

                inline  LockType_&    GetLock_ ()
                {
                    AssertNotNull (sLock_);  // automatically initailized by BlockAllocated::Private::ActualModuleInit
                    return *sLock_;
                }

            }


            namespace   Private {

#if     qCompilerAndStdLib_constexpr_Buggy || 1
#define BlockAllocation_Private_AdjustSizeForPool_(n)\
    (((n + sizeof (void*) - 1u) / sizeof (void*)) * sizeof (void*))
#else
                // not quite right - too much of a PITA to support both constexpr and non- just wait til all our compilers support constexpr and then fix!
                inline    constexpr size_t  BlockAllocation_Private_AdjustSizeForPool_ (size_t n)
                {
                    // when we really fix constexpr usage, we can use the below!
                    //return  Math::RoundUpTo (sizeof(T), sizeof (void*));
                    return (((n + sizeof (void*) - 1u) / sizeof (void*)) * sizeof (void*));
                }
#endif


                inline    constexpr size_t  BlockAllocation_Private_ComputeChunks_ (size_t poolElementSize)
                {
                    /*
                     * Picked particular kTargetMallocSize since with malloc overhead likely to turn out to be
                     * a chunk which memory allocator can do a good job on.
                     */
                    constexpr   size_t  kTargetMallocSize   =   16360;                  // 16384 = 16K - leave a few bytes sluff...
                    return std::max (static_cast<size_t> (kTargetMallocSize / poolElementSize), static_cast<size_t> (10));
                }


                // This must be included here to keep genclass happy, since the .cc file will not be included
                // in the genclassed .cc file....
                inline  void**  GetMem_Util_ (size_t sz)
                {
                    Require (sz >= sizeof (void*));

                    const       size_t  kChunks = BlockAllocation_Private_ComputeChunks_ (sz);

                    /*
                     * Please note that the following line is NOT a memory leak. Please look at the
                     * Led FAQ question#29 - "Does Led have any memory leaks?
                     * How does qAllowBlockAllocation affect memory leaks?"
                     */
                    void**  newLinks    =   (void**)DEBUG_NEW char [kChunks * sz];
                    void**  curLink     =   newLinks;
                    for (size_t i = 1; i < kChunks; i++) {
                        *curLink = &(((char*)newLinks)[i * sz]);
                        curLink = (void**)*curLink;
                    }
                    *curLink = nullptr;     // nullptr-terminate the link list
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
                template    <size_t SIZE>
                class   BlockAllocationPool_  {
                public:
                    static  void*   Allocate (size_t n);
                    static  void    Deallocate (void* p);
                    static  void    Compact ();

                private:
                    // make op new inline for MOST important case
                    // were alloc is cheap linked list operation...
                    static  void*   sNextLink_;
                };


            }


            /*
             ********************************************************************************
             ************************* BlockAllocationPool_<SIZE> ***************************
             ********************************************************************************
             */
            template    <size_t SIZE>
            inline  void*   Private::BlockAllocationPool_<SIZE>::Allocate (size_t n)
            {
                static_assert (SIZE >= sizeof (void*), "SIZE >= sizeof (void*)");
                Require (n <= SIZE);
                Arg_Unused (n);                         // n only used for debuggging, avoid compiler warning

                lock_guard<LockType_>  critSec (Private::GetLock_ ());
                /*
                 * To implement linked list of BlockAllocated(T)'s before they are
                 * actually alloced, re-use the begining of this as a link pointer.
                 */
                if (sNextLink_ == nullptr) {
                    sNextLink_ = GetMem_Util_ (SIZE);
                }
                void*   result = sNextLink_;
                AssertNotNull (result);
                /*
                 * treat this as a linked list, and make head point to next member
                 */
                sNextLink_ = (*(void**)sNextLink_);
                return result;
            }
            template    <size_t SIZE>
            inline  void    Private::BlockAllocationPool_<SIZE>::Deallocate (void* p)
            {
                static_assert (SIZE >= sizeof (void*), "SIZE >= sizeof (void*)");
                Require (p != nullptr);
                lock_guard<LockType_>  critSec (Private::GetLock_ ());
                // push p onto the head of linked free list
                (*(void**)p) = sNextLink_;
                sNextLink_ = p;
            }
            template    <size_t SIZE>
            void    Private::BlockAllocationPool_<SIZE>::Compact ()
            {
                lock_guard<LockType_>  critSec (Private::GetLock_ ());

                // step one: put all the links into a single, sorted vector
                const   size_t  kChunks = BlockAllocation_Private_ComputeChunks_ (SIZE);
                std::vector<void*> links;
                try {
                    links.reserve (kChunks * 2);
                    void*   link = sNextLink_;
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

                std::sort (links.begin (), links.end ());

                // now look for unused memory blocks. Since the vector is sorted by pointer value, the first pointer encountered is potentially the start of a block.
                // Look at the next N vector elements, where N is the amount of elements that would have been alloced (the chunk size). If they are all there, then the
                // block is unused can can be freed. Otherwise do the same thing to the first element found outside of the block.
                size_t  originalSize = links.size ();
                size_t  index = 0;
                while (index < links.size ()) {
                    Byte*   deleteCandidate = (Byte*)links[index];
                    bool    canDelete = true;
                    size_t  i = 1;
                    for (; i < kChunks; ++i) {
                        if ((index + i) >= links.size ()) break;
                        Byte* next = (Byte*)links[index + i];
                        Byte* prev = (Byte*)links[index + i - 1];
                        if (canDelete and ((next - prev) != SIZE)) {
                            canDelete = false;  // don't break here, as have to find end up this block, which could be further on
                        }
                        if (static_cast <size_t> (std::abs (next - deleteCandidate) / SIZE) >= kChunks) {
                            Assert (not canDelete);
                            break;
                        }
                    }
                    if (canDelete) {
                        links.erase (links.begin () + index, links.begin () + index + kChunks);
                        delete static_cast<Byte*> ((void*)deleteCandidate);
                    }
                    else {
                        index += i;
                    }
                }

                // finally, clean up the pool. The head link is probably wrong, and the elements are no longer linked up correctly
                sNextLink_ = (links.size () == 0) ? nullptr : links[0];
                if (links.size () != originalSize and links.size () > 0) {
                    // we delete some, which means that the next pointers are bad
                    void**  curLink     =   (void**)sNextLink_;
                    for (size_t i = 1; i < links.size (); i++) {
                        *curLink = links[i];
                        curLink = (void**)*curLink;
                    }
                    *curLink = nullptr;
                }
            }
            template    <size_t SIZE>
            void*   Private::BlockAllocationPool_<SIZE>::sNextLink_ = nullptr;



            /*
             ********************************************************************************
             ********************************** BlockAllocator<T> ***************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  void*   BlockAllocator<T>::Allocate (size_t n)
            {
                Require (n == sizeof (T));
                Arg_Unused (n);                         // n only used for debuggging, avoid compiler warning

#if     qAllowBlockAllocation
                return Private::BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Allocate (n);
#else
                return ::operator new (n);
#endif
            }
            template    <typename   T>
            inline  void    BlockAllocator<T>::Deallocate (void* p)
            {
#if     qAllowBlockAllocation
                if (p != nullptr) {
                    Private::BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Deallocate (p);
                }
#else
                ::operator delete (p);
#endif
            }
            template    <typename   T>
            void    BlockAllocator<T>::Compact ()
            {
#if     qAllowBlockAllocation
                Private::BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Compact ();
#endif
            }

        }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Memory::Private::BlockAllocation_ModuleInit_>   _Stroika_Foundation_Memory_BlockAllocated_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
