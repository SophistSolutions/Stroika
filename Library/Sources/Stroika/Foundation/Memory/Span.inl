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
#include <cstring>

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
     *************************** Memory::CopySpanData *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr std::span<T> CopySpanData (span<const T> src, span<T> target)
    {
        Require (src.size () <= target.size ());
        std::copy (src.begin (), src.end (), target.data ());
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
        requires (sizeof (FROM_T) == sizeof (TO_T))
    {
        Require (src.size () <= target.size ());
        TO_T* tb = target.data ();
        for (const auto& i : src) {
            *tb++ = static_cast<TO_T> (i);
        }
        return target.subspan (0, src.size ());
    }
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<FROM_T> src, span<TO_T> target)
        requires (sizeof (FROM_T) == sizeof (TO_T))
    {
        return CopySpanData_StaticCast (ConstSpan (src), target);
    }

}
#endif /*_Stroika_Foundation_Memory_Span_inl_*/
