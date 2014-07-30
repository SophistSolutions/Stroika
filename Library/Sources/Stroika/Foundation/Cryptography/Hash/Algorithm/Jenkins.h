/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Jenkins_h_
#define _Stroika_Foundation_Cryptography_Hash_Jenkins_h_  1

#include    "../../../StroikaPreComp.h"

#include    <cstdint>

#include    "../Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://en.wikipedia.org/wiki/Jenkins_hash_function
 *
 *
 * TODO:
 *      @todo   Add other hashes - listed on http://en.wikipedia.org/wiki/Jenkins_hash_function - like - SPOOKY???
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                namespace Algorithm {
                    // Just a name to select template implementation
                    struct Jenkins {
                    };
                }


                template    <>
                struct Hasher<uint32_t, Algorithm::Jenkins> {
                    using   ReturnType      =   uint32_t;

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
#include    "Jenkins.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_Jenkins_h_*/
