/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_SuperFastHash_h_
#define _Stroika_Foundation_Cryptography_Hash_SuperFastHash_h_  1

#include    "../../../StroikaPreComp.h"

#include    <cstdint>

#include    "../Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://www.azillionmonkeys.com/qed/hash.html
 *
 * TODO:
 *      @todo   See if we need to wrap this whole file in #ifndef for some 'license' issue, since I'm not sure
 *              this associated license is OK...
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                namespace Algorithm {
                    // Just a name to select template implementation
                    struct SuperFastHash {
                    };
                }


                template    <>
                struct Hasher<uint32_t, Algorithm::SuperFastHash> {
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
#include    "SuperFastHash.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_SuperFastHash_h_*/
