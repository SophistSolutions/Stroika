/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_AES_h_
#define _Stroika_Foundation_Cryptography_AES_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Cryptography/OpenSSL/DerivedKey.h"
#include "../../../Memory/BLOB.h"
#include "../../../Streams/InputStream.h"
#include "../../../Streams/OutputStream.h"

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
    Streams::InputStream<std::byte>::Ptr DecodeAES (const OpenSSL::DerivedKey& key, const Streams::InputStream<std::byte>::Ptr& in, AESOptions options = AESOptions::eDEFAULT);
    Memory::BLOB                         DecodeAES (const OpenSSL::DerivedKey& key, const Memory::BLOB& in, AESOptions options = AESOptions::eDEFAULT);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream<std::byte>::Ptr EncodeAES (const OpenSSL::DerivedKey& key, const Streams::InputStream<std::byte>::Ptr& in, AESOptions options = AESOptions::eDEFAULT);
    Memory::BLOB                         EncodeAES (const OpenSSL::DerivedKey& key, const Memory::BLOB& in, AESOptions options = AESOptions::eDEFAULT);
#endif

#if qHasFeature_OpenSSL
    /**
     *  Taken an existing binary output stream, and wrap it with decryption, so that stuff written
     *  to the returned output stream is decrypted before being passed to the argument output stream.
     */
    Streams::OutputStream<std::byte>::Ptr AESDecoder (const OpenSSL::DerivedKey& key, const Streams::OutputStream<std::byte>::Ptr& out, AESOptions options = AESOptions::eDEFAULT);
#endif

#if qHasFeature_OpenSSL
    /**
     *  Taken an existing binary output stream, and wrap it with encpytion, so that stuff written
     *  to the returned output stream is encrpted before being passed to the argument output stream.
     */
    Streams::OutputStream<std::byte>::Ptr AESEncoder (const OpenSSL::DerivedKey& key, const Streams::OutputStream<std::byte>::Ptr& out, AESOptions options = AESOptions::eDEFAULT);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_AES_h_*/
