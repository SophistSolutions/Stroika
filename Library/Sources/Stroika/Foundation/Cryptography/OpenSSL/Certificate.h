/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/Certificate.h"

namespace Stroika::Foundation::Cryptography::OpenSSL::Certificate {

    struct IRep : Cryptography::Certificate::IRep {};
    /**
         */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
             *  @todo fix - inherit fewer CTORS - must be from OpenSSL
             */
        using inherited::inherited;
    };

    // @todo VERY rough - needs optional private key, and TYPE info
    Ptr New (Memory::BLOB pubKey, Memory::BLOB privateKey);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_*/
