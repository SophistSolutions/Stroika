/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

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
        return OpenSSLCryptoParams (CipherAlgorithm::eRC4, key, BLOB{});
    }
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeRC4 *****************************
 ********************************************************************************
 */
Streams::InputStream<byte>::Ptr Algorithm::DecodeRC4 (const BLOB& key, const Streams::InputStream<byte>::Ptr& in)
{
    return OpenSSLInputStream::New (cvt_ (key), Direction::eDecrypt, in);
}
Memory::BLOB Algorithm::DecodeRC4 (const BLOB& key, const BLOB& in)
{
    return DecodeRC4 (key, in.As<Streams::InputStream<byte>::Ptr> ()).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeRC4 ****************************
 ********************************************************************************
 */
Streams::InputStream<byte>::Ptr Algorithm::EncodeRC4 (const BLOB& key, const Streams::InputStream<byte>::Ptr& in)
{
    return OpenSSLInputStream::New (cvt_ (key), Direction::eEncrypt, in);
}
BLOB Algorithm::EncodeRC4 (const Memory::BLOB& key, const BLOB& in)
{
    return EncodeRC4 (key, in.As<Streams::InputStream<byte>::Ptr> ()).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::RC4Encoder *****************************
 ********************************************************************************
 */
Streams::OutputStream<byte>::Ptr Algorithm::RC4Decoder (const Memory::BLOB& key, const Streams::OutputStream<byte>::Ptr& out)
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
Streams::OutputStream<byte>::Ptr Algorithm::RC4Encoder (const Memory::BLOB& key, const Streams::OutputStream<byte>::Ptr& out)
{
    return OpenSSLOutputStream::New (cvt_ (key), Direction::eEncrypt, out);
}
#endif
