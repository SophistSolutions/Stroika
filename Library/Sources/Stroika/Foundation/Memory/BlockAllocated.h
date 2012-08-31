/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_h_
#define _Stroika_Foundation_Memory_BlockAllocated_h_    1

#include    "../StroikaPreComp.h"

#include    <cstddef>

#include    "../Configuration/Common.h"

#include    "../Execution/ModuleInit.h"


/**
 *  \file
 *
 *  @todo   We should either add a VARIANT or template parameter to BlockAllocated<> saying whether we
 *          should force block allocation, or ONLY block-allocate if size is appropriate to one of our
 *          preformed pools.
 *
 *          The reason for this option is for better use in templates like LinkedList<> - where we might
 *          want to blockallocate for small sizes of T, but not for ALL.
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {



            /**
             *  \def qAllowBlockAllocation
             *
             *  \brief  qAllowBlockAllocation
             *
             *  Allow use of block-allocation. The main reason to disable it indescriminantly
             *  is for debugging purposes (looking for memory leaks). But others may have other
             *  reasons.
             *
             *      Defaults to true
             *
             *  \hideinitializer
             */
#if     !defined (qAllowBlockAllocation)
#error "qAllowBlockAllocation should normally be defined indirectly by StroikaConfig.h"
#endif




            /**
              * \brief  Utility class to implement special memory allocator pattern which can greatly improve performance - @see DECLARE_USE_BLOCK_ALLOCATION()
              *
              * BlockAllocated<T> is a templated class designed to allow easy use
              * of a block-allocated memory strategy. This means zero overhead malloc/memory allocation for fixed
              * size blocks (with the only problem being the storage is never - or almost never - returned to the
              * free store - it doesn't leak - but cannot be used for other things). This is often a useful
              * tradeoff for things you allocate a great number of.
              *
              * You shouldn't disable it lightly. But you may wish to temporarily disable block-allocation
              * while checking for memory leaks by shutting of the @'qAllowBlockAllocation' compile-time configuration variable.
              *
              * Note also - you can avoid some of the uglines of the overload declarations by using the
              * DECLARE_USE_BLOCK_ALLOCATION() macro.
            */
            template    <typename   T>
            class   BlockAllocated {
            public:
                static  void*   operator new (size_t n);
                static  void    operator delete (void* p);

#if     qAllowBlockAllocation
            private:
                static  void    GetMem_ (); // Break out into separate function so we can
                // make op new inline for MOST important case
                // were alloc is cheap linked list operation...

                static  void*   sNextLink;
                static  void*   GetNextLink_ ();
                static  void    SetNextLink_ (void* nextLink);
#endif
            };


            /**
             * \def DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)
             *
             *  \brief  Use this to enable block allocation for a particular class. *Beware* of subclassing.
             *
             *  This helper macro can be used to avoid some of the C++ gorp required in declaring that you are
             *  using block-allocation with a given class.
             *
             *  An example of use might be:
             *  <code>
             *  <pre>
             *      class   Foo {
             *          public:
             *              DECLARE_USE_BLOCK_ALLOCATION(Foo);
             *      }
             *  </pre>
             *  </code>
             *
             *  @see DECLARE_DONT_USE_BLOCK_ALLOCATION()
             *  @see BlockAllocated
             *
             *  \hideinitializer
             */
#if     qAllowBlockAllocation
#define DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)\
    static  void*   operator new (size_t n)                         {   return (Stroika::Foundation::Memory::BlockAllocated<THIS_CLASS>::operator new (n)); }\
    static  void*   operator new (size_t n,int,const char*,int)     {   return (Stroika::Foundation::Memory::BlockAllocated<THIS_CLASS>::operator new (n)); }\
    static  void    operator delete (void* p)                       {   Stroika::Foundation::Memory::BlockAllocated<THIS_CLASS>::operator delete (p);       }\
    static  void    operator delete (void* p,int,const char*,int)   {   Stroika::Foundation::Memory::BlockAllocated<THIS_CLASS>::operator delete (p);       }
#else
#define DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)
#endif


            /**
             *  \def    DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)
             *
             *  \brief  Use this to disable block allocation for a subclass of a class that HAD been using block allocation.
             *
             *  If you subclass from a class which uses block-allocation, you can use this to turn off block allocation
             *  in your particular subclass.</p>
             *
             *
             *  An example of use might be:
             *  <code>
             *  <pre>
             *      class   Foo {
             *          public:
             *              DECLARE_USE_BLOCK_ALLOCATION(Foo);
             *      };
             *      class   Bar : public Foo {
             *          public:
             *              DECLARE_DONT_USE_BLOCK_ALLOCATION(Bar);
             *      };
             *  </pre>
             *  </code>
			 *
             *  @see BlockAllocated
             *
             *  \hideinitializer
             */
#if     qAllowBlockAllocation
#define DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)\
    static  void*   operator new (size_t n)     {   return ::operator new (n);  }\
    static  void    operator delete (void* p)   {   ::operator delete (p);      }
#else
#define DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_BlockAllocated_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockAllocated.inl"
