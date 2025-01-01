/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/ServerContext.h"

namespace Stroika::Foundation::Cryptography::OpenSSL::ServerContext {

    struct IRep : Cryptography::SSL::ServerContext::IRep {

        virtual SSL_CTX* Get_SSL_CTX () const = 0;
    };
    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
             *   @todo fix - inherit fewer CTORS - must be from OpenSSL IRep
             */
        using inherited::inherited;
    };
    // Need function SSL::Ptr -> OpenSSL::Ptr (or throw, or assert?)

    struct Options : SSL::ServerContext::Options {};

    /**
     */
    Ptr New (const Options& o);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_*/
