/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Adapters_inl_
#define _Stroika_Foundation_Cryptography_Hash_Adapters_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Algorithms/Jenkins.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                template    <>
                struct Adapapter<Hasher<uint32_t, Algorithms::Jenkins>> {
                    typedef Hasher<uint32_t, Algorithms::Jenkins>::ReturnType   ReturnType;

                    static  uint32_t  Hash (uint32_t data2Hash);
                };


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Hash_Adapters_inl_*/
