/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_h_
#define _Stroika_Foundation_Memory_BlockAllocated_h_    1

#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstddef>

#include    "../Configuration/Common.h"

#include    "../Execution/ModuleInit.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  @todo   Document how you can use USE_BLOCK_ALLOCATION style usage, or explicit BlockAllocated<T>
 *          usage (as in Memory::Optional).
 *
 *  @todo   Fix all the fValue_ code - clean it up, put it in INL file, and think through more carefully
 *          CTORs etc - to make it act more like a "T" type.
 *
 *          Closely related - document WHY we didn't just have BlockAllocated<T> inherit from T (issue is
 *          when T = int, for example. There maybe some template magic I can use to fix that, and then
 *          thats probably a better solution!
 *
 *  @todo   We should either add a VARIANT or template parameter to BlockAllocated<> saying whether we
 *          should force block allocation, or ONLY block-allocate if size is appropriate to one of our
 *          preformed pools.
 *
 *          The reason for this option is for better use in templates like LinkedList<> - where we might
 *          want to blockallocate for small sizes of T, but not for ALL.
 *
 *  @todo   BlockAllocationSupport<>::Compact ()
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





/**
 *  \def qAllowBlockAllocation
 *
 *  \brief  Allow use of block-allocation im classes which uses DECLARE_USE_BLOCK_ALLOCATION()
 *
 *  Allow use of block-allocation. The main reason to disable it indescriminantly
 *  is for debugging purposes (looking for memory leaks). But others may have other
 *  reasons.
 *
 *      Defaults to 1
 *
 *  \hideinitializer
 */
#if     defined (__Doxygen__)
#define qAllowBlockAllocation
#endif


#if     !defined (qAllowBlockAllocation)
#error "qAllowBlockAllocation should normally be defined indirectly by StroikaConfig.h"
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  Low-level tool to allocate and free memory from a fixed size/element pool. Very high performance since
             *  no searching or coalescing ever needed, but at the cost of creating some amount of fragmentation.
             *
             *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store, but if qAllowBlockAllocation is
             *  false (0), this will just default to the global ::new/::delete
             */
            template    <typename   T>
            class   BlockAllocationSupport  {
            public:
                static  void*   Allocate (size_t n);
                static  void    Deallocate (void* p);
                /**
                  * Return to the free store all deallocated blocks whcih can be returned.
                  *
                  * This takes oN, where N is the total amount of operator new for this size.
                  *
                  * It also currently uses a large block of contiguous memory, which could fail.
                  *
                  * But it might sometimes return memory from a particular data structure (fixed element size pool) to
                  * the general free store.
                  */
                static  void    Compact ();
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
             *  @see Stroika::Foundation::Memory::BlockAllocationSupport
             *  @see Stroika::Foundation::Memory::BlockAllocated
             *
             *  \hideinitializer
             */
#if     qAllowBlockAllocation
#define DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)\
    static  void*   operator new (size_t n)                         {   return (Stroika::Foundation::Memory::BlockAllocationSupport<THIS_CLASS>::Allocate (n)); }\
    static  void*   operator new (size_t n,int,const char*,int)     {   return (Stroika::Foundation::Memory::BlockAllocationSupport<THIS_CLASS>::Allocate (n)); }\
    static  void    operator delete (void* p)                       {   Stroika::Foundation::Memory::BlockAllocationSupport<THIS_CLASS>::Deallocate (p);       }\
    static  void    operator delete (void* p,int,const char*,int)   {   Stroika::Foundation::Memory::BlockAllocationSupport<THIS_CLASS>::Deallocate (p);       }
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
             *  @see DECLARE_USE_BLOCK_ALLOCATION
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



            /**
              * \brief  Utility class to implement special memory allocator pattern which can greatly improve performance - @see DECLARE_USE_BLOCK_ALLOCATION()
              *
              * BlockAllocated<T> is a templated class designed to allow easy use
              * of a block-allocated memory strategy. This means zero overhead malloc/memory allocation for fixed
              * size blocks (with the only problem being the storage is never - or almost never - returned to the
              * free store - it doesn't leak - but cannot be used for other things. This is often a useful
              * tradeoff for things you allocate a great number of.
              *
              * You shouldn't disable it lightly. But you may wish to temporarily disable block-allocation
              * while checking for memory leaks by shutting of the qAllowBlockAllocation compile-time configuration variable.
              *
              * Note also - you can avoid some of the uglines of the overload declarations by using the
              * DECLARE_USE_BLOCK_ALLOCATION() macro.
            */
            template    <typename   T>
            class   BlockAllocated   {
            public:
                DECLARE_USE_BLOCK_ALLOCATION(T);

            public:
                /**
                 * @todo Clean this section of code (BlockAllocated) up. See if some better way to wrap type T, with extras.
                 *      something that does good job forwarding CTOR arguments (perfect forwarding?) and does a better job
                 *      with stuff like operator==, operaotr<, etc... (maybe explicitly override  each)?
                 */
                BlockAllocated () : fValue_ () {}
                BlockAllocated (const BlockAllocated<T>& t) : fValue_ (t) {}
                BlockAllocated (const T& t) : fValue_ (t) {}
                BlockAllocated (T && t) : fValue_ (std::move (t)) {}
                nonvirtual const BlockAllocated<T>& operator= (const BlockAllocated<T>& t) {
                    fValue_ = t.fValue_;
                    return *this;
                }
                nonvirtual const BlockAllocated<T>& operator= (const T& t) {
                    fValue_ = t;
                    return *this;
                }
                nonvirtual  operator T () const {
                    return fValue_;
                }
                nonvirtual  T* get () {
                    return &fValue_;
                }

            private:
                T   fValue_;
            };


            /**
             *  This can be referenced in your ModuleInit<> to force correct inter-module construction order.
             */
            Execution::ModuleDependency MakeModuleDependency_BlockAllocated ();


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockAllocated.inl"

#endif  /*_Stroika_Foundation_Memory_BlockAllocated_h_*/
