/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/Format.h"
#include    "../../../Execution/Exceptions.h"
#include    "../HTTP/Headers.h"
#include    "../HTTP/Methods.h"

#include    "Client.h"

#if     qHasFeature_libcurl
#include    "Client_libcurl.h"
#endif
#if     qHasFeature_WinHTTP
#include    "Client_WinHTTP.h"
#endif



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::Transfer;






/*
 ********************************************************************************
 *********************** Transfer::Request::Options *****************************
 ********************************************************************************
 */
Request::Options::Options ()
    : fReturnSSLInfo (false)
{
}






/*
 ********************************************************************************
 ******************************* Transfer::Request *****************************
 ********************************************************************************
 */
Request::Request ()
    : fMethod ()
    , fOverrideHeaders ()
    , fData ()
    , fOptions ()
{
}

InternetMediaType   Request::GetContentType () const
{
    map<String, String>::const_iterator i   =   fOverrideHeaders.find (HTTP::HeaderName::kContentType);
    if (i != fOverrideHeaders.end ()) {
        return InternetMediaType (i->second);
    }
    return InternetMediaType ();
}

void    Request::SetContentType (const InternetMediaType& ct)
{
    fOverrideHeaders[HTTP::HeaderName::kContentType] = ct.As<String> ();
}







/*
 ********************************************************************************
 ****************** Transfer::Response::SSLResultInfo ***************************
 ********************************************************************************
 */

Response::SSLResultInfo::SSLResultInfo ()
    : fSubjectCommonName ()
    , fSubjectCompanyName ()
    , fStyleOfValidation ()
    , fIssuer ()
    , fValidationStatus (eNoSSL)
{
}








/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
Response::Response ()
    : fData ()
    , fHeaders ()
    , fStatus ()
    , fServerEndpointSSLInfo ()

{
}

InternetMediaType   Response::GetContentType () const
{
    map<String, String>::const_iterator i   =   fHeaders.find (HTTP::HeaderName::kContentType);
    if (i != fHeaders.end ()) {
        return InternetMediaType (i->second);
    }
    return InternetMediaType ();
}






/*
 ********************************************************************************
 **************************** Transfer::Connection ******************************
 ********************************************************************************
 */
Response    Connection::Get (const map<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kGet;
    r.fOverrideHeaders = extraHeaders;
    return SendAndRequest (r);
}

Response    Connection::Post (const vector<Byte>& data, const InternetMediaType& contentType, const map<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kPost;
    r.fOverrideHeaders = extraHeaders;
    r.fData = data;
    r.SetContentType (contentType);
    return SendAndRequest (r);
}

Response    Connection::Delete (const map<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kDelete;
    r.fOverrideHeaders = extraHeaders;
    return SendAndRequest (r);
}

Response    Connection::Put (const vector<Byte>& data, const InternetMediaType& contentType, const map<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kPut;
    r.fOverrideHeaders = extraHeaders;
    r.fData = data;
    r.SetContentType (contentType);
    return SendAndRequest (r);
}

Response    Connection::Options (const map<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kOptions;
    r.fOverrideHeaders = extraHeaders;
    return SendAndRequest (r);
}






/*
 ********************************************************************************
 ************************ Transfer::CreateConnection ****************************
 ********************************************************************************
 */
Connection  Transfer::CreateConnection ()
{
#if     qHasFeature_libcurl
    return Connection_LibCurl ();
#endif
#if     qHasFeature_WinHTTP
    return Connection_WinHTTP ();
#endif
}
