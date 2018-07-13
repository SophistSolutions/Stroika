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
 *  @todo   Document why we didnt use BlockAllocated<T> : T ... issue is that wouldnt work for non-class T, such
 *          as int.
 *
 *  @todo   We should either add a VARIANT or template parameter to BlockAllocated<> saying whether we
 *          should force block allocation, or ONLY block-allocate if size is appropriate to one of our
 *          preformed pools.
 *
 *          The reason for this option is for better use in templates like LinkedList<> - where we might
 *          want to blockallocate for small sizes of T, but not for ALL.
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
         *  @see Stroika::Foundation::Memory::BlockAllocationUseHelper
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
         *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store, but if qAllowBlockAllocation is
         *  false (0), this will just default to the global ::new/::delete
         *
         */
        template <typename T>
        [[deprecated ("bad idea - if you use to allocate - must retain type AutomaticallyBlockAllocated<T>* or do wrong delete - deprecated in v2.1d4")]] class AutomaticallyBlockAllocated : public UseBlockAllocationIfAppropriate<T> {
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
        *   \brief ManuallyBlockAllocated<T> is a simple wrapper on BlockAllocator<T>. If qAllowBlockAllocation defined, this will use block allocation for a given type - at a given call.
        *
        *   This is in sharp contrast to struct T : UseBlockAllocationIfAppropriate<T> {};
        *   If you use UseBlockAllocationIfAppropriate<> - the blockallocation strategy happens automatically for all new creations of objects of that type.
        *
        *   Using ManuallyBlockAllocated, only the particular places ManuallyBlockAllocated<T>::New ()/Delete are used will participate in block allocations, and other uses
        *   of T wont be block allocated. Note that means you MUST BE VERY CAREFUL with this and assure all objects allocated this way are deleted this way, and vice versa, and 
        *   dont mix with regular free-store.
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
