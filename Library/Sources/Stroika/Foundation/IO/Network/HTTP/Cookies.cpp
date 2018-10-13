/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Containers/Set.h"
#include "../../../Streams/TextReader.h"

#include "Cookies.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ************************************ HTTP::Cookie ******************************
 ********************************************************************************
 */
Mapping<String, String> Cookie::GetAttributes () const
{
    Mapping<String, String> result;
    if (fExpires) {
        result.Add (kExpiresAttributeLabel, fExpires->Format (Time::DateTime::PrintFormat::eISO8601)); // not sure this is right???
    }
    if (fMaxAge) {
        result.Add (kMaxAgeAttributeLabel, Characters::Format (L"%d", *fMaxAge));
    }
    if (fDomain) {
        result.Add (kDomainAttributeLabel, *fDomain);
    }
    if (fPath) {
        result.Add (kPathAttributeLabel, *fPath);
    }
    if (fSecure) {
        result.Add (kSecureAttributeLabel, String{});
    }
    if (fHttpOnly) {
        result.Add (kHttpOnlyAttributeLabel, String{});
    }
    if (fOtherAttributes) {
        result += *fOtherAttributes;
    }
    return result;
}

void Cookie::AddAttribute (const String& aEqualsBAttributePair)
{
    if (auto i = aEqualsBAttributePair.Find ('=')) {
        AddAttribute (aEqualsBAttributePair.SubString (0, *i), aEqualsBAttributePair.SubString (0, *i + 1));
    }
    else {
        AddAttribute (aEqualsBAttributePair);
    }
}

void Cookie::AddAttribute (const String& key, const String& value)
{
    using Time::DateTime;
    // ordered so most likely first
    if (key == kPathAttributeLabel) {
        fPath = value;
    }
    else if (key == kDomainAttributeLabel) {
        fDomain = value;
    }
    else if (key == kExpiresAttributeLabel) {
        fExpires = DateTime::Parse (value, DateTime::ParseFormat::eISO8601); // wag - read spec!!!
    }
    else if (key == kMaxAgeAttributeLabel) {
        fMaxAge = String2Int<int> (value);
    }
    else if (key == kSecureAttributeLabel) {
        fSecure = true;
    }
    else if (key == kHttpOnlyAttributeLabel) {
        fHttpOnly = true;
    }
    else {
        if (fOtherAttributes) {
            fOtherAttributes->Add (key, value);
        }
        else {
            fOtherAttributes = Mapping<String, String>{KeyValuePair<String, String>{key, value}};
        }
    }
}

String Cookie::Encode () const
{
    // @todo re-read spec more carefully about character encoding...
    StringBuilder sb;
    sb += fKey;
    sb += L"=";
    sb += fValue;
    for (KeyValuePair<String, String> kvp : GetAttributes ()) {
        sb += L"; ";
        sb += kvp.fKey;
        static const Set<String> kNoValueAttributes_{kSecureAttributeLabel, kHttpOnlyAttributeLabel};
        if (not kNoValueAttributes_.Contains (kvp.fKey)) {
            sb += L"=";
            sb += kvp.fValue;
        }
    }
    return String ();
}

Cookie Cookie::Decode (const Streams::InputStream<Character>::Ptr& src)
{
    AssertNotImplemented ();
    return Cookie ();
}

Cookie Cookie::Decode (const String& src)
{
    return Decode (TextReader::New (src));
}
