/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Cookie.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

using Traversal::Iterable;

/*
 ********************************************************************************
 ********************************* HTTP::Cookie *********************************
 ********************************************************************************
 */
Cookie::Cookie (const String& name, const String& value, const Mapping<String, String>& attributes)
    : Cookie{name, value}
{
    for (const auto& i : attributes) {
        AddAttribute (i.fKey, i.fValue);
    }
}

Mapping<String, String> Cookie::GetAttributes () const
{
    Mapping<String, String> result;
    if (fExpires) {
        result.Add (kExpiresAttributeLabel, fExpires->Format (Time::DateTime::kISO8601Format)); // not sure this is right???
    }
    if (fMaxAge) {
        result.Add (kMaxAgeAttributeLabel, Characters::Format ("{}"_f, *fMaxAge));
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
        fExpires = DateTime::Parse (value, DateTime::kRFC1123Format);
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

template <>
String Cookie::As<String> () const
{
    // @todo re-read spec more carefully about character encoding...
    StringBuilder sb;
    sb << fKey << "="sv << fValue;
    for (const KeyValuePair<String, String>& kvp : GetAttributes ()) {
        sb << "; "sv << kvp.fKey;
        static const Set<String> kNoValueAttributes_{kSecureAttributeLabel, kHttpOnlyAttributeLabel};
        if (not kNoValueAttributes_.Contains (kvp.fKey)) {
            sb << "="sv << kvp.fValue;
        }
    }
    return String{};
}

Cookie Cookie::Parse (Streams::InputStream::Ptr<Character> src)
{
    Require (src.IsSeekable ());
    auto skipWS = [&] () {
        while (optional<Character> c = src.Read ()) {
            if (not c->IsWhitespace ()) {
                src.Seek (Streams::Whence::eFromCurrent, -1);
                return;
            }
        }
    };
    // scan up to target char; leave stream after that character, but return string just before it
    auto skipUpTo = [&] (Character targetChar, String* s) {
        StringBuilder sb;
        while (optional<Character> c = src.Read ()) {
            if (*c == targetChar) {
                break;
            }
            else {
                sb.Append (*c);
            }
        }
        *s = sb.str ();
    };
    // same as skipUpTo, but with 2 possible characters
    auto skipUpTo2 = [&] (Character targetChar, Character targetChar2, String* s) {
        StringBuilder sb;
        while (optional<Character> c = src.Read ()) {
            if (*c == targetChar or *c == targetChar2) {
                break;
            }
            else {
                sb.Append (*c);
            }
        }
        *s = sb.str ();
    };
    auto prevChar = [&] () {
        src.Seek (Streams::Whence::eFromCurrent, -1);
        auto c = src.Read ();
        Assert (c.has_value ());
        return *c;
    };
    skipWS ();
    String key;
    skipUpTo ('=', &key);
    String value;
    skipUpTo (';', &value);
    Mapping<String, String> attributes;
    while (not src.IsAtEOF ()) {
        skipWS ();
        String k2;
        String val2;
        skipUpTo2 ('=', ';', &k2);
        if (prevChar () == '=') {
            skipUpTo (';', &val2);
        }
        if (not key.empty ()) {
            attributes.Add (k2, val2);
        }
    }
    return Cookie{key, value, attributes};
}

Cookie Cookie::Parse (const String& src)
{
    return Parse (TextReader::New (src));
}

/*
 ********************************************************************************
 ***************************** HTTP::CookieList *********************************
 ********************************************************************************
 */
CookieList::CookieList ()
    : cookies{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Mapping<String, String> {
                  const CookieList* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &CookieList::cookies);
                  return thisObj->fCookieDetails_.As<Mapping<String, String>> ();
              },
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const Mapping<String, String>& basicCookies) {
                  CookieList* thisObj      = qStroika_Foundation_Common_Property_OuterObjPtr (property, &CookieList::cookies);
                  thisObj->fCookieDetails_ = basicCookies.Map<Iterable<Cookie>> ([] (const auto& i) { return Cookie{i.fKey, i.fValue}; });
              }}
    , cookieDetails{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Collection<Cookie> {
                        const CookieList* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &CookieList::cookieDetails);
                        return thisObj->fCookieDetails_;
                    },
                    [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const Collection<Cookie>& cookies) {
                        CookieList* thisObj      = qStroika_Foundation_Common_Property_OuterObjPtr (property, &CookieList::cookieDetails);
                        thisObj->fCookieDetails_ = cookies;
                    }}
{
}

String CookieList::EncodeForCookieHeader () const
{
    return String::Join (fCookieDetails_.Map<Iterable<String>> ([] (const auto& i) { return i.fKey + "="sv + i.fValue; }), "; "sv);
}

CookieList CookieList::Parse (const String& cookieValueArg)
{
    Collection<Cookie> results;
    for (const auto& keyValuePair : cookieValueArg.Tokenize ({';'})) {
        results += Cookie::Parse (keyValuePair);
    }
    return results;
}
