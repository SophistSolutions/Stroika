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
                    struct CRC32 {
                    };

                }



                /// SEE
                // http://www.riccibitti.com/crcguide.htm
                /// for C code / algoritjhm
#if 0
#endif
                template    <>
                struct Hasher<uint32_t, Algorithms::CRC32> {
                    static  uint32_t    Hash (const Streams::BinaryInputStream& from);
                    static  uint32_t    Hash (const Byte* from, const Byte* to);
#if 0
                    template    <typename   PODTYPE>
                    static  uint32_t    Hash (const PODTYPE& pod);
#endif
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
