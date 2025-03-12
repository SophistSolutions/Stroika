/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/err.h>
#include <openssl/evp.h>
#endif

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Synchronized.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers::OpenSSL;
using namespace Stroika::Foundation::Memory;

using Execution::Synchronized;

#if qStroika_HasComponent_OpenSSL
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
    : inherited{GetMessage (errorCode)}
    , fErrorCode_{errorCode}
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
    ::ERR_error_string_n (errorCode, buf, NEltsOf (buf));
    return Characters::String::FromNarrowSDKString (buf);
}

[[noreturn]] void Exception::ThrowLastError ()
{
    Execution::Throw (Exception{ERR_get_error ()});
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
