/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/ToString.h"
#include "../../../Streams/TextReader.h"
#include "../HTTP/Headers.h"

#include "Request.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

/*
 ********************************************************************************
 ******************************* Transfer::Request ******************************
 ********************************************************************************
 */
InternetMediaType Request::GetContentType () const
{
    if (optional<String> i = fOverrideHeaders.Lookup (String::FromStringConstant (HTTP::HeaderName::kContentType)); i) {
        return InternetMediaType (*i);
    }
    return InternetMediaType{};
}

void Request::SetContentType (const InternetMediaType& ct)
{
    fOverrideHeaders.Add (String::FromStringConstant (HTTP::HeaderName::kContentType), ct.As<String> ());
}

String Request::ToString () const
{
    StringBuilder sb;
    sb += "{";
    sb += "Method: " + Characters::ToString (fMethod) + ",";
    sb += "fAuthorityRelativeURL: " + Characters::ToString (fAuthorityRelativeURL) + ",";
    sb += "fOverrideHeaders: " + Characters::ToString (fOverrideHeaders) + ",";
    sb += "fData: " + Characters::ToString (fData) + ",";
    sb += "}";
    return sb.str ();
}
