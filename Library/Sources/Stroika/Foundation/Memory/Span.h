/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Span_h_
#define _Stroika_Foundation_Memory_Span_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <span>

#include "../Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::Memory {

    /**
     *  \brief convert a (possibly already const) span to the same span, but with const on the 'T' argument
     *
     *  I hope this function isnt needed often. And whereever it is needed, I think is a std::span<> design
     *  defect (or something I don't yet understand about how to use span better).
     * 
     *  See https://stackoverflow.com/questions/62688814/stdspanconst-t-as-parameter-in-function-template
     */
    template <class T, size_t EXTENT>
    constexpr std::span<const T, EXTENT> ConstSpan (std::span<T, EXTENT> s);

    /*
     *  \brief Span-flavored memcpy/std::copy (copies from, to)
     *
     *  like std::copy, except copies the data the spans point to/reference. Target span maybe larger,
     *  but must (require) be no smaller than src span.
     *  
     *  Returns the subset of the target span filled (so a subspan of target).
     */
    template <typename T>
    constexpr std::span<T> CopySpanData (span<const T> src, span<T> target);
    template <typename T>
    constexpr std::span<T> CopySpanData (span<T> src, span<T> target);

    /*
     *  \brief Span-flavored memcpy/std::copy (copies from, to), but with cast (like CopySpanData but with cast)
     *
     *  Same as CopySpanData, except does 'static cast' on data being copied
     * 
     *  \req sizeof (FROM_T) == sizeof (TO_T)       ; for now - consider losing this requirement someday if convenient/need
     */
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<const FROM_T> src, span<TO_T> target)
       requires (sizeof (FROM_T) == sizeof (TO_T));
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<FROM_T> src, span<TO_T> target)
        requires (sizeof (FROM_T) == sizeof (TO_T));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Span.inl"

#endif /*_Stroika_Foundation_Memory_Span_h_*/
