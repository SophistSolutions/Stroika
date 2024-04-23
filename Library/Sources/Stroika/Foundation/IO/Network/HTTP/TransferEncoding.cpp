/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if 0
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#endif

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
    return String::Join (Map<Iterable<String>> ([] (auto i) { return Configuration::DefaultNames<TransferEncoding>{}.GetName (i); }));
}

TransferEncodings TransferEncodings::Parse (const String& headerValue)
{
    return TransferEncodings{headerValue.Tokenize ({','}).Map<Iterable<TransferEncoding>> (
        [] (const String& i) { return Configuration::DefaultNames<TransferEncoding>{}.PeekValue (i.As<wstring> ().c_str ()); })};
}
