/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Bits_inl_
#define _Stroika_Foundation_Memory_Bits_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <climits>

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     *************************************** Bit ************************************
     ********************************************************************************
     */
    template <typename INT_TYPE>
    inline constexpr INT_TYPE Bit (unsigned int bitNumber)
    {
        Require (bitNumber < CHAR_BIT * sizeof (INT_TYPE));
        return (static_cast<INT_TYPE> (1) << bitNumber);
    }
    template <typename INT_TYPE, typename... BIT_ARGS>
    inline constexpr INT_TYPE Bit (unsigned int bitNumber, const BIT_ARGS&... args)
    {
        return Bit<INT_TYPE> (bitNumber) | Bit<INT_TYPE> (args...);
    }

    /*
     ********************************************************************************
     ********************************* BitSubstring *********************************
     ********************************************************************************
     */
    template <typename INT_TYPE>
    inline constexpr INT_TYPE BitSubstring (INT_TYPE bitField, unsigned int startOffset, unsigned int endOffset)
    {
        Require (startOffset <= endOffset);
        Require (startOffset <= (CHAR_BIT * sizeof (INT_TYPE)));
        if (startOffset == endOffset) {
            return 0;
        }
        Require (((endOffset - startOffset) - 1 + startOffset) < (CHAR_BIT * sizeof (INT_TYPE)));
        return (bitField >> startOffset) & ((1 << (endOffset - startOffset)) - 1);
    }

}

#endif /*_Stroika_Foundation_Memory_Bits_inl_*/
