/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/RegularExpression.h"
#include "../../Characters/String2Int.h"
#include "../../Characters/ToString.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"

#include "UniformResourceIdentification.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::UniformResourceIdentification;

namespace {
    inline uint8_t ConvertReadSingleHexDigit_ (char digit)
    {
        if (isupper (digit)) {
            digit = static_cast<char> (tolower (digit));
        }
        if (isdigit (digit)) {
            return static_cast<uint8_t> (digit - '0');
        }
        else if (islower (digit)) {
            if (digit > 'f') {
                Execution::Throw (Execution::RuntimeErrorException (L"illegal hex digit"sv));
            }
            return static_cast<uint8_t> (10 + (digit - 'a'));
        }
        else {
            Execution::Throw (Execution::RuntimeErrorException (L"illegal hex digit"sv));
        }
    }
}

/*
 ********************************************************************************
 *************** UniformResourceIdentification::NormalizeScheme *****************
 ********************************************************************************
 */
SchemeType UniformResourceIdentification::NormalizeScheme (const SchemeType& s)
{
    // replace all uppercase with lowercase - don't validate here
    return s.ToLowerCase ();
}

/*
 ********************************************************************************
 *************** UniformResourceIdentification::ValidateScheme ******************
 ********************************************************************************
 */
void UniformResourceIdentification::ValidateScheme (const SchemeType& s)
{
    // use for (Character c : s) {... when that works -- LGP 2013-05-29)
    for (size_t i = 0; i < s.GetLength (); ++i) {
        if (not s[i].IsASCII () or not(s[i].IsAlphabetic () or s[i].IsDigit () or s[i] == '-' or s[i] == '.' or s[i] == '+'))
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (Execution::RuntimeErrorException (L"bad character in URI scheme"sv));
            }
    }
}

/*
 ********************************************************************************
 *********** UniformResourceIdentification::GetDefaultPortForScheme *************
 ********************************************************************************
 */
optional<PortType> UniformResourceIdentification::GetDefaultPortForScheme (const optional<String>& scheme)
{
    // From http://www.iana.org/assignments/port-numbers
    static const Mapping<String, PortType> kPredefined_{
        String::EqualToCI{},
        initializer_list<Common::KeyValuePair<String, PortType>>{
            {L"http"sv, static_cast<PortType> (80)},
            {L"https"sv, static_cast<PortType> (443)},
            {L"ldap"sv, static_cast<PortType> (389)},
            {L"ldaps"sv, static_cast<PortType> (636)},
            {L"ftp"sv, static_cast<PortType> (21)},
            {L"ftps"sv, static_cast<PortType> (990)},
        }};
    if (scheme) {
        return kPredefined_.Lookup (*scheme);
    }
    return nullopt;
}

/*
 ********************************************************************************
 *************************************** Host ***********************************
 ********************************************************************************
 */
pair<optional<String>, optional<InternetAddress>> Host::ParseRaw_ (const String& raw)
{
    // See https://tools.ietf.org/html/rfc3986#section-3.2.2 for details of this algorithm
    if (raw.empty ()) {
        Execution::Throw (Execution::RuntimeErrorException (L"Empty string cannot be parsed as a URL"sv));
    }
    if (raw[0].IsDigit ()) {
        // must be ipv4 address
        return pair<optional<String>, optional<InternetAddress>>{nullopt, InternetAddress{raw, InternetAddress::AddressFamily::V4}};
    }
    else if (raw[0] == '[') {
        // must be ipv6 address
        // must be surrounded with []
        if (raw.Last () != ']') {
            Execution::Throw (Execution::RuntimeErrorException (L"IPV6 hostanme in URL must be surrounded with []"sv));
        }
        return pair<optional<String>, optional<InternetAddress>>{nullopt, InternetAddress{raw.SubString (1, -1), InternetAddress::AddressFamily::V6}};
    }
    else {
        // must 'registeredname' - typically a DNS name
        // Must decode %NN values, and ??? characterset???
        string utf8ResultBuffer;

        // @todo THROW IF INCOMING STRING NOT VALID ASCII - SO NO REAL UTF8 if I read spec right - use validator code (checked covnvert)

        // pct encoding defined on characters as UTF8 so convert to utf8 string first before decoding
        // @todo very unsure if I interpretted this correctly...

        // See https://tools.ietf.org/html/rfc3986#section-2.1 Percent-Encoding
        string tmp = raw.AsUTF8 ();
        utf8ResultBuffer.reserve (tmp.length ());
        for (auto i = tmp.begin (); i != tmp.end (); ++i) {
            switch (*i) {
                case '%': {
                    if (i + 2 < tmp.end ()) {
                        unsigned char newC = (ConvertReadSingleHexDigit_ (*(i + 1)) << 4) + ConvertReadSingleHexDigit_ (*(i + 2));
                        utf8ResultBuffer.push_back (newC);
                        i += 2;
                    }
                    else {
                        Execution::Throw (Execution::RuntimeErrorException (L"illegal % character parsing URL hostname"sv));
                    }
                } break;
                default: {
                    utf8ResultBuffer.push_back (*i);
                } break;
            }
        }
        return pair<optional<String>, optional<InternetAddress>>{String::FromUTF8 (utf8ResultBuffer), nullopt};
    }
}
String Host::EncodeAsRawURL_ (const String& registeredName)
{
    // See https://tools.ietf.org/html/rfc3986#section-3.2.2 for details of this algorithm
    string utf8Query = registeredName.AsUTF8 ();
    string result;
    size_t sLength = utf8Query.length ();
    result.reserve (sLength);
    for (size_t i = 0; i < sLength; ++i) {
        wchar_t ccode = utf8Query[i];
        if (isascii (ccode) and (isalnum (ccode) or (ccode == '-') or (ccode == '.') or (ccode == '_') or (ccode == '~'))) {
            result += static_cast<char> (utf8Query[i]);
        }
        else {
            result += CString::Format ("%%%.2x", ccode);
        }
    }
    return String::FromUTF8 (result);
}

String Host::EncodeAsRawURL_ (const InternetAddress& ipAddr)
{
    // See https://tools.ietf.org/html/rfc3986#section-3.2.2 for details of this algorithm
    switch (ipAddr.GetAddressFamily ()) {
        case InternetAddress::AddressFamily::V4: {
            return ipAddr.As<String> ();
        } break;
        case InternetAddress::AddressFamily::V6: {
            return L"[" + ipAddr.As<String> () + L"]";
        } break;
        default: {
            WeakAssertNotImplemented ();
            // Probably need to use the V??? format - but this maybe the best we can do for now...
            return ipAddr.As<String> ();
        } break;
    }
}

/*
 ********************************************************************************
 ********************************* Authority ************************************
 ********************************************************************************
 */
optional<Authority> Authority::Parse (const String& rawURLAuthorityText)
{
    if (rawURLAuthorityText.empty ()) {
        return nullopt;
    }
    optional<String> userInfo;
    // From https://tools.ietf.org/html/rfc3986#appendix-A
    String remainingString2Parse = rawURLAuthorityText;
    if (auto oat = remainingString2Parse.Find ('@')) {
        userInfo              = remainingString2Parse.SubString (0, *oat);
        remainingString2Parse = remainingString2Parse.SubString (*oat + 1);
    }
    optional<uint16_t> port;
    if (auto oPortColon = remainingString2Parse.Find (':')) {
        port                  = Characters::String2Int<uint16_t> (remainingString2Parse.SubString (*oPortColon + 1));
        remainingString2Parse = remainingString2Parse.SubString (0, *oPortColon);
    }
    return Authority{Host::Parse (remainingString2Parse), port, userInfo};
}

template <>
String Authority::As () const
{
    StringBuilder sb;
    if (fUserInfo) {
        sb += *fUserInfo + L"@";
    }
    if (fHost) {
        sb += fHost->AsEncoded ();
    }
    if (fPort) {
        sb += Format (L":%d", *fPort);
    }
    return sb.str ();
}

/*
 ********************************************************************************
 ************************************** URLQuery ********************************
 ********************************************************************************
 */
namespace {
    // According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
    // doing % etc substitution
    void InitURLQueryDecoder_ (Mapping<String, String>* m, const string& utf8Query)
    {
        size_t utfqLen = utf8Query.length ();
        for (size_t i = 0; i < utfqLen;) {
            size_t e   = utf8Query.find ('&', i);
            string elt = utf8Query.substr (i, e - i);
            size_t brk = elt.find ('=');
            if (brk != string::npos) {
                string val = elt.substr (brk + 1);
                for (auto p = val.begin (); p != val.end (); ++p) {
                    switch (*p) {
                        case '+':
                            *p = ' ';
                            break;
                        case '%': {
                            if (p + 2 < val.end ()) {
                                unsigned char newC = (ConvertReadSingleHexDigit_ (*(p + 1)) << 4) + ConvertReadSingleHexDigit_ (*(p + 2));
                                p                  = val.erase (p, p + 2);
                                *p                 = static_cast<char> (newC);
                            }
                            break;
                        }
                    }
                }
                m->Add (UTF8StringToWide (elt.substr (0, brk)), UTF8StringToWide (val));
            }
            if (e == String::npos) {
                break;
            }
            i = e + 1;
        }
    }
}
Query::Query (const String& query)
    : fMap_ ()
{
    InitURLQueryDecoder_ (&fMap_, query.AsUTF8 ());
}

Query::Query (const string& query)
    : fMap_ ()
{
    InitURLQueryDecoder_ (&fMap_, query);
}

void Query::RemoveFieldIfAny (const String& idx)
{
    fMap_.Remove (idx);
}

String Query::ComputeQueryString () const
{
    string result;
    for (auto i = fMap_.begin (); i != fMap_.end (); ++i) {
        Containers::ReserveSpeedTweekAdd1 (result);
        if (not result.empty ()) {
            result += "&";
        }
        //careful - need to encode first/second
        result += EncodeURLQueryStringField (i->fKey) + "=" + EncodeURLQueryStringField (i->fValue);
    }
    return ASCIIStringToWide (result);
}

/*
 ********************************************************************************
 ********** UniformResourceIdentification::EncodeURLQueryStringField ************
 ********************************************************************************
 */
string UniformResourceIdentification::EncodeURLQueryStringField (const String& s)
{
    //
    // According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
    // doing % etc substitution
    //
    // From http://tools.ietf.org/html/rfc3986#section-2.3
    //      unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
    string utf8Query = s.AsUTF8 ();
    string result;
    size_t sLength = utf8Query.length ();
    result.reserve (sLength);
    for (size_t i = 0; i < sLength; ++i) {
        Containers::ReserveSpeedTweekAdd1 (result);
        switch (utf8Query[i]) {
            case ' ':
                result += "+";
                break;
            default: {
                wchar_t ccode = utf8Query[i];
                if (isascii (ccode) and (isalnum (ccode) or (ccode == '-') or (ccode == '.') or (ccode == '_') or (ccode == '~'))) {
                    result += static_cast<char> (utf8Query[i]);
                }
                else {
                    result += CString::Format ("%%%.2x", ccode);
                }
            }
        }
    }
    return result;
}
