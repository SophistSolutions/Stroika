/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Bits_h_
#define _Stroika_Foundation_Memory_Bits_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"



/**
 * TODO:
 *      @todo   quick draft of bitfield helpers
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            template    <typename INT_TYPE>
            inline  INT_TYPE    TakeNBitsFrom (INT_TYPE bitField, unsigned int nBits, unsigned int offset)
            {
                // @todo add assertions bitrange in range...
                return ((bitField) >> (offset)) & ((1 << (nBits)) - 1);
            }
//#define TAKE_N_BITS_FROM(b, p, n) ((b) >> (p)) & ((1 << (n)) - 1)


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
