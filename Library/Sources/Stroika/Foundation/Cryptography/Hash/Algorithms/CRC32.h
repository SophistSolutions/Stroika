/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_CRC32_h_
#define _Stroika_Foundation_Cryptography_Hash_CRC32_h_  1

#include    "../../../StroikaPreComp.h"

#include    <cstdint>

#include    "../Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *  @see    http://www.riccibitti.com/crcguide.htm
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {
                namespace   Algorithms {


                    struct CRC32 {
                    };


                }


                template    <>
                struct Hasher<uint32_t, Algorithms::CRC32> {
                    typedef uint32_t   ReturnType;

                    static  ReturnType    Hash (const Streams::BinaryInputStream& from);
                    static  ReturnType    Hash (const Byte* from, const Byte* to);
                };


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CRC32.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_CRC32_h_*/
