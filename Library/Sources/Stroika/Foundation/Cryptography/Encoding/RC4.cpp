/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "OpenSSLCryptoStream.h"

#include    "RC4.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;



#if     qHas_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const Memory::BLOB& key, OpenSSLCryptoParams::Direction direction)
    {
        return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eRC4, key, direction);
    }
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 ************************** Cryptography::DecodeRC4 *****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Cryptography::DecodeRC4 (const Memory::BLOB& key, Streams::BinaryInputStream in)
{
    return OpenSSLInputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eDecrypt), in);
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 *************************** Cryptography::EncodeRC4 ****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Cryptography::EncodeRC4 (const Memory::BLOB& key, Streams::BinaryInputStream in)
{
    return OpenSSLInputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eEncrypt), in);
}
#endif







#if     qHas_OpenSSL
/*
 ********************************************************************************
 ************************* Cryptography::RC4Encoder *****************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Cryptography::RC4Decoder (const Memory::BLOB& key, Streams::BinaryOutputStream out)
{
    return OpenSSLOutputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eDecrypt), out);
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 *************************** Cryptography::RC4Encoder ***************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Cryptography::RC4Encoder (const Memory::BLOB& key, Streams::BinaryOutputStream out)
{
    return OpenSSLOutputStream (cvt_ (key, OpenSSLCryptoParams::Direction::eEncrypt), out);
}
#endif










