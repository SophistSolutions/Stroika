/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_RC4_h_
#define _Stroika_Foundation_Cryptography_RC4_h_  1

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BLOB.h"
#include    "../../../Streams/InputStream.h"
#include    "../../../Streams/BinaryOutputStream.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   REDO THIS API - USELESS AS IS. Pass in DervivedKey object - but only after I've made it
 *              more portable.
 *
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Encoding {
                namespace   Algorithm {


#if     qHasFeature_OpenSSL
                    /**
                     */
                    Streams::InputStream<Memory::Byte>  DecodeRC4 (const Memory::BLOB& key, Streams::InputStream<Memory::Byte> in);
                    Memory::BLOB                        DecodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in);
#endif


#if     qHasFeature_OpenSSL
                    /**
                     */
                    Streams::InputStream<Memory::Byte>  EncodeRC4 (const Memory::BLOB& key, Streams::InputStream<Memory::Byte> in);
                    Memory::BLOB                        EncodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in);
#endif


#if     qHasFeature_OpenSSL
                    /**
                     */
                    Streams::BinaryOutputStream<>   RC4Decoder (const Memory::BLOB& key, Streams::BinaryOutputStream<> out);
#endif


#if     qHasFeature_OpenSSL
                    /**
                     */
                    Streams::BinaryOutputStream<>   RC4Encoder (const Memory::BLOB& key, Streams::BinaryOutputStream<> out);
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
