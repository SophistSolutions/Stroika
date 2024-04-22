/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "../HTTP/Headers.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Streams/TextReader.h"

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
    sb << "{"sv;
    sb << "Method: "sv << fMethod << ","sv;
    sb << "AuthorityRelativeURL: "sv << fAuthorityRelativeURL << ","sv;
    sb << "OverrideHeaders: "sv << fOverrideHeaders << ","sv;
    sb << "Data: "sv << fData << ","sv;
    sb << "}"sv;
    return sb.str ();
}
