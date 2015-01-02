/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/Format.h"
#include    "../../../Characters/String_Constant.h"
#include    "../../../Execution/Exceptions.h"
#include    "../../../Execution/RequiredComponentMissingException.h"
#include    "../../../Streams/TextInputStreamBinaryAdapter.h"
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
using   namespace   Stroika::Foundation::Memory;






/*
 ********************************************************************************
 ******************************* Transfer::Request ******************************
 ********************************************************************************
 */
InternetMediaType   Request::GetContentType () const
{
    Optional<String> i   =   fOverrideHeaders.Lookup (String_Constant (HTTP::HeaderName::kContentType));
    if (i.get () != nullptr) {
        return InternetMediaType (*i);
    }
    return InternetMediaType ();
}

void    Request::SetContentType (const InternetMediaType& ct)
{
    fOverrideHeaders.Add (String_Constant (HTTP::HeaderName::kContentType), ct.As<String> ());
}







/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
InternetMediaType   Response::GetContentType () const
{
    Optional<String> i   =   fHeaders_.Lookup (String_Constant (HTTP::HeaderName::kContentType));
    if (i.get () != nullptr) {
        return InternetMediaType (*i);
    }
    return InternetMediaType ();
}

void    Response::ThrowIfFailed () const
{
    HTTP::Exception::DoThrowIfError (fStatus_);
}

BinaryInputStream       Response::GetDataBinaryInputStream () const
{
    if (fDataBinaryInputStream_.IsMissing ()) {
        fDataBinaryInputStream_ = fData_.As<BinaryInputStream> ();
    }
    return *fDataBinaryInputStream_;
}

TextInputStream       Response::GetDataTextInputStream () const
{
    if (fDataTextInputStream_.IsMissing ()) {
        fDataTextInputStream_ = Streams::TextInputStreamBinaryAdapter (GetDataBinaryInputStream ());
    }
    return *fDataTextInputStream_;
}




/*
 ********************************************************************************
 **************************** Transfer::Connection ******************************
 ********************************************************************************
 */
Response    Connection::GET (const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kGet;
    r.fOverrideHeaders = extraHeaders;
    return Send (r);
}

Response    Connection::POST (const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kPost;
    r.fOverrideHeaders = extraHeaders;
    r.fData = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response    Connection::DELETE (const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kDelete;
    r.fOverrideHeaders = extraHeaders;
    return Send (r);
}

Response    Connection::PUT (const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kPut;
    r.fOverrideHeaders = extraHeaders;
    r.fData = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response    Connection::OPTIONS (const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod = HTTP::Methods::kOptions;
    r.fOverrideHeaders = extraHeaders;
    return Send (r);
}






/*
 ********************************************************************************
 ************************ Transfer::CreateConnection ****************************
 ********************************************************************************
 */
Connection  Transfer::CreateConnection (const Connection::Options& options)
{
#if     qHasFeature_libcurl
    return Connection_LibCurl (options);
#endif
#if     qHasFeature_WinHTTP
    return Connection_WinHTTP (options);
#endif
    Execution::DoThrow (Execution::RequiredComponentMissingException (Execution::RequiredComponentMissingException::kIONetworkClientFactory));
}
