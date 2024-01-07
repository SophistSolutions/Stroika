/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
 *  \def qStroika_Foundation_Memory_PreferBlockAllocation
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
#define qStroika_Foundation_Memory_PreferBlockAllocation
#endif

#if !defined(qStroika_Foundation_Memory_PreferBlockAllocation)
#error "qStroika_Foundation_Memory_PreferBlockAllocation should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation::Memory {

    /**
     *  Use this to force use of block allocation for a given type, by inheriting this class from that type (this ignores qStroika_Foundation_Memory_PreferBlockAllocation).
     *
     *  \note - typically DONT use this, but use UseBlockAllocationIfAppropriate intead.
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
     * 
     *  \note See also
     *      @see UsesBlockAllocation<T> ()
     */
    template <typename T>
    struct BlockAllocationUseHelper {
        static void* operator new (size_t n);
        static void* operator new (size_t n, int, const char*, int);
        static void  operator delete (void* p);
        static void  operator delete (void* p, int, const char*, int);
    };

    /**
     */
    template <typename T>
    constexpr bool UsesBlockAllocation ();

    /**
     *  \brief same as make_shared, but if type T has block allocation, then use block allocation for the 'shared part' of T as well.
     * 
     *  \note this is helpful for shared_ptr (performance), but not for unique_ptr<>.
     */
    template <typename T, typename... ARGS_TYPE>
    auto MakeSharedPtr (ARGS_TYPE&&... args) -> shared_ptr<T>;

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
     *                    : fStr{r}
     *                    , fCurIdx{idx}
     *                {
     *                    Require (fCurIdx <= fStr->_GetLength ());
     *                }
     *                ...
     *            };
     *      \endcode
     *
     *  If qStroika_Foundation_Memory_PreferBlockAllocation true (default) - this will use the optimized block allocation store, but if qStroika_Foundation_Memory_PreferBlockAllocation is
     *  false (0), this will just default to the global ::new/::delete
     *
     *  @see Stroika::Foundation::Memory::BlockAllocationUseHelper
     *  @see Stroika::Foundation::Memory::BlockAllocator
     *  @see Stroika::Foundation::Memory::AutomaticallyBlockAllocated
     *  @see Stroika::Foundation::Memory::ManuallyBlockAllocated
     */
    template <typename T, bool andTrueCheck = true>
    using UseBlockAllocationIfAppropriate =
        conditional_t<qStroika_Foundation_Memory_PreferBlockAllocation and andTrueCheck, BlockAllocationUseHelper<T>, Configuration::Empty>;

    /**
     *  \brief same idea as UseBlockAllocationIfAppropriate, except always 'inherits' from BASE_REP, so hides any existing static operator new/delete
     *         methods.
     *
     *     more CRTP style
     * 
     *  @todo - somewhat confusing implmentation - not sure why cannot be simpler?? -- LGP 2023-12-25
     * // couldn't get concepots working for this - on g++
     */
    template <typename DERIVED, typename BASE_REP, bool andTrueCheck = true>
    struct InheritAndUseBlockAllocationIfAppropriate : BASE_REP {
        template <typename... ARGS>
        InheritAndUseBlockAllocationIfAppropriate (ARGS&&... args)
            : BASE_REP{forward<ARGS> (args)...}
        {
        }
        static void* operator new (size_t n)
        {
            if constexpr (andTrueCheck) {
                return BlockAllocationUseHelper<DERIVED>::operator new (n);
            }
            else {
                return BASE_REP::operator new (n);
            }
        }
        static void* operator new (size_t n, int a, const char* b, int c)
        {
            if constexpr (andTrueCheck) {
                return BlockAllocationUseHelper<DERIVED>::operator new (n, a, b, c);
            }
            else {
                return BASE_REP::operator new (n, a, b, c);
            }
        }
        static void operator delete (void* p)
        {
            if constexpr (andTrueCheck) {
                BlockAllocationUseHelper<DERIVED>::operator delete (p);
            }
            else {
                return BASE_REP::operator delete (p);
            }
        }
        static void operator delete (void* p, int a, const char* b, int c)
        {
            if constexpr (andTrueCheck) {
                BlockAllocationUseHelper<DERIVED>::operator delete (p, a, b, c);
            }
            else {
                return BASE_REP::operator delete (p, a, b, c);
            }
        }
    };

    /**
     */
    template <typename T, bool andTrueCheck = true>
    using BlockAllocatorOrStdAllocatorAsAppropriate =
        conditional_t<qStroika_Foundation_Memory_PreferBlockAllocation and andTrueCheck, BlockAllocator<T>, std::allocator<T>>;

    /**
     *   \brief ManuallyBlockAllocated<T> is a simple wrapper on BlockAllocator<T>. If qStroika_Foundation_Memory_PreferBlockAllocation defined, this will use block allocation for a given type - at a given call.
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
