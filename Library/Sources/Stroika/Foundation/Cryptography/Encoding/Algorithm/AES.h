/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_AES_h_
#define _Stroika_Foundation_Cryptography_AES_h_  1

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BLOB.h"
#include    "../../../Streams/BinaryInputStream.h"



/**
 *  \file
 *
 * TODO:
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Encoding {
                namespace   Algorithm {


                    /**
                     */
                    enum class AESOptions {
                        e128_CBC,
                        e128_ECB,
                        e128_OFB,
                        e128_CFB1,
                        e128_CFB8,
                        e128_CFB128,
                        e192_CBC,
                        e192_ECB,
                        e192_OFB,
                        e192_CFB1,
                        e192_CFB8,
                        e192_CFB128,
                        e256_CBC,
                        e256_ECB,
                        e256_OFB,
                        e256_CFB1,
                        e256_CFB8,
                        e256_CFB128,

                        Stroika_Define_Enum_Bounds (e128_CBC, e256_CFB128)
                        eDEFAULT = e256_CBC,
                    };


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryInputStream  DecodeAES (const Memory::BLOB& key, Streams::BinaryInputStream in, AESOptions options = AESOptions::eDEFAULT);
                    Memory::BLOB                DecodeAES (const Memory::BLOB& key, const Memory::BLOB& in, AESOptions options = AESOptions::eDEFAULT);
#endif


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryInputStream  EncodeAES (const Memory::BLOB& key, Streams::BinaryInputStream in, AESOptions options = AESOptions::eDEFAULT);
                    Memory::BLOB                EncodeAES (const Memory::BLOB& key, const Memory::BLOB& in, AESOptions options = AESOptions::eDEFAULT);
#endif


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryOutputStream  AESDecoder (const Memory::BLOB& key, Streams::BinaryOutputStream out, AESOptions options = AESOptions::eDEFAULT);
#endif


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryOutputStream  AESEncoder (const Memory::BLOB& key, Streams::BinaryOutputStream out, AESOptions options = AESOptions::eDEFAULT);
#endif


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Cryptography_AES_h_*/
