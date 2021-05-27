/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_h_
#define _Stroika_Foundation_Memory_Common_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"

/**
 * TODO:
 */

namespace Stroika::Foundation::Memory {

    /**
     *  \brief NEltsOf(X) returns the number of elements in array X
     *
     *      @todo   Found std::begin() could be used to replace old StartOfArray() macro -
     *              see if this too can be replaced with something in C++11?
     */
    template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
    inline constexpr size_t NEltsOf_REAL_ ([[maybe_unused]] const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY])
    {
        return SIZE_OF_ARRAY;
    }
// after testing - do this - cuz otherwise use in constant array arg wont work
#define NEltsOf(X) Stroika::Foundation::Memory::NEltsOf_REAL_ (X)

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
        GlobalAllocationStatistics ();

        size_t fTotalOutstandingAllocations;
        size_t fTotalOutstandingBytesAllocated;
        size_t fPageFaultCount;
        size_t fWorkingSetSize;
        size_t fPagefileUsage;
    };
    GlobalAllocationStatistics GetGlobalAllocationStatistics ();

    /**
     *  \brief - like std::memcmp() - except count is in units of T (not bytes) and this function is
     *           constexpr.
     *
     *      \req  (count == 0 or lhs != nullptr);
     *      \req  (count == 0 or rhs != nullptr);
     */
    template <typename T>
    constexpr int MemCmp (const T* lhs, const T* rhs, size_t count);

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
    namespace Private {
        /**
         *  Sadly, using pragmas to disable warnings didn't work with VC11. VC_BWA_std_copy
         *  provides a little shared code for a worakround - implementation of std::copy that - while probably
         *  inferior to the default - at least isn't much inferior, and doesn't give those
         *  annoying warnings.
         */
        template <class _InIt, class _OutIt>
        inline void VC_BWA_std_copy (_InIt _First, _InIt _Last, _OutIt _Dest);
    }
#endif

    /**
     *  REPLACE calls to:
     *       offsetof (CLASS, MEMBER)
     * with:
     *       OffsetOf (&CLASS::MEMBER)
     * 
     *  \note   This exploits UNDEFINED BEHAVIOR.
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
     *          static_assert (OffsetOf (&Person::firstName) == 0);
     *      \endcode
     *
     * 
     *  @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516
     *  @see https://en.cppreference.com/w/cpp/types/offsetof
     *  @see https://stackoverflow.com/questions/65940393/c-why-the-restriction-on-offsetof-for-non-standard-layout-objects-or-how-t
     * 
     *  \note   Tricky to get this to work with constexpr. Works on clang++ and g++, but not VisualStudio
     *          really only constexpr if !qCompilerAndStdLib_OffsetOf_Constexpr_Buggy and is_default_constructible_v<OWNING_OBJECT>
     */
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT, enable_if_t<is_default_constructible_v<OWNING_OBJECT>>* = nullptr>
    size_t constexpr OffsetOf (FIELD_VALUE_TYPE OWNING_OBJECT::*member);
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT, enable_if_t<not is_default_constructible_v<OWNING_OBJECT>>* = nullptr>
    size_t OffsetOf (FIELD_VALUE_TYPE OWNING_OBJECT::*member);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Memory_Common_h_*/
