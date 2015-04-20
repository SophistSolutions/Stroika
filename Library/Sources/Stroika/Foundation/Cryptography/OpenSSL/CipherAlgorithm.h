/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Execution/StringException.h"
#include    "../../Memory/Common.h"



/**
 *  \file
 *
 *  TODO:
 *
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace OpenSSL {


#if     qHasFeature_OpenSSL
                /**
                 *      @see http://linux.die.net/man/3/evp_cipher_ctx_init
                 */
                enum    class   CipherAlgorithm {
                    eAES_128_CBC,
                    eAES_128_ECB,
                    eAES_128_OFB,
                    eAES_128_CFB1,
                    eAES_128_CFB8,
                    eAES_128_CFB128,
                    eAES_192_CBC,
                    eAES_192_ECB,
                    eAES_192_OFB,
                    eAES_192_CFB1,
                    eAES_192_CFB8,
                    eAES_192_CFB128,
                    eAES_256_CBC,
                    eAES_256_ECB,
                    eAES_256_OFB,
                    eAES_256_CFB1,
                    eAES_256_CFB8,
                    eAES_256_CFB128,

                    eBlowfish_CBC,
                    eBlowfish_ECB,
                    eBlowfish_CFB,
                    eBlowfish_OFB,

                    eRC2_CBC,
                    eRC2_ECB,
                    eRC2_CFB,
                    eRC2_OFB,

                    eRC4,

                    Stroika_Define_Enum_Bounds(eAES_128_CBC, eRC4)
                };
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "CipherAlgorithm.inl"

#endif  /*_Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_*/
