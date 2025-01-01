/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_ClientContext_h_
#define _Stroika_Foundation_Cryptography_SSL_ClientContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/Common.h"
#include "Stroika/Foundation/Cryptography/Certificate.h"

namespace Stroika::Foundation::Cryptography::SSL::ClientContext {

    // Certs, policies, options etc - for a client trying to open an SSL connection

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
        /**
             *  inherit all CTORS from base
             */
        using inherited::inherited;
    };

    
        // todo add method for 'UseClientCert', or better to have OPTIONS object passed to NEW() probably sufficnet API and
    // more flexibly applied to other backend apis maybe(non openssl crypto and threading issues)
    struct Options {
        optional<Certificate::Ptr> fClientCertificate;
    };

    Ptr New (const Options& options = {});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_SSL_ClientContext_h_*/
