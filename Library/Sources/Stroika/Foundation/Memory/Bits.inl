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
            inline  constexpr   INT_TYPE    Bit (unsigned int bitNumber)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy || qCompilerAndStdLib_constexpr_Buggy
                Require (bitNumber < CHAR_BIT * sizeof (INT_TYPE));
#endif
                return (static_cast<INT_TYPE> (1) << bitNumber);
            }
            template    <typename INT_TYPE, typename... BIT_ARGS>
            inline  constexpr   INT_TYPE    Bit (unsigned int bitNumber, const BIT_ARGS& ... args)
            {
                return Bit<INT_TYPE> (bitNumber) | Bit<INT_TYPE> (args...);
            }


            /*
             ********************************************************************************
             ********************************* TakeNBitsFrom ********************************
             ********************************************************************************
             */
            template    <typename INT_TYPE>
            inline  constexpr   INT_TYPE    TakeNBitsFrom (INT_TYPE bitField, unsigned int nBits, unsigned int offset)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy || qCompilerAndStdLib_constexpr_Buggy
                Require (nBits >= 1);
                Require (offset < (CHAR_BIT * sizeof (INT_TYPE)));
                Require ((nBits - 1 + offset) < (CHAR_BIT * sizeof (INT_TYPE)));
#endif
                return (bitField >> offset) & ((1 << nBits) - 1);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Bits_inl_*/
