/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_MD5_h_
#define _Stroika_Foundation_Cryptography_Hash_MD5_h_  1

#include    "../../../StroikaPreComp.h"

#include    <array>

#include    "../Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                namespace Algorithms {
                    // Just a name to select template implementation
                    struct MD5 {
                    };
                }


                template    <>
                struct Hasher<array<uint8_t, 16>, Algorithms::MD5> {
                    using   ReturnType      =   array<uint8_t, 16>;

                    static  ReturnType  Hash (const Streams::BinaryInputStream& from);
                    static  ReturnType  Hash (const Byte* from, const Byte* to);
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
#include    "MD5.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_MD5_h_*/
