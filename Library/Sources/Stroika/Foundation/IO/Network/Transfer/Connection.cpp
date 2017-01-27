/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/Format.h"
#include    "../../../Characters/String_Constant.h"
#include    "../../../Execution/Exceptions.h"
#include    "../../../Execution/RequiredComponentMissingException.h"
#include    "../../../Streams/TextReader.h"
#include    "../HTTP/Headers.h"
#include    "../HTTP/Methods.h"

#include    "Connection.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::Transfer;
using   namespace   Stroika::Foundation::Memory;





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

Response    Connection::Send (const Request& r)
{
    Response    response    =    fRep_->Send (r);
    if (not response.GetSucceeded ()) {
        Execution::Throw (Exception (response));
    }
    return response;
}
