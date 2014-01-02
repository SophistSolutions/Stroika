/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_inl_
#define _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {


#if     qHas_OpenSSL
            /*
             ********************************************************************************
             ********************* Cryptography::OpenSSLCryptoParams ************************
             ********************************************************************************
             */
            inline  OpenSSLCryptoParams::OpenSSLCryptoParams (const std::function<void(EVP_CIPHER_CTX*)>& f)
                : fInitializer (f)
            {
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_OpenSSLCryptoStream_inl_*/
