/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_RC4_h_
#define _Stroika_Foundation_Cryptography_RC4_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   REDO THIS API - USELESS AS IS. Pass in DerivedKey object - but only after I've made it
 *              more portable.
 *
 *
 *
 */

namespace Stroika::Foundation::Cryptography::Encoding::Algorithm {

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream::Ptr<byte> DecodeRC4 (const Memory::BLOB& key, const Streams::InputStream::Ptr<byte>& in);
    Memory::BLOB                    DecodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream::Ptr<byte> EncodeRC4 (const Memory::BLOB& key, const Streams::InputStream::Ptr<byte>& in);
    Memory::BLOB                    EncodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::OutputStream::Ptr<byte> RC4Decoder (const Memory::BLOB& key, const Streams::OutputStream::Ptr<byte>& out);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::OutputStream::Ptr<byte> RC4Encoder (const Memory::BLOB& key, const Streams::OutputStream::Ptr<byte>& out);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_RC4_h_*/
