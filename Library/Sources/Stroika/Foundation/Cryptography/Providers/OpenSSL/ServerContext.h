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

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::ServerContext {

    /**
     */
    struct IRep : Cryptography::SSL::ServerContext::IRep {
        virtual SSL_CTX* Get_SSL_CTX () const = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        Ptr (SSL::ServerContext::Ptr p)
        {
            if (auto pp = dynamic_pointer_cast<IRep> (p)) {
                *this = Ptr{pp};
            }
            else {
                // Need function SSL::Ptr -> OpenSSL::Ptr (or throw, or assert?)
                throw ("oops");
            }
        }
    };

    struct Options : SSL::ServerContext::Options {

        const SSL_METHOD* fMethod{nullptr}; // e.g. TLS_1.3_method ()
    };

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
