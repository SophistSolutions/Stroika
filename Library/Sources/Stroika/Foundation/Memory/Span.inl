/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     *********************** Memory::CopySpanData_StaticCast ************************
     ********************************************************************************
     */
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopySpanData_StaticCast (span<const FROM_T, FROM_E> src, span<TO_T, TO_E> target)
    {
        // NOT YET [[maybe_unused]]auto a = SpanReInterpretCast<TO_T> (src);
#if 0
        return CopyBytes (SpanReInterpretCast<const TO_T, dynamic_extent> (src), target);
#else
        Require (src.size_bytes () <= target.size_bytes ());
        Require (not Intersects (src, target));
        TO_T* tb = target.data ();
        for (const auto& i : src) {
            *tb++ = static_cast<TO_T> (i);
        }
        return target.subspan (0, src.size ());
#endif
    }
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr std::span<TO_T, TO_E> CopySpanData_StaticCast (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target)
    {
        return CopySpanData_StaticCast (ConstSpan (src), target);
    }

}
