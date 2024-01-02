/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
        // See Range<T, TRAITS>::Intersects for explanation - avoid direct call here to avoid include file reference
        auto lhsStart = as_bytes (lhs).data ();
        auto rhsStart = as_bytes (rhs).data ();
        auto lhsEnd   = lhsStart + lhs.size_bytes ();
        auto rhsEnd   = rhsStart + rhs.size_bytes ();
        if (rhsEnd <= lhsStart) {
            return false;
        }
        if (rhsStart >= lhsEnd) {
            return false;
        }
        if (lhs.empty () or rhs.empty ()) {
            return false;
        }
        return true;
    }

    /*
     ********************************************************************************
     *************************** Memory::SpanReInterpretCast ************************
     ********************************************************************************
     */
    template <typename TO_T, typename FROM_T, size_t FROM_EXTENT>
    constexpr std::span<TO_T> SpanReInterpretCast (span<FROM_T, FROM_EXTENT> src)
        requires (sizeof (FROM_T) % sizeof (TO_T) == 0)
    {
        return span<TO_T>{reinterpret_cast<TO_T*> (src.data ()), src.size () * (sizeof (FROM_T) / sizeof (TO_T))};
    }

    /*
     ********************************************************************************
     *************************** Memory::CopySpanData *******************************
     ********************************************************************************
     */
    template <typename T, size_t E>
    constexpr std::span<T, E> CopySpanData (span<const T, E> src, span<T, E> target)
    {
        Require (src.size () <= target.size ());
        Require (not Intersects (src, target));
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wstringop-overflow\""); // this suppress doesn't work for g++-11, so must use configure to add suppress to cmdline
        std::copy (src.begin (), src.end (), target.data ());
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wstringop-overflow\"");
        return target.subspan (0, src.size ());
    }
    template <typename T, size_t E>
    constexpr std::span<T, E> CopySpanData (span<T, E> src, span<T, E> target)
    {
        return CopySpanData (ConstSpan (src), target);
    }

    /*
     ********************************************************************************
     *********************** Memory::CopySpanData_StaticCast ************************
     ********************************************************************************
     */
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr std::span<TO_T, TO_E> CopySpanData_StaticCast (span<const FROM_T, FROM_E> src, span<TO_T, TO_E> target)
    {
        Require (src.size () <= target.size ());
        Require (not Intersects (src, target));
        TO_T* tb = target.data ();
        for (const auto& i : src) {
            *tb++ = static_cast<TO_T> (i);
        }
        return target.subspan (0, src.size ());
    }
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr std::span<TO_T, TO_E> CopySpanData_StaticCast (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target)
    {
        return CopySpanData_StaticCast (ConstSpan (src), target);
    }

}
#endif /*_Stroika_Foundation_Memory_Span_inl_*/
