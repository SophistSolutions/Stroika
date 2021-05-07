/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_inl_
#define _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

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

    /**
     *  \req valid algorithm from above enum, and \ens not nullptr.
     *
     *  \note - the returned pointer is immutable, and the data remains valid until the end of the program.
     */
    [[deprecated ("Since Stroika 2.1b12")]] inline const EVP_CIPHER* Convert2OpenSSL (CipherAlgorithm alg)
    {
        return alg;
    }
#endif

}

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_inl_*/
