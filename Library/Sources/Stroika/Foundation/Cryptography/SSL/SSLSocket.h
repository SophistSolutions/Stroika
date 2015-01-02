/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_SSLSocket_h_
#define _Stroika_Foundation_Cryptography_SSL_SSLSocket_h_   1

#include    "../../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../IO/Network/Socket.h"
#include    "Common.h"
#include    "ClientContext.h"
#include    "ServerContext.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   SSL {


                // wrapper calss - real work is in protected REP subclasses from Socket Rep...
#if     qHas_OpenSSL
                class   SSLSocket : public IO::Network::Socket {
                public:
                    SSLSocket ();

                private:
                    class   Rep_;
                };
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

#endif  /*_Stroika_Foundation_Cryptography_SSL_SSLSocket_h_*/
