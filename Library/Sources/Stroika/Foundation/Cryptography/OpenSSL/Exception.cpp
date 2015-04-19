/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
#include    <openssl/evp.h>
#include    <openssl/err.h>
#endif

#include    "../../Containers/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "Exception.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::OpenSSL;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;

using   Memory::BLOB;




#if     qHasFeature_OpenSSL && defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "libeay32.lib")
#pragma comment (lib, "ssleay32.lib")
#endif






#if     qHasFeature_OpenSSL

namespace {
    struct ErrStringIniter_ {
        ErrStringIniter_ ()
        {
            ERR_load_crypto_strings ();
            //SSL_load_error_strings ();
        }
        ~ErrStringIniter_ ()
        {
            ERR_free_strings ();
        }

    } _InitOpenSSLErrStrings_;
}


/*
 ********************************************************************************
 ********************** Cryptography::OpenSSLException **************************
 ********************************************************************************
 */
Exception::Exception (InternalErrorCodeType errorCode)
    : StringException (GetMessage (errorCode))
    , fErrorCode_ (errorCode)
{
}

Exception::InternalErrorCodeType Exception::GetErrorCode () const
{
    return fErrorCode_;
}

Characters::String  Exception::GetMessage (InternalErrorCodeType errorCode)
{
    char    buf[10 * 1024];
    buf[0] = '\0';
    ERR_error_string_n (errorCode, buf, NEltsOf (buf));
    return Characters::String::FromNarrowSDKString (buf);
}

void    Exception::DoThrowLastErrorIfFailed (int status)
{
    if (status != 1) {
        DoThrowLastError ();
    }
}

void    Exception::DoThrowLastError ()
{
    Execution::DoThrow (Exception (ERR_get_error ()));
}
#endif

