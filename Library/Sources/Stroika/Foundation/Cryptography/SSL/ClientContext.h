/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_ClientContext_h_
#define _Stroika_Foundation_Cryptography_SSL_ClientContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/PKI/Certificate.h"
#include "Stroika/Foundation/Cryptography/PKI/PrivateKey.h"
#include "Stroika/Foundation/Cryptography/SSL/Common.h"

namespace Stroika::Foundation::Cryptography::SSL::ClientContext {

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        using inherited::inherited;
    };

    struct Options {
        /**
         *  Rarely used, but can be used if client-side certs needed
         *      \see https://en.wikipedia.org/wiki/Client_certificate
         */
        optional<tuple<PKI::PrivateKey::Ptr, PKI::Certificate::Ptr>> fClientCertificate;
    };

    Ptr New (const Options& options = {});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_SSL_ClientContext_h_*/
