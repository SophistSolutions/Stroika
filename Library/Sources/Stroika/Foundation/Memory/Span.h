/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Span_h_
#define _Stroika_Foundation_Memory_Span_h_ 1

#include "../StroikaPreComp.h"

#include <span>
#include <string>

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
     *  But it appears if you declare function argument span<const T,EXT> and pass in a non-const T span, you get
     *  error messages about failure to deduce (at least on visual studio).
     * 
     *  See https://stackoverflow.com/questions/62688814/stdspanconst-t-as-parameter-in-function-template
     */
    template <class T, size_t EXTENT>
    constexpr std::span<const T, EXTENT> ConstSpan (std::span<T, EXTENT> s);

    /**
     *  \brief use ISpanOfT<T> as a concept declaration for parameters where you want a span, but accept either T or const T
     * 
     *  Sadly, if I declare a function
     *      f (span<int>) {}
     *  and try to call it with:
     *      f (span<const int>{}) - that fails, whereas I think, considering the logic/intent, it probably should work.
     * 
     *  Anyhow, replacing the f declaration with the (almost as clear);
     *      template <ISpanOfT<int> SPAN_OF_T>
     *      f (SPAN_OF_T) {}
     * 
     *  fixes the problem.
     * 
     *  \note Aliases:
     *      SpanOfPossiblyConstT - but name just too long (though might be clearer).
     * 
     *  note - matches span<T>, span<T,EXTENT>, span<const T>, span<const T,EXTENT>, but not things that
     * 
     *  are CONVERTIBLE to span<T>
     *  \see https://stackoverflow.com/questions/62688814/stdspanconst-t-as-parameter-in-function-template
     */
    template <typename SPAN_T, typename T>
    concept ISpanOfT = same_as<remove_cvref_t<SPAN_T>, span<T>> or same_as<remove_cvref_t<SPAN_T>, span<const T>> or
                       same_as<remove_cvref_t<SPAN_T>, span<T, SPAN_T::extent>> or same_as<remove_cvref_t<SPAN_T>, span<const T, SPAN_T::extent>>;
    static_assert (ISpanOfT<span<int>, int> and ISpanOfT<span<const int>, int> and ISpanOfT<span<const int, 3>, int> and not ISpanOfT<span<int>, char>);

    namespace Private_ {
        template <class>
        inline constexpr bool _Is_span_v = false;
        template <class _Ty, size_t _Extent>
        inline constexpr bool _Is_span_v<span<_Ty, _Extent>> = true;
    }
    /**
     *  For when you want to assert an argument is a SPAN, but you haven't yet deduced the type its a span of yet.
     * 
     *  \note matches span<T>, span<T,EXTENT>, span<const T>, span<const T,EXTENT>, but not things that
     *  are CONVERTIBLE to span<T>
     */
    template <typename SPAN_T>
    concept ISpan = Private_::_Is_span_v<SPAN_T>;
    static_assert (ISpan<span<int>> and ISpan<span<int, 3>>);
    static_assert (not ISpan<std::string> and not ISpan<int>); // we don't include <string> in this module, but sometimes helpful to test/debug/document

    /**
     * \brief return true iff intersection of the two spans is non-empty (contains any bytes)
     * 
     *  Note this is similar to Range::Intersects, except for the business about openness/closedness and details at the edge conditions
     * 
     *  The only known use for this is assertions in CopySpanData that the spans don't overlap (memcpy vs memmove)
     */
    template <typename T1, typename T2, size_t E1, size_t E2>
    constexpr bool Intersects (std::span<T1, E1> lhs, std::span<T2, E2> rhs);

    /**
     *  Reinterpret a span of one thing, into a span of another (of the same total size in bytes).
     * 
     *  This requirement of the same size in bytes means sizeof FROM_T must evently divide sizeof TO_T
     */
    template <typename TO_T, typename FROM_T, size_t FROM_EXTENT>
    constexpr std::span<TO_T> SpanReInterpretCast (span<FROM_T, FROM_EXTENT> src)
        requires (sizeof (FROM_T) % sizeof (TO_T) == 0);

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
     * 
     *          @todo MAYBE RENAME TO MemMove()????
     */
    template <typename T, size_t E>
    constexpr std::span<T, E> CopySpanData (span<const T, E> src, span<T, E> target);
    template <typename T, size_t E>
    constexpr std::span<T, E> CopySpanData (span<T, E> src, span<T, E> target);

    /*
     *  \brief Span-flavored memcpy/std::copy (copies from, to), but with cast (like CopySpanData but with cast)
     *
     *  Same as CopySpanData, except does 'static cast' on data being copied.
     * 
     *  \returns the subspan of the target which was just filled in.
     * 
     *  \note sizeof (FROM_T) may differ from sizeof (TO_T). So this can be used to downshift char16_t data
     *        to plain ASCII so long as the caller assures the source data is truely ascii first.
     * 
     *  \req not Intersects (src, target) - so non-overlapping
     * 
     *          @todo MAYBE RENAME TO MemMove()????- STATICCAST
     */
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr std::span<TO_T, TO_E> CopySpanData_StaticCast (span<const FROM_T, FROM_E> src, span<TO_T, TO_E> target);
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr std::span<TO_T, TO_E> CopySpanData_StaticCast (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Span.inl"

#endif /*_Stroika_Foundation_Memory_Span_h_*/
