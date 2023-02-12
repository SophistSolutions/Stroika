/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Span_h_
#define _Stroika_Foundation_Memory_Span_h_ 1

#include "../StroikaPreComp.h"

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

    /**
    * @todo RESPOND TO See https://stackoverflow.com/questions/62688814/stdspanconst-t-as-parameter-in-function-template
    * POINT THIS SOLUTION OUT
     */
    template <typename SPAN_T, typename T>
    concept SpanOfT = is_same_v<remove_cvref_t<SPAN_T>, span<T>> or is_same_v<remove_cvref_t<SPAN_T>, span<const T>>;

    /**
     * \brief return true iff intersection of the two spans is non-empty
     * 
     *  The only known use for this is assertions in CopySpanData that the spans don't overlap (memcpy vs memmove)
     */
    template <typename T1, typename T2, size_t E1, size_t E2>
    constexpr bool Intersects (std::span<T1, E1> lhs, std::span<T2, E2> rhs);

    /*
     *  \brief Span-flavored memcpy/std::copy (copies from, to)
     *
     *  like std::copy, except copies the data the spans point to/reference. Target span maybe larger than src,
     *  but must (require) be no smaller than src span.
     * 
     *  \req src.size () <= target.size ()
     *  \req not Intersects (src, target) - so non-overlapping
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
     *  Same as CopySpanData, except does 'static cast' on data being copied.
     * 
     *  \returns the subspan of the target which was just filled in.
     * 
     *  \note sizeof (FROM_T) may differ from sizeof (TO_T). So this can be used to downshift char16_t data
     *        to plain ASCII (Character_ASCII) so long as the caller assures the source data is truely ascii first.
     * 
     *  \req not Intersects (src, target) - so non-overlapping
     */
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<const FROM_T> src, span<TO_T> target);
    template <typename FROM_T, typename TO_T>
    constexpr std::span<TO_T> CopySpanData_StaticCast (span<FROM_T> src, span<TO_T> target);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Span.inl"

#endif /*_Stroika_Foundation_Memory_Span_h_*/
