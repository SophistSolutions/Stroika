/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Bits_h_
#define _Stroika_Foundation_Memory_Bits_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"



/**
 *  \file
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *
             */
            template    <typename INT_TYPE = unsigned int>
            constexpr   INT_TYPE    Bit (unsigned int bitNumber);
            template    <typename INT_TYPE, typename... BIT_ARGS>
            constexpr   INT_TYPE    Bit (unsigned int bitNumber, const BIT_ARGS& ... args);


            /**
             *
             */
            template    <typename INT_TYPE>
            constexpr   INT_TYPE    TakeNBitsFrom (INT_TYPE bitField, unsigned int nBits, unsigned int offset);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Bits.inl"

#endif  /*_Stroika_Foundation_Memory_Bits_h_*/
