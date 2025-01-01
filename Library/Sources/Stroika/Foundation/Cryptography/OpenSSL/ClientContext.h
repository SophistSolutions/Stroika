/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_ClientContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_ClientContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/ClientContext.h"

namespace Stroika::Foundation::Cryptography::OpenSSL::ClientContext {

    struct IRep : Cryptography::SSL::ClientContext::IRep {};
    /**
         */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
             *  @todo fix - inherit fewer CTORS - must be from OpenSSL
             */
        using inherited::inherited;


    };

    struct Options : SSL::ClientContext::Options {
        using SSL::ClientContext::Options::Options;
    };


    /**
         */
    Ptr New (const Options& o = {});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_ClientContext_h_*/
