/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "ContentEncoding.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

/*
 ********************************************************************************
 ************************** HTTPs::ContentEncoding ******************************
 ********************************************************************************
 */
template <>
String ContentEncodings::As<String> () const
{
    return String::Join (Map<Iterable<String>> ([] (auto i) { return Characters::ToString (i); }), ", "sv);
}

ContentEncodings ContentEncodings::Parse (const String& headerValue)
{
    // @todo more complex - can have ; and = stuff we drop on floor...
    return ContentEncodings{
        headerValue.Tokenize ({','}).Map<Iterable<ContentEncoding>> ([] (const String& i) { return ContentEncoding{i.Trim ()}; })};
}
