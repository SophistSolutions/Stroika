/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Span_inl_
#define _Stroika_Foundation_Memory_Span_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ***************************** Memory::ConstSpan ********************************
     ********************************************************************************
     */
    template <class T, size_t EXTENT>
    constexpr std::span<const T, EXTENT> ConstSpan (std::span<T, EXTENT> s)
    {
        return s;
    }

    /*
     ********************************************************************************
     ***************************** Memory::Intersects *******************************
     ********************************************************************************
     */
    template <typename T1, typename T2, size_t E1, size_t E2>
    constexpr bool Intersects (std::span<T1, E1> lhs, std::span<T2, E2> rhs)
    {
        // See Range<T, TRAITS>::Intersects for explanation - avoid direct call here to avoid include file refrence
        auto lhsStart = as_bytes (lhs).data ();
        auto rhsStart = as_bytes (rhs).data ();
        auto lhsEnd   = lhsStart + lhs.size_bytes ();
        auto rhsEnd   = rhsStart + rhs.size_bytes ();
        if (rhsEnd < lhsStart) {
            return false;
        }
        if (rhsStart > lhsEnd) {
            return false;
        }
        if (lhs.empty () or rhs.empty ()) {
            return false;
        }
        if (rhsEnd == lhsStart) {
            return true;
        }
        if (rhsStart == lhsEnd) {
            return true;
        }
        return true;
    }

    /*
     ********************************************************************************
     *************************** Memory::CopySpanData *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr std::span<T> CopySpanData (span<const T> src, span<T> target)
    {
        Require (src.size () <= target.size ());
        Require (not Intersects (src, target));
#if qCompilerAndStdLib_copy_warning_overflow_Buggy
        auto targetOutputIterator = target.begin ();
        for (const auto& elt : src) {
            *targetOutputIterator++ = elt;
        }
#else
        std::copy (src.begin (), src.end (), target.data ());
#endif
        return target.subspan (0, src.size ());
    }
    template <typename T>
    constexpr std::span<T> CopySpanData (span<T> src, span<T> target)
    {
        return CopySpanData (ConstSpan (src), target);
    }

    /*
     ********************************************************************************
     *********************** Memory::CopySpanData_StaticCast ************************
     ********************************************************************************
     */
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<const FROM_T> src, span<TO_T> target)
    {
        Require (src.size () <= target.size ());
        Require (not Intersects (src, target));
        TO_T* tb = target.data ();
        for (const auto& i : src) {
            *tb++ = static_cast<TO_T> (i);
        }
        return target.subspan (0, src.size ());
    }
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<FROM_T> src, span<TO_T> target)
    {
        return CopySpanData_StaticCast (ConstSpan (src), target);
    }

}
#endif /*_Stroika_Foundation_Memory_Span_inl_*/