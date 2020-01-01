/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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

namespace Stroika::Foundation::Memory {

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
        static void* operator new (size_t n);
        static void* operator new (size_t n, int, const char*, int);
        static void  operator delete (void* p);
        static void  operator delete (void* p, int, const char*, int);
    };

    /**
     *  Use this to (roughly) undo the effect of BlockAllocationUseHelper<> or UseBlockAllocationIfAppropriate<> for a subclass.
     */
    template <typename T>
    struct BlockAllocationUseGlobalAllocatorHelper {
        static void* operator new (size_t n);
        static void* operator new (size_t n, int, const char*, int);
        static void  operator delete (void* p);
        static void  operator delete (void* p, int, const char*, int);
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
     *   \brief ManuallyBlockAllocated<T> is a simple wrapper on BlockAllocator<T>. If qAllowBlockAllocation defined, this will use block allocation for a given type - at a given call.
     *
     *   This is in sharp contrast to struct T : UseBlockAllocationIfAppropriate<T> {};
     *   If you use UseBlockAllocationIfAppropriate<> - the blockallocation strategy happens automatically for all new creations of objects of that type.
     *
     *   Using ManuallyBlockAllocated, only the particular places ManuallyBlockAllocated<T>::New ()/Delete are used will participate in block allocations, and other uses
     *   of T wont be block allocated. Note that means you MUST BE VERY CAREFUL with this and assure all objects allocated this way are deleted this way, and vice versa, and 
     *   don't mix with regular free-store.
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BlockAllocated.inl"

#endif /*_Stroika_Foundation_Memory_BlockAllocated_h_*/
