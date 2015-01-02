/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_h_
#define _Stroika_Foundation_Configuration_Endian_h_ 1

namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  @see http://en.wikipedia.org/wiki/Endianness
             */
            enum    class Endian {
                eBigByte,       // byte-swapped big-endian
                eBigWord,       // word-swapped big-endian

                eLittleByte,    // byte-swapped little-endian
                eLittleWord,    // word-swapped little-endian

                eBig = eBigByte,
                eLittle = eLittleByte,
                ePDP = eLittleWord,
            };


            /**
             */
            constexpr   Endian  GetEndianness ();



        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Endian.inl"

#endif  /*_Stroika_Foundation_Configuration_Endian_h_*/
