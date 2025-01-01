/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_ServerContext_h_
#define _Stroika_Foundation_Cryptography_SSL_ServerContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/Certificate.h"
#include "Stroika/Foundation/Cryptography/SSL/Common.h"

namespace Stroika::Foundation::Cryptography::SSL::ServerContext {

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = 0;
    };
    /**
     *  \note Design Note:
     *      Could have had get/set properites on IRep, or passed in Options. The former a bit more flexible
     *      but that flexability rarely (never) used, so go with simpler API, which may be easier to adapt to
     *      diffderent backend ssl impls (and simpler threading implications).
     */
    struct Options {
        // @todo  typical args here are -METHOD : SSL-VERSION (TLS1.3, etc) - CERT, and PRIVATE KEY
        Certificate::Ptr fCertificate;
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
