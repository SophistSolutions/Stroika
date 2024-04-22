/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
namespace Stroika::Foundation::Cryptography::Encoding {

#if qHas_OpenSSL
    /*
     ********************************************************************************
     ********************* Cryptography::OpenSSLCryptoParams ************************
     ********************************************************************************
     */
    inline OpenSSLCryptoParams::OpenSSLCryptoParams (const function<void (EVP_CIPHER_CTX*, Direction)>& f)
        : fInitializer{f}
    {
    }
#endif

}
