/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/Execution/Throw.h"

#include "JWT.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Foundation::DataExchange::JSON;

using Memory::BLOB;

/*
 ********************************************************************************
 ********************************** JSON::JWT ***********************************
 ********************************************************************************
 */
JWT::JWT (const String& encodedString, bool fullyValidate)
{
    // super quick and dirty incomplete impl
    //      @todo track / validate signature
    //      @todo validate numeric assertions (like enddate - only validating we can convert type to string)
    Sequence<String> parts = encodedString.Tokenize ({'.'});
    if (parts.size () != 3) {
        Throw (BadFormatException{"JWT must contain exactly two . characters"});
    }
    BLOB                  joseHeader = Cryptography::Encoding::Algorithm::Base64::Decode (parts[0]);
    BLOB                  payload    = Cryptography::Encoding::Algorithm::Base64::Decode (parts[1]);
    [[maybe_unused]] BLOB signature  = Cryptography::Encoding::Algorithm::Base64::Decode (parts[2]);
    // note these reads and transforms CAN fail if there is bad data
    VariantValue headerObj  = DataExchange::Variant::JSON ::Reader{}.Read (joseHeader);
    VariantValue payloadObj = DataExchange::Variant::JSON ::Reader{}.Read (payload);
    fHeaderClaims_          = headerObj.As<Mapping<String, VariantValue>> ().Map<Mapping<String, String>> (
        [] (auto kvp) { return KeyValuePair<String, String>{kvp.fKey, kvp.fValue.template As<String> ()}; });
    fPayloadClaims_ = payloadObj.As<Mapping<String, VariantValue>> ().Map<Mapping<String, String>> (
        [] (auto kvp) { return KeyValuePair<String, String>{kvp.fKey, kvp.fValue.template As<String> ()}; });
    if (fullyValidate) {
        // sadly NYI - appears a bit of work - maybe use https://github.com/Thalhammer/jwt-cpp
    }
}

String JWT::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "headerClaims: " << fHeaderClaims_;
    sb << ", payloadClaims_: " << fPayloadClaims_;
    sb << "}"sv;
    return sb;
}