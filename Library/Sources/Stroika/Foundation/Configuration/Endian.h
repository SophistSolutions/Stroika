/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_h_
#define _Stroika_Foundation_Configuration_Endian_h_ 1

namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  @see http://en.wikipedia.org/wiki/Endianness
             */
            enum class Endian {
                eBig,
                eLittle
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
