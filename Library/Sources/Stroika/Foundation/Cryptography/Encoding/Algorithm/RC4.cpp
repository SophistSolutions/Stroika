/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../OpenSSLCryptoStream.h"

#include    "RC4.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Encoding;
using   namespace   Stroika::Foundation::Cryptography::Encoding::Algorithm;



#if     qHasFeature_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const Memory::BLOB& key, OpenSSLCryptoParams::Direction direction)
    {
        return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eRC4, key, direction);
    }
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeRC4 *****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Algorithm::DecodeRC4 (const Memory::BLOB& key, Streams::BinaryInputStream in)
{
    return OpenSSLInputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eDecrypt), in);
}
Memory::BLOB  Algorithm::DecodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in)
{
    return DecodeRC4 (key, in.As<Streams::BinaryInputStream> ()).ReadAll ();
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeRC4 ****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Algorithm::EncodeRC4 (const Memory::BLOB& key, Streams::BinaryInputStream in)
{
    return OpenSSLInputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eEncrypt), in);
}
Memory::BLOB  Algorithm::EncodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in)
{
    return EncodeRC4 (key, in.As<Streams::BinaryInputStream> ()).ReadAll ();
}
#endif







#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::RC4Encoder *****************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Algorithm::RC4Decoder (const Memory::BLOB& key, Streams::BinaryOutputStream out)
{
    return OpenSSLOutputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eDecrypt), out);
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::RC4Encoder ***************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Algorithm::RC4Encoder (const Memory::BLOB& key, Streams::BinaryOutputStream out)
{
    return OpenSSLOutputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eEncrypt), out);
}
#endif

