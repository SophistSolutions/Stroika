/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_ 1

#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
extern "C" {
    using  EVP_MD   =   struct env_md_st;
}
#endif

#include    "../../Configuration/Common.h"
#include    "../../Execution/StringException.h"
#include    "../../Memory/Common.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   AddLookupByName functions.
 *
 *
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace OpenSSL {


#if     qHasFeature_OpenSSL
                /**
                 *  ROUGH DRAFT
                 */
                enum    class   DigestAlgorithm  {
                    eMD5,
                    eSHA1
                };
#endif



#if     qHasFeature_OpenSSL
                /**
                 *  \req valid algorithm from above enum, and \ens not nullptr.
                 *
                 *  \note - the returned pointer is immutable, and the data remains valid until the end of the program.
                 */
                const EVP_MD*   Convert2OpenSSL (DigestAlgorithm hashAlg);
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "DigestAlgorithm.inl"

#endif  /*_Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_*/
