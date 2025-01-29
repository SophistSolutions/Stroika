/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Request.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

/*
 ********************************************************************************
 ******************************* Transfer::Request ******************************
 ********************************************************************************
 */
optional<InternetMediaType> Request::GetContentType () const
{
    // static_assert (convertible_to<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::InternetMediaType>);
    return OptionallyCopy<InternetMediaType> (fOverrideHeaders.Lookup (HTTP::HeaderName::kContentType));
}

void Request::SetContentType (const optional<InternetMediaType>& ct)
{
    if (ct) {
        fOverrideHeaders.Add (HTTP::HeaderName::kContentType, ct->As<String> ());
    }
    else {
        fOverrideHeaders.Remove (HTTP::HeaderName::kContentType);
    }
}

TypedBLOB Request::GetTypedBLOB () const
{
    return TypedBLOB{.fData = fData, .fType = GetContentType ()};
}

void Request::SetTypedBLOB (const TypedBLOB& tb)
{
    fData = tb.fData;
    SetContentType (tb.fType);
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
    return sb;
}
