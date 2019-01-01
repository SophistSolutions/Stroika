/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_SSLSocket_h_
#define _Stroika_Foundation_Cryptography_SSL_SSLSocket_h_ 1

#include "../../StroikaPreComp.h"

#include <string>
#include <vector>

#include "../../IO/Network/Socket.h"
#include "ClientContext.h"
#include "Common.h"
#include "ServerContext.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 */

namespace Stroika::Foundation::Cryptography::SSL {

// wrapper calss - real work is in protected REP subclasses from Socket Rep...
//
// @todo - probably a mistake - @todo DEPRECATED - probably just do SSLSocketStream - not socket itself. That avoids
// the issue of running afoul of existing socket object hierarchy.
#if qHasFeature_OpenSSL
    class SSLSocket : public IO::Network::Socket {
    public:
    private:
        class Rep_;
    };
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_SSL_SSLSocket_h_*/
