/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_ServerContext_h_
#define _Stroika_Foundation_Cryptography_SSL_ServerContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/PKI/Certificate.h"
#include "Stroika/Foundation/Cryptography/PKI/PrivateKey.h"
#include "Stroika/Foundation/Cryptography/SSL/Common.h"

namespace Stroika::Foundation::Cryptography::SSL::ServerContext {

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = default;
    };

    /**
     *  \note Design Note:
     *      Could have had get/set properties on IRep, or passed in Options. The former a bit more flexible
     *      but that flexibility rarely (never) used, so go with simpler API, which may be easier to adapt to
     *      different backend ssl implementations (and simpler threading implications).
     */
    struct Options {
        // @todo  typical args here are -METHOD : SSL-VERSION (TLS1.3, etc) - CERT, and PRIVATE KEY

        /**
         *  To server SSL, need both private key and public CERT
         */
        tuple<PKI::PrivateKey::Ptr, PKI::Certificate::Ptr> fCertificate;
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

    Ptr New (const Options& o);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#endif /*_Stroika_Foundation_Cryptography_SSL_ServerContext_h_*/
