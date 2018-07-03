/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_h_
#define _Stroika_Foundation_Memory_BlockAllocated_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Empty.h"

#include "BlockAllocator.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  @todo   Document why we didnt use BlockAllocated<T> : T ... issue is that wouldnt work for non-class T, such
 *          as int.
 *
 *  @todo   Document how you can use USE_BLOCK_ALLOCATION style usage, or explicit BlockAllocated<T>
 *          usage (as in Memory::Optional).
 *
 *  @todo   We should either add a VARIANT or template parameter to BlockAllocated<> saying whether we
 *          should force block allocation, or ONLY block-allocate if size is appropriate to one of our
 *          preformed pools.
 *
 *          The reason for this option is for better use in templates like LinkedList<> - where we might
 *          want to blockallocate for small sizes of T, but not for ALL.
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
#if defined(__Doxygen__)
#define qAllowBlockAllocation
#endif

#if !defined(qAllowBlockAllocation)
#error "qAllowBlockAllocation should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation {
    namespace Memory {

        /**
         *  Use this to force use of block allocation for a given type, by inheriting this class from that type (this ignores qAllowBlockAllocation).
         *
         *  \par Example Usage:
         *      \code
         *            struct MyIterRep_ : Iterator<Character>::IRep, public Memory::BlockAllocationUseHelper<MyIterRep_> {
         *                _SharedPtrIRep fStr; // effectively RO, since if anyone modifies, our copy will remain unchanged
         *                size_t         fCurIdx;
         *                MyIterRep_ (const _SharedPtrIRep& r, size_t idx = 0)
         *                    : fStr (r)
         *                    , fCurIdx (idx)
         *                {
         *                    Require (fCurIdx <= fStr->_GetLength ());
         *                }
         *                ...
         *              };
         *      \endcode
         */
        template <typename T>
        struct BlockAllocationUseHelper {
            static void* operator new (size_t n) { return (Stroika::Foundation::Memory::BlockAllocator<T>::Allocate (n)); }
            static void* operator new (size_t n, int, const char*, int) { return (Stroika::Foundation::Memory::BlockAllocator<T>::Allocate (n)); }
            static void  operator delete (void* p) { Stroika::Foundation::Memory::BlockAllocator<T>::Deallocate (p); }
            static void  operator delete (void* p, int, const char*, int) { Stroika::Foundation::Memory::BlockAllocator<T>::Deallocate (p); }
        };

        /**
         *  Use this to undo the effect of BlockAllocationUseHelper<> or UseBlockAllocationIfAppropriate<> for a subclass.
         */
        template <typename T>
        struct BlockAllocationUseGlobalAllocatorHelper {
            static void* operator new (size_t n) { return ::operator new (n); }
            static void* operator new (size_t n, int, const char*, int) { return ::operator new (n); }
            static void  operator delete (void* p) { ::operator delete (p); }
            static void  operator delete (void* p, int, const char*, int) { ::operator delete (p); }
        };

        /**
         *  \brief  Use this to enable block allocation for a particular class. *Beware* of subclassing.
         *
         *  This utility class can be used to avoid some of the C++ gorp required in declaring that you are
         *  using block-allocation with a given class.
         *
         *  \par Example Usage:
         *      \code
         *            struct MyIterRep_ : Iterator<Character>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIterRep_> {
         *                _SharedPtrIRep fStr; // effectively RO, since if anyone modifies, our copy will remain unchanged
         *                size_t         fCurIdx;
         *                MyIterRep_ (const _SharedPtrIRep& r, size_t idx = 0)
         *                    : fStr (r)
         *                    , fCurIdx (idx)
         *                {
         *                    Require (fCurIdx <= fStr->_GetLength ());
         *                }
         *                ...
         *            };
         *      \endcode
         *
         *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store, but if qAllowBlockAllocation is
         *  false (0), this will just default to the global ::new/::delete
         *
         *  @see DECLARE_DONT_USE_BLOCK_ALLOCATION()
         *  @see Stroika::Foundation::Memory::BlockAllocator
         *  @see Stroika::Foundation::Memory::AutomaticallyBlockAllocated
         *  @see Stroika::Foundation::Memory::ManuallyBlockAllocated
         */
        template <typename T>
        using UseBlockAllocationIfAppropriate = conditional_t<qAllowBlockAllocation, BlockAllocationUseHelper<T>, Configuration::Empty>;

        /**
         *  \note DEPRECATED - USE UseBlockAllocationIfAppropriate
         */
#if qAllowBlockAllocation
#define DECLARE_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)                                                                                         \
    static void* operator new (size_t n) { return (Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Allocate (n)); }                        \
    static void* operator new (size_t n, int, const char*, int) { return (Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Allocate (n)); } \
    static void  operator delete (void* p) { Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Deallocate (p); }                             \
    static void  operator delete (void* p, int, const char*, int) { Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Deallocate (p); }
#else
#define DECLARE_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)
#endif

// Deprecated in Stroika v2.1d4 - USE UseBlockAllocationIfAppropriate
#define DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS) use = "DECLARE_USE_BLOCK_ALLOCATION_DEPRECATED"

        /**
             */
// Deprecated in Stroika v2.1d4 - USE UseBlockAllocationIfAppropriate
#if qAllowBlockAllocation
#define DECLARE_DONT_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)        \
    static void* operator new (size_t n) { return ::operator new (n); } \
    static void  operator delete (void* p) { ::operator delete (p); }
#else
#define DECLARE_DONT_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)
#endif

// Deprecated in Stroika v2.1d4 - USE BlockAllocationUseGlobalAllocatorHelper
#define DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS) use = "DECLARE_DONT_USE_BLOCK_ALLOCATION_DEPRECATED"

        /**
         * \brief  Utility class to implement special memory allocator pattern which can greatly improve performance - @see DECLARE_USE_BLOCK_ALLOCATION()
         *
         * AutomaticallyBlockAllocated<T> is a templated class designed to allow easy use
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
         *
         *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store, but if qAllowBlockAllocation is
         *  false (0), this will just default to the global ::new/::delete
         */
        template <typename T>
        class AutomaticallyBlockAllocated : public UseBlockAllocationIfAppropriate<T> {
        public:
            /**
             * @todo Clean this section of code (BlockAllocated) up. See if some better way to wrap type T, with extras.
             *      something that does good job forwarding CTOR arguments (perfect forwarding?) and does a better job
             *      with stuff like operator==, operaotr<, etc... (maybe explicitly override  each)?
             */
            AutomaticallyBlockAllocated ();
            AutomaticallyBlockAllocated (const AutomaticallyBlockAllocated& t);
            AutomaticallyBlockAllocated (const T& t);
            AutomaticallyBlockAllocated (T&& t);

        public:
            nonvirtual const AutomaticallyBlockAllocated<T>& operator= (const AutomaticallyBlockAllocated& t);
            nonvirtual const AutomaticallyBlockAllocated<T>& operator= (const T& t);

        public:
            nonvirtual operator T () const;

        public:
            nonvirtual T* get ();

        private:
            T fValue_;
        };

        /**
         *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store,
         *  but if qAllowBlockAllocation is false (0), this will just default to the global ::new/::delete
         *  NEW EXPERIMENTAL POSSIBLE REPLACEMNT FOR BlockAllocated<>
         *  LGP 2014-02-27
         *
         *  @todo CLEANUP THIS DOC SECITON
         *
         *  maybe just augoemtn - this is where you must save a T* and return that easil. Maybe existing BlockAllocated best for
         *  when you can afford to hang onto the wrapper BlockAllocated<T>?
         *
         *  maybe call this ManuallyBlockAllocated, and the other one AutomaticallyBlockAllocated()??
         */
        template <typename T>
        class ManuallyBlockAllocated {
        public:
            /**
                 */
            template <typename... ARGS>
            static T* New (ARGS&&... args);

        public:
            /**
                 */
            static void Delete (T* p) noexcept;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BlockAllocated.inl"

#endif /*_Stroika_Foundation_Memory_BlockAllocated_h_*/
