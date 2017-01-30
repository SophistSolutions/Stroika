/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../OpenSSLCryptoStream.h"

#include "RC4.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Encoding;
using namespace Stroika::Foundation::Cryptography::Encoding::Algorithm;

using Memory::BLOB;

#if qHasFeature_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const BLOB& key)
    {
        return OpenSSLCryptoParams (CipherAlgorithm::eRC4, key, BLOB ());
    }
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeRC4 *****************************
 ********************************************************************************
 */
Streams::InputStream<Byte> Algorithm::DecodeRC4 (const BLOB& key, Streams::InputStream<Byte> in)
{
    return OpenSSLInputStream (cvt_ (key), Direction::eDecrypt, in);
}
Memory::BLOB Algorithm::DecodeRC4 (const BLOB& key, const BLOB& in)
{
    return DecodeRC4 (key, in.As<Streams::InputStream<Byte>> ()).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeRC4 ****************************
 ********************************************************************************
 */
Streams::InputStream<Byte> Algorithm::EncodeRC4 (const Memory::BLOB& key, Streams::InputStream<Byte> in)
{
    return OpenSSLInputStream (cvt_ (key), Direction::eEncrypt, in);
}
Memory::BLOB Algorithm::EncodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in)
{
    return EncodeRC4 (key, in.As<Streams::InputStream<Byte>> ()).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::RC4Encoder *****************************
 ********************************************************************************
 */
Streams::OutputStream<Byte> Algorithm::RC4Decoder (const Memory::BLOB& key, Streams::OutputStream<Byte> out)
{
    return OpenSSLOutputStream (cvt_ (key), Direction::eDecrypt, out);
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::RC4Encoder ***************************
 ********************************************************************************
 */
Streams::OutputStream<Byte> Algorithm::RC4Encoder (const Memory::BLOB& key, Streams::OutputStream<Byte> out)
{
    return OpenSSLOutputStream (cvt_ (key), Direction::eEncrypt, out);
}
#endif
