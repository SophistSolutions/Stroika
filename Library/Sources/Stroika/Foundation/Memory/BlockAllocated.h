/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 *
 *  @todo   Consider doing GetMem_Util_ code outside of the context of the lock-gaurd, and if
 *          we get multiple results, just patch them into the linked list. That way in case of
 *          multithreading (when we're paging in freepool) - we'll do less busy waiting.
 *
 *  @todo   Document why we didnt use BlockAllocated<T> : T ... issue is that wouldnt work for non-class T, such
 *          as int.
 *
 *  @todo   BlockAllocated<T> could hugely benefit from some optimistic locking
 *          strategy - like in http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3341.pdf
 *
 *          Make sure when we experiment with that - we include it here as one of our first
 *          optimization points!
 *
 *  @todo   Could decouple the BlockAllocator (move to separate file along with pool spec)
 *          from BlockAllocated<>. And maybe use TRAITS on BlockAllocator to define some stuff
 *          about strategies (options) - like how to share pools?
 *
 *  @todo   Comments generally need a thorough review. Many VERY VERY old - from Led days.
 *
 *  @todo   Document how you can use USE_BLOCK_ALLOCATION style usage, or explicit BlockAllocated<T>
 *          usage (as in Memory::Optional).
 *
 *  @todo   Fix all the BlockAllocated<T>::fValue_ code - clean it up, put it in INL file, and
 *          think through more carefully CTORs etc - to make it act more like a "T" type.
 *
 *          Sterl thinks its best to just add a generic solution like:
 *              template <typename T>
 *              struct  AsObject {
 *                  AsObject ()    {    }
 *
 *                  AsObject (T value) :         fValue (value)    {    }
 *
 *                  nonvirtual  operator T () const    {        return fValue;    }
 *                  nonvirtual  operator T ()    {        return fValue;    }
 *
 *                  T fValue;
 *              };
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
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename   T>
            class   BlockAllocator  {
            public:
                /**
                 *  \req (n == sizeof (T))
                 */
                static  void*   Allocate (size_t n);

            public:
                /**
                 *  \req (p allocated by BlockAllocator<T>::Allocate ());
                 *  p can be nullptr
                 */
                static  void    Deallocate (void* p);

            public:
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
             *  @see Stroika::Foundation::Memory::BlockAllocator
             *  @see Stroika::Foundation::Memory::BlockAllocated
             *
             *  \hideinitializer
             */
#if     qAllowBlockAllocation
#define DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)\
    static  void*   operator new (size_t n)                         {   return (Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Allocate (n)); }\
    static  void*   operator new (size_t n,int,const char*,int)     {   return (Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Allocate (n)); }\
    static  void    operator delete (void* p)                       {   Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Deallocate (p);       }\
    static  void    operator delete (void* p,int,const char*,int)   {   Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Deallocate (p);       }
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
                BlockAllocated ();
                BlockAllocated (const BlockAllocated<T>& t);
                BlockAllocated (const T& t);
                BlockAllocated (T&&  t);
            public:
                nonvirtual const BlockAllocated<T>& operator= (const BlockAllocated<T>& t);
                nonvirtual const BlockAllocated<T>& operator= (const T& t);

            public:
                nonvirtual  operator T () const;

            public:
                nonvirtual  T* get ();

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
