/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "../OpenSSLCryptoStream.h"

#include "RC4.h"

using std::byte;

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
        return OpenSSLCryptoParams{OpenSSL::CipherAlgorithms::kRC4, key, BLOB{}};
    }
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeRC4 *****************************
 ********************************************************************************
 */
Streams::InputStream::Ptr<byte> Algorithm::DecodeRC4 (const BLOB& key, const Streams::InputStream::Ptr<byte>& in)
{
    return OpenSSLInputStream::New (cvt_ (key), Direction::eDecrypt, in);
}
Memory::BLOB Algorithm::DecodeRC4 (const BLOB& key, const BLOB& in)
{
    return DecodeRC4 (key, in.As<Streams::InputStream::Ptr<byte>> ()).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeRC4 ****************************
 ********************************************************************************
 */
Streams::InputStream::Ptr<byte> Algorithm::EncodeRC4 (const BLOB& key, const Streams::InputStream::Ptr<byte>& in)
{
    return OpenSSLInputStream::New (cvt_ (key), Direction::eEncrypt, in);
}
BLOB Algorithm::EncodeRC4 (const Memory::BLOB& key, const BLOB& in)
{
    return EncodeRC4 (key, in.As<Streams::InputStream::Ptr<byte>> ()).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::RC4Encoder *****************************
 ********************************************************************************
 */
Streams::OutputStream::Ptr<byte> Algorithm::RC4Decoder (const Memory::BLOB& key, const Streams::OutputStream::Ptr<byte>& out)
{
    return OpenSSLOutputStream::New (cvt_ (key), Direction::eDecrypt, out);
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::RC4Encoder ***************************
 ********************************************************************************
 */
Streams::OutputStream::Ptr<byte> Algorithm::RC4Encoder (const Memory::BLOB& key, const Streams::OutputStream::Ptr<byte>& out)
{
    return OpenSSLOutputStream::New (cvt_ (key), Direction::eEncrypt, out);
}
#endif
