/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_ClientContext_h_
#define _Stroika_Foundation_Cryptography_SSL_ClientContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/Common.h"

namespace Stroika::Foundation::Cryptography::SSL {

    // Certs, policies, options etc - for a client trying to open an SSL connection
    namespace ClientContext {

        class IRep {
        public:
            virtual ~IRep () = 0;
        };
        using Ptr = shared_ptr<IRep>;

        Ptr New ();
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_SSL_ClientContext_h_*/
