/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/ServerContext.h"

namespace Stroika::Foundation::Cryptography::OpenSSL::ServerContext {

    using Ptr = Cryptography::SSL::ServerContext::Ptr;

    Ptr New ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_*/
