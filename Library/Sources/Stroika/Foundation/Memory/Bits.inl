/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Bits_inl_
#define _Stroika_Foundation_Memory_Bits_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <climits>



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             ********************************************************************************
             *************************************** Bit ************************************
             ********************************************************************************
             */
            template    <typename INT_TYPE>
            inline  INT_TYPE    Bit (unsigned int bitNumber)
            {
                Require (bitNumber < CHAR_BIT * sizeof (INT_TYPE));
                return (1 << bitNumber);
            }
            template    <typename INT_TYPE, typename... BIT_ARGS>
            inline  INT_TYPE    Bit (unsigned int bitNumber, const BIT_ARGS& ... args)
            {
                return Bit<INT_TYPE> (bitNumber) | Bit<INT_TYPE> (args...);
            }


            /*
             ********************************************************************************
             ********************************* TakeNBitsFrom ********************************
             ********************************************************************************
             */
            template    <typename INT_TYPE>
            inline  INT_TYPE    TakeNBitsFrom (INT_TYPE bitField, unsigned int nBits, unsigned int offset)
            {
                Require ((nBits + offset) < CHAR_BIT * sizeof (INT_TYPE));
                return ((bitField) >> (offset)) & ((1 << (nBits)) - 1);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Bits_inl_*/
