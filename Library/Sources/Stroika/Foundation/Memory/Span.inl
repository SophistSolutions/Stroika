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

}
#endif /*_Stroika_Foundation_Memory_Span_inl_*/
