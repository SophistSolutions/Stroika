/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_RC4_h_
#define _Stroika_Foundation_Cryptography_RC4_h_  1

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


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryInputStream  DecodeRC4 (const Memory::BLOB& key, Streams::BinaryInputStream in);
#endif


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryInputStream  EncodeRC4 (const Memory::BLOB& key, Streams::BinaryInputStream in);
#endif


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryOutputStream  RC4Decoder (const Memory::BLOB& key, Streams::BinaryOutputStream out);
#endif


#if     qHas_OpenSSL
                    /**
                     */
                    Streams::BinaryOutputStream  RC4Encoder (const Memory::BLOB& key, Streams::BinaryOutputStream out);
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

#endif  /*_Stroika_Foundation_Cryptography_RC4_h_*/
