/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_CRC32_h_
#define _Stroika_Foundation_Cryptography_Hash_CRC32_h_  1

#include    "../../../StroikaPreComp.h"

#include    <cstdint>

#include    "../Common.h"



/*
 * STATUS *****************DRAFT ..... ROUHGH OUTLINE
 *
 *      VERY ROUGH
 *
 *      SEE
 *
 *      Several OTHERS there todo likewise - SPOOKY???
 *
 *      (SEE README ABOUT ADAPTERS)
 *
 *
 *
 * TODO:
 *      o
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {
                namespace   Algorithms {


                    // Just a name to select template implementation
                    /// SEE
                    // http://www.riccibitti.com/crcguide.htm
                    /// for C code / algoritjhm
                    struct CRC32 {
                    };


                }



                /// SEE
                // http://www.riccibitti.com/crcguide.htm
                /// for C code / algoritjhm
                template    <>
                struct Hasher<uint32_t, Algorithms::CRC32> {
                    static  uint32_t    Hash (const Streams::BinaryInputStream& from);
                    static  uint32_t    Hash (const Byte* from, const Byte* to);
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
