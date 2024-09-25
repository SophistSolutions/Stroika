/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_AES_h_
#define _Stroika_Foundation_Cryptography_AES_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/DerivedKey.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   REDO THIS API - USELESS AS IS. Pass in DervivedKey object - but only after I've made it
 *              more portable.
 *
 */

namespace Stroika::Foundation::Cryptography::Encoding::Algorithm {

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

        eDEFAULT = e256_CBC,

        Stroika_Define_Enum_Bounds (e128_CBC, e256_CFB128)
    };

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream::Ptr<byte> DecodeAES (const OpenSSL::DerivedKey& key, const Streams::InputStream::Ptr<byte>& in,
                                               AESOptions options = AESOptions::eDEFAULT);
    Memory::BLOB DecodeAES (const OpenSSL::DerivedKey& key, const Memory::BLOB& in, AESOptions options = AESOptions::eDEFAULT);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream::Ptr<byte> EncodeAES (const OpenSSL::DerivedKey& key, const Streams::InputStream::Ptr<byte>& in,
                                               AESOptions options = AESOptions::eDEFAULT);
    Memory::BLOB EncodeAES (const OpenSSL::DerivedKey& key, const Memory::BLOB& in, AESOptions options = AESOptions::eDEFAULT);
#endif

#if qHasFeature_OpenSSL
    /**
     *  Taken an existing binary output stream, and wrap it with decryption, so that stuff written
     *  to the returned output stream is decrypted before being passed to the argument output stream.
     */
    Streams::OutputStream::Ptr<byte> AESDecoder (const OpenSSL::DerivedKey& key, const Streams::OutputStream::Ptr<byte>& out,
                                                 AESOptions options = AESOptions::eDEFAULT);
#endif

#if qHasFeature_OpenSSL
    /**
     *  Taken an existing binary output stream, and wrap it with encpytion, so that stuff written
     *  to the returned output stream is encrpted before being passed to the argument output stream.
     */
    Streams::OutputStream::Ptr<byte> AESEncoder (const OpenSSL::DerivedKey& key, const Streams::OutputStream::Ptr<byte>& out,
                                                 AESOptions options = AESOptions::eDEFAULT);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_AES_h_*/
