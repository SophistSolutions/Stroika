/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_Common_h_
#define _Stroika_Foundation_Cryptography_SSL_Common_h_ 1

#include "../../StroikaPreComp.h"

#include <string>
#include <vector>

#include "../../Configuration/Common.h"

/*
 * TODO:
 *      o   MAYBE impleemnt SSLStream class- using 'memory-based' BIO. Supposedly that possible. Not totally clear.
 *          (i think even with it - it maybe still best to use SSLSocket - cuz some stuff stuff seems to require using
 *          socket layer directly).
 *
 *      o   http://www.rtfm.com/openssl-examples/
 *
 *
 */

/*
@CONFIGVAR:     qHasFeature_OpenSSL
@DESCRIPTION:   <p>0 or 1</p>
*/
#if !defined(qHasFeature_OpenSSL)
#error "qHasFeature_OpenSSL should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation::Cryptography::SSL {

    // common defines - like if we are using OPENSSL
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_SSL_Common_h_*/
