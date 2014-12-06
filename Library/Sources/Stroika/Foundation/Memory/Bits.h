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
             *  bitNumber's start with 0, not 1.
             *
             *  EXAMPLES:
             *      VerifyTestResult (Bit (0) == 0x1);
             *      VerifyTestResult (Bit (1) == 0x2);
             *      VerifyTestResult (Bit (3) == 0x8);
             *      VerifyTestResult (Bit (15) == 0x8000);
             *      VerifyTestResult (Bit<int> (1, 2) == 0x6);
             *      VerifyTestResult (Bit<int> (1, 2, 15) == 0x8006);
             */
            template    <typename INT_TYPE = unsigned int>
            constexpr   INT_TYPE    Bit (unsigned int bitNumber);
            template    <typename INT_TYPE, typename... BIT_ARGS>
            constexpr   INT_TYPE    Bit (unsigned int bitNumber, const BIT_ARGS& ... args);


            /**
             *  Capture the bits from 'bitField' - starting at bit 'offset' (zero-based), and go for
             *  'nBits'. Bits are indexed from the low-order bit (starting with bit 0).
             *
             *  EXAMPLES:
             *      VerifyTestResult (TakeNBitsFrom (0x3, 1, 0) == 1);
             *      VerifyTestResult (TakeNBitsFrom (0x3, 1, 1) == 1);
             *      VerifyTestResult (TakeNBitsFrom (0x3, 1, 2) == 0);
             *      VerifyTestResult (TakeNBitsFrom (0x3, 3, 0) == 0x3);
             &&&
             second byte....
             ***see if this is right - and add to retest
             *      VerifyTestResult (TakeNBitsFrom (0xff, 8, 0) == 0xff);
             *      VerifyTestResult (TakeNBitsFrom (0xff, 8, 8) == 0x0);
             &&&&
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
