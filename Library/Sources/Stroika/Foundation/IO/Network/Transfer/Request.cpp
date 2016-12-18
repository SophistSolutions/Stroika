/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/String_Constant.h"
#include    "../../../Streams/TextReader.h"
#include    "../HTTP/Headers.h"

#include    "Request.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::Transfer;





/*
 ********************************************************************************
 ******************************* Transfer::Request ******************************
 ********************************************************************************
 */
InternetMediaType   Request::GetContentType () const
{
    Optional<String> i   =   fOverrideHeaders.Lookup (String_Constant (HTTP::HeaderName::kContentType));
    if (i) {
        return InternetMediaType (*i);
    }
    return InternetMediaType ();
}

void    Request::SetContentType (const InternetMediaType& ct)
{
    fOverrideHeaders.Add (String_Constant (HTTP::HeaderName::kContentType), ct.As<String> ());
}
