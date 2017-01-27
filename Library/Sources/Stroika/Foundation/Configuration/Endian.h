/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_h_
#define _Stroika_Foundation_Configuration_Endian_h_ 1

#include    "../StroikaPreComp.h"

#include    <cstdint>


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


            /**
             *  Utility to convert endianness. Logically this can be defined on any numeric
             *  integer type, but for now is restricted to uint16_t, uint32_t;
             *
             *  @todo - make this constexpr
             */
            template    <typename T>
            T   EndianConverter (T value, Endian from, Endian to);
            template    <>
            uint16_t    EndianConverter (uint16_t value, Endian from, Endian to);
            template    <>
            uint32_t    EndianConverter (uint32_t value, Endian from, Endian to);


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
