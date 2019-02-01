/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/err.h>
#include <openssl/evp.h>
#endif

#include "../../Containers/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Common.h"
#include "../../Execution/Synchronized.h"
#include "../../Memory/SmallStackBuffer.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Memory;

#if qHasFeature_OpenSSL && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#else
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#endif
#endif

using Execution::Synchronized;

#if qHasFeature_OpenSSL
namespace {
    Synchronized<bool> sNamesSupported_{true}; // @todo note locking not done right yet here - not safely...
    Synchronized<bool> sNamesLoaded_{false};

    struct ErrStringIniter_ {
        ~ErrStringIniter_ ()
        {
            if (sNamesSupported_) {
                auto l = sNamesLoaded_.rwget ();
                if (static_cast<bool> (l)) {
                    ERR_free_strings ();
                    l = false;
                }
            }
        }
    } _InitOpenSSLErrStrings_;
    void LoadStringsIfNeeded_ ()
    {
        if (not sNamesLoaded_ and sNamesSupported_) {
            auto l = sNamesLoaded_.rwget ();
            if (not static_cast<bool> (l)) {
                ERR_load_crypto_strings ();
                //SSL_load_error_strings ();
                l = true;
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Cryptography::Exception **************************
 ********************************************************************************
 */
Exception::Exception (InternalErrorCodeType errorCode)
    : inherited (GetMessage (errorCode))
    , fErrorCode_ (errorCode)
{
}

Exception::InternalErrorCodeType Exception::GetErrorCode () const
{
    return fErrorCode_;
}

Characters::String Exception::GetMessage (InternalErrorCodeType errorCode)
{
    LoadStringsIfNeeded_ ();
    char buf[10 * 1024];
    buf[0] = '\0';
    ERR_error_string_n (errorCode, buf, NEltsOf (buf));
    return Characters::String::FromNarrowSDKString (buf);
}

void Exception::ThrowLastErrorIfFailed (int status)
{
    if (status != 1) {
        ThrowLastError ();
    }
}

void Exception::ThrowLastError ()
{
    Execution::Throw (Exception (ERR_get_error ()));
}

bool Exception::GetNamesSupported ()
{
    return sNamesSupported_.load ();
}

void Exception::SetNamesSupported (bool openSSLStringsSupported)
{
    auto l = sNamesSupported_.rwget ();
    if (static_cast<bool> (l) != openSSLStringsSupported) {
        if (openSSLStringsSupported) {
            // nothing todo - just
        }
        else {
            auto lNamesLoaded = sNamesLoaded_.rwget ();
            if (static_cast<bool> (l)) {
                ERR_free_strings ();
                lNamesLoaded = false;
            }
        }
        sNamesSupported_ = openSSLStringsSupported;
    }
}
#endif
