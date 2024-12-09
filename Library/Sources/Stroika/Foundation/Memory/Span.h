/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Span_h_
#define _Stroika_Foundation_Memory_Span_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <span>
#include <string>

#include "Stroika/Foundation/Common/Common.h"

/**
 */

namespace Stroika::Foundation::Memory {

    //&&&&&
    // new experimetnal APIS
    // see if we can depreact CopySpanData_StaticCast using CopyBytes(SpanReInterpretCast())
    //

    /*
     *  \brief Span-flavored memcpy/std::copy (copies from, to), but with cast (like CopySpanData but with cast)
     *
     *  Same as CopySpanData, except does 'static cast' on data being copied.
     * 
     *  \req src.size_bytes () <= target.size_bytes ()      -- so that all of source can always be copied (else would need api/indicator of how much copied)
     *  \req not Intersects (src, target) - so non-overlapping
     * 
     *  \returns the subspan of the target which was just filled in.
     * 
     *  \note sizeof (FROM_T) may differ from sizeof (TO_T). So this can be used to downshift char16_t data
     *        to plain ASCII so long as the caller assures the source data is truly ascii first.
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
