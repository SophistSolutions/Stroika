/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/ToString.h"
#include "../../../Containers/Set.h"
#include "../../../Streams/TextReader.h"

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
String TransferEncodings::Encode () const
{
    return String::Join (Select<String> ([] (auto i) { return Configuration::DefaultNames<TransferEncoding>{}.GetName (i); }));
}

TransferEncodings TransferEncodings::Decode (const String& headerValue)
{
    return TransferEncodings{headerValue.Tokenize ({','}).Select<TransferEncoding> ([] (const String& i) { return Configuration::DefaultNames<TransferEncoding>{}.PeekValue (i.c_str ()); })};
}
