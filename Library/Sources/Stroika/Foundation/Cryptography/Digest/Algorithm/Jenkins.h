/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Jenkins_h_
#define _Stroika_Foundation_Cryptography_Digest_Jenkins_h_  1

#include    "../../../StroikaPreComp.h"

#include    <cstdint>

#include    "../Digester.h"



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
            namespace   Digest {


                namespace Algorithm {
                    // Just a name to select template implementation
                    struct Jenkins {
                    };
                }


                template    <>
                struct  Digester<Algorithm::Jenkins, uint32_t> {
                    using   ReturnType      =   uint32_t;

                    static  ReturnType  ComputeDigest (const Streams::BinaryInputStream& from);
                    static  ReturnType  ComputeDigest (const Byte* from, const Byte* to);
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

#endif  /*_Stroika_Foundation_Cryptography_Digest_Jenkins_h_*/
