/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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

#include    "../Debug/Assertions.h"
#include    "../Memory/LeakChecker.h"

#include    "Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            namespace   Private {

                struct  BlockAllocation_ModuleInit_ {
                    BlockAllocation_ModuleInit_ ();
                    ~BlockAllocation_ModuleInit_ ();
                };

                extern  mutex*    sCritSection_;

                inline  mutex&    GetCritSection_ ()
                {
                    AssertNotNull (sCritSection_);  // automatically initailized by BlockAllocated::Private::ActualModuleInit
                    return *sCritSection_;
                }

            }


            namespace   Private {


#if qCompilerAndStdLib_Supports_constexpr
                constexpr inline    size_t  BlockAllocation_Private_AdjustSizeForPool_ (size_t n)
                {
                    // when we really fix constexpr usage, we can use the below!
                    //return  Math::RoundUpTo (sizeof(T), sizeof (void*));
                    return (((x + sizeof (void*) - 1u) / sizeof (void*)) * sizeof (void*));
                }
#else
#define BlockAllocation_Private_AdjustSizeForPool_(n)\
    (((n + sizeof (void*) - 1u) / sizeof (void*)) * sizeof (void*))
#endif

                // This must be included here to keep genclass happy, since the .cc file will not be included
                // in the genclassed .cc file....
                inline  void**  GetMem_Util_ (size_t sz)
                {
                    Require (sz >= sizeof (void*));

                    /*
                     * Picked particular kTargetMallocSize since with malloc overhead likely to turn out to be
                     * a chunk which memory allocator can do a good job on.
                     */
                    constexpr   size_t  kTargetMallocSize   =   16360;                  // 16384 = 16K - leave a few bytes sluff...
                    const       size_t  kChunks = max (static_cast<size_t> (kTargetMallocSize / sz), static_cast<size_t> (10));

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
                 *  <<@todo - update docs - now using xx...
                 */
                template    <size_t SIZE>
                class   BlockAllocationPool_  {
                public:
                    static  void*   Allocate (size_t n);
                    static  void    Deallocate (void* p);

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
                Require (n == SIZE);
                Arg_Unused (n);                         // n only used for debuggging, avoid compiler warning

#if     qAllowBlockAllocation
                lock_guard<mutex>  critSec (Private::GetCritSection_ ());
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
#else
                return ::operator new (n);
#endif
            }
            template    <size_t SIZE>
            inline  void    Private::BlockAllocationPool_<SIZE>::Deallocate (void* p)
            {
                static_assert (SIZE >= sizeof (void*), "SIZE >= sizeof (void*)");
#if     qAllowBlockAllocation
                if (p != nullptr) {
                    lock_guard<mutex>  critSec (Private::GetCritSection_ ());
                    // push p onto the head of linked free list
                    (*(void**)p) = sNextLink_;
                    sNextLink_ = p;
                }
#else
                ::operator delete (p);
#endif
            }
            template    <size_t SIZE>
            void*   Private::BlockAllocationPool_<SIZE>::sNextLink_ = nullptr;



            /*
             ********************************************************************************
             ************************** BlockAllocationSupport<T> ***************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  void*   BlockAllocationSupport<T>::Allocate (size_t n)
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
            inline  void    BlockAllocationSupport<T>::Deallocate (void* p)
            {
#if     qAllowBlockAllocation
                if (p != nullptr) {
                    Private::BlockAllocationPool_<BlockAllocation_Private_AdjustSizeForPool_ (sizeof (T))>::Deallocate (p);
                }
#else
                ::operator delete (p);
#endif
            }


        }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Memory::Private::BlockAllocation_ModuleInit_>   _Stroika_Foundation_Memory_BlockAllocated_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
