/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_h_
#define _Stroika_Foundation_Memory_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <bit>
#include <compare>
#include <memory>
#include <span>

#include "Stroika/Foundation/Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::Memory {

    /**
     *  \brief NEltsOf(X) returns the number of elements in array argument (ie return sizeof (arg)/sizeof(arg[0]))
     *
     *      @todo   Found std::begin() could be used to replace old StartOfArray() macro -
     *              see if this too can be replaced with something in C++11?
     */
    template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
    constexpr size_t NEltsOf ([[maybe_unused]] const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY]);

    /**
     *  API to return memory allocation statistics. Generally - these will be inaccurate,
     *  unless certain defines are set in Memory.cpp - but at least some stats can be
     *  returned in either case.
     *
     *  Note - this currently only produces useful answers on windows, but can easily pull
     *  similar values out of /proc fs stuff with linux (nyi).
     *
     *  @todo   Does this belong in "Execution" module"
     */
    struct GlobalAllocationStatistics {
        constexpr GlobalAllocationStatistics () = default;

        size_t fTotalOutstandingAllocations{0};
        size_t fTotalOutstandingBytesAllocated{0};
        size_t fPageFaultCount{0};
        size_t fWorkingSetSize{0};
        size_t fPagefileUsage{0};
    };
    GlobalAllocationStatistics GetGlobalAllocationStatistics ();

    /**
     *  \brief - like std::memcmp() - except count is in units of T (not bytes) and this function is
     *           constexpr, and this function allows nullptr arguments (if count == 0).
     * 
     *  Pointer Overload: 
     *      \req  (count == 0 or lhs != nullptr);
     *      \req  (count == 0 or rhs != nullptr);
     * 
     *  Span Overload: 
     *      \req  lhs.size () == rhs.size ()
     * 
     *  \note - like std::memcmp() it returns an int < 0 for less, == 0 for equal, and > 0 for greater, but that corresponds
     *          backward compatably to the strong_ordering C++20 type, so we use that for clarity going forward.
     */
    template <typename T>
    constexpr strong_ordering MemCmp (const T* lhs, const T* rhs, size_t count);
    template <typename T>
    constexpr strong_ordering MemCmp (span<const T> lhs, span<const T> rhs);
    template <typename T>
    constexpr strong_ordering MemCmp (span<T> lhs, span<T> rhs);

    /**
     *  \brief use Memory::OffsetOf(&CLASS::MEMBER) in place of offsetof(CLASS,MEMBER) to avoid compiler warnings, and cuz easier to 
     *         map from other constructors (e.g. StructFieldMetaInfo) cuz ptr to member legit C++ object, whereas CLASS and MEMBER are not.
     * 
     *  REPLACE calls to:
     *       offsetof (CLASS, MEMBER)
     * with:
     *       OffsetOf (&CLASS::MEMBER)
     * 
     *  \note   The current implementation exploits UNDEFINED BEHAVIOR.
     * 
     *          expr.add-5.sentence-2
     *              "If the expressions P and Q point to, respectively, elements x[i] and x[j] of 
     *              the same array object x, the expression P - Q has the value i - j; otherwise, the behavior is undefined.
     * 
     *  \par Example Usage
     *      \code
     *          struct  Person {
     *              String  firstName;
     *              String  lastName;
     *          };
     *          constexpr size_t kOffset_ = OffsetOf(&Person::lastName);
     *          static_assert (OffsetOf (&Person::firstName) == 0);         // NOTE - we WANT this to work, but for now cannot get constexpr stuff working
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          struct X1 {
     *              int a;
     *              int b;
     *          };
     *          void DoTest ()
     *          {
     *              assert (OffsetOf (&X1::a) == 0);
     *              assert (OffsetOf (&X1::b) >= sizeof (int));
     *          }
     *      \endcode
     * 
     *  @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516
     *  @see https://en.cppreference.com/w/cpp/types/offsetof
     *  @see https://stackoverflow.com/questions/65940393/c-why-the-restriction-on-offsetof-for-non-standard-layout-objects-or-how-t
     * 
     *  \note   Tricky to get this to work with constexpr. See implemtnation for details.
     */
    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    constexpr size_t OffsetOf (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember));

    /**
     *  \brief UninitializedConstructorFlag::eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  This is mainly used as a performance tweek, for objects that don't need to be initialized, and can just be copied into.
     * 
     *  \note the APIS that use this typically require  static_assert (is_trivially_copyable_v<T>);
     */
    enum class UninitializedConstructorFlag {
        eUninitialized
    };

    /**
     *  \brief eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  \see UninitializedConstructorFlag
     */
    using UninitializedConstructorFlag::eUninitialized;

    inline namespace Literals {
        /**
         *  \brief A utility for declaring constant bytes (byte literals).
         * 
         *  \req b <= 0xff
         * 
         *  \see https://stackoverflow.com/questions/75411756/how-do-i-declare-and-initialize-an-array-of-bytes-in-c
         */
        constexpr byte operator"" _b (unsigned long long b);
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Memory_Common_h_*/
