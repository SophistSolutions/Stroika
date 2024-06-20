/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "TransferEncoding.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

/*
 ********************************************************************************
 ************************* HTTPs::TransferEncoding ******************************
 ********************************************************************************
 */
template <>
String TransferEncodings::As<String> () const
{
    return String::Join (Map<Iterable<String>> ([] (auto i) { return Characters::ToString (i); }));
}

TransferEncodings TransferEncodings::Parse (const String& headerValue)
{
    return TransferEncodings{
        headerValue.Tokenize ({','}).Map<Iterable<TransferEncoding>> ([] (const String& i) { return TransferEncoding{i.Trim ()}; })};
}
