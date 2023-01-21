/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/RegularExpression.h"
#include "../../Characters/String2Int.h"
#include "../../Characters/ToString.h"
#include "../../Containers/Support/ReserveTweaks.h"
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
        static const auto kException_ = Execution::RuntimeErrorException{"illegal hex digit"sv};
        if (isupper (digit)) {
            digit = static_cast<char> (tolower (digit));
        }
        if (isdigit (digit)) {
            return static_cast<uint8_t> (digit - '0');
        }
        else if (islower (digit)) {
            if (digit > 'f') {
                Execution::Throw (kException_);
            }
            return static_cast<uint8_t> (10 + (digit - 'a'));
        }
        else {
            Execution::Throw (kException_);
        }
    }
}

/*
 ********************************************************************************
 *************** UniformResourceIdentification::SchemeType **********************
 ********************************************************************************
 */
SchemeType SchemeType::Normalize () const
{
    // replace all uppercase with lowercase - don't validate here
    return ToLowerCase ();
}

void SchemeType::Validate () const
{
    // https://tools.ietf.org/html/rfc3986#appendix-A  -- scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    for (Characters::Character c : *this) {
        if (not c.IsASCII () or not(c.IsAlphabetic () or c.IsDigit () or c == '-' or c == '.' or c == '+')) [[unlikely]] {
            Execution::Throw (Execution::RuntimeErrorException{"bad character in URI scheme"sv});
        }
    }
}

bool SchemeType::IsSecure () const
{
    SchemeType ns = Normalize ();
    return ns == "https"sv or ns == "ftps"sv or ns == "ldaps"sv or ns == "ssh"sv;
}

optional<PortType> SchemeType::GetDefaultPort () const
{
    // From http://www.iana.org/assignments/port-numbers
    static const Mapping<String, PortType> kPredefined_{String::EqualsComparer{CompareOptions::eCaseInsensitive},
                                                        initializer_list<Common::KeyValuePair<String, PortType>>{
                                                            {"http"sv, static_cast<PortType> (80)},
                                                            {"https"sv, static_cast<PortType> (443)},
                                                            {"ldap"sv, static_cast<PortType> (389)},
                                                            {"ldaps"sv, static_cast<PortType> (636)},
                                                            {"ftp"sv, static_cast<PortType> (21)},
                                                            {"ftps"sv, static_cast<PortType> (990)},
                                                        }};
    return kPredefined_.Lookup (*this);
}

strong_ordering SchemeType::TWC_ (const SchemeType& lhs, const SchemeType& rhs)
{
    using namespace Characters;
    return String::ThreeWayComparer{CompareOptions::eCaseInsensitive}(lhs, rhs);
}

/*
 ********************************************************************************
 ************************************ Host **************************************
 ********************************************************************************
 */
pair<optional<String>, optional<InternetAddress>> Host::ParseRaw_ (const String& raw)
{
    Require (not raw.empty ());

    // See https://tools.ietf.org/html/rfc3986#section-3.2.2 for details of this algorithm
    if (raw[0].IsDigit ()) {
        // must be ipv4 address
        return pair<optional<String>, optional<InternetAddress>>{nullopt, InternetAddress{raw, InternetAddress::AddressFamily::V4}};
    }
    else if (raw[0] == '[') {
        // must be ipv6 address
        // must be surrounded with []
        if (raw.Last () != ']') {
            Execution::Throw (Execution::RuntimeErrorException{"IPV6 hostname in URL must be surrounded with []"sv});
        }
        return pair<optional<String>, optional<InternetAddress>>{nullopt, InternetAddress{raw.SubString (1, -1), InternetAddress::AddressFamily::V6}};
    }
    else {
        return pair<optional<String>, optional<InternetAddress>>{PCTDecode2String (raw.AsUTF8<string> ()), nullopt};
    }
}

Host Host::Normalize () const
{
    if (fRegisteredName_) {
        return Host{fRegisteredName_->ToLowerCase ()};
    }
    Assert (fInternetAddress_);
    return Host{*fInternetAddress_};
}

String Host::EncodeAsRawURL_ (const String& registeredName)
{
    // https://tools.ietf.org/html/rfc3986#appendix-A
    //reg-name      = *( unreserved / pct-encoded / sub-delims )
    static constexpr UniformResourceIdentification::PCTEncodeOptions kHostEncodeOptions_{true};
    return UniformResourceIdentification::PCTEncode2String (registeredName, kHostEncodeOptions_);
}

String Host::EncodeAsRawURL_ (const InternetAddress& ipAddr)
{
    // See https://tools.ietf.org/html/rfc3986#section-3.2.2 for details of this algorithm
    switch (ipAddr.GetAddressFamily ()) {
        case InternetAddress::AddressFamily::V4: {
            return ipAddr.As<String> ();
        } break;
        case InternetAddress::AddressFamily::V6: {
            return "["sv + ipAddr.As<String> () + "]"sv;
        } break;
        default: {
            WeakAssertNotImplemented ();
            // Probably need to use the V??? format - but this maybe the best we can do for now...
            return ipAddr.As<String> ();
        } break;
    }
}

String Host::ToString () const { return Characters::ToString (AsDecoded ()); }

/*
 ********************************************************************************
 ************************************ UserInfo **********************************
 ********************************************************************************
 */
String UserInfo::ParseRaw_ (const String& raw)
{
    Require (not raw.empty ());
    // See https://tools.ietf.org/html/rfc3986#section-3.2.1 for details of this algorithm
    return PCTDecode2String (raw);
}

String UserInfo::EncodeAsRawURL_ (const String& decodedName)
{
    Require (not decodedName.empty ());
    // https://tools.ietf.org/html/rfc3986#appendix-A
    //userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    static constexpr UniformResourceIdentification::PCTEncodeOptions kUserInfoEncodeOptions_{true};
    return UniformResourceIdentification::PCTEncode2String (decodedName, kUserInfoEncodeOptions_);
}

String UserInfo::ToString () const { return Characters::ToString (AsDecoded ()); }

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
    optional<UserInfo> userInfo;
    // From https://tools.ietf.org/html/rfc3986#appendix-A
    //      authority     = [ userinfo "@" ] host [ ":" port ]
    //      host          = IP-literal / IPv4address / reg-name
    //      IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    //      IPv6address   ...
    //      IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    //      IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    //      reg-name      = *( unreserved / pct-encoded / sub-delims )
    String remainingString2Parse = rawURLAuthorityText;
    {
        if (auto oat = remainingString2Parse.Find ('@')) {
            optional<String> encodedUserInfo = remainingString2Parse.SubString (0, *oat);
            if (encodedUserInfo) {
                userInfo = UserInfo::Parse (*encodedUserInfo);
            }
            remainingString2Parse = remainingString2Parse.SubString (*oat + 1);
        }
    }
    optional<Host> host;
    {
        String hostString;
        // here we are looking for characters that are [] (IP-literal) or IPv4address or reg-name characters
        // There are no 'colons' in reg-name, nor IPv4Address. The only possible : in hostname is inside an IP-Literal
        // so check for []
        if (remainingString2Parse.size () >= 2 and remainingString2Parse[0] == '[') {
            auto closeBracket = remainingString2Parse.Find (']'); // a close bracket cannot be in a legal IP-literal except at the end
            if (closeBracket) {
                hostString            = remainingString2Parse.SubString (0, *closeBracket + 1);
                remainingString2Parse = remainingString2Parse.SubString (*closeBracket + 1);
            }
            else {
                Execution::Throw (Execution::RuntimeErrorException{"no closing bracket in host part of authority of URI"sv});
            }
        }
        else {
            // since not IP-literal, any colons would be introducing a port#
            if (auto oPortColon = remainingString2Parse.Find (':')) {
                hostString            = remainingString2Parse.SubString (0, *oPortColon);
                remainingString2Parse = remainingString2Parse.SubString (*oPortColon);
            }
            else {
                hostString            = remainingString2Parse;
                remainingString2Parse = String{};
            }
        }
        host = hostString.empty () ? optional<Host>{} : Host::Parse (hostString);
    }
    optional<uint16_t> port;
    if (auto oPortColon = remainingString2Parse.Find (':')) {
        port = Characters::String2Int<uint16_t> (remainingString2Parse.SubString (*oPortColon + 1));
    }
    return Authority{host, port, userInfo};
}

Authority Authority::Normalize () const { return Authority{fHost_ ? fHost_->Normalize () : optional<Host>{}, fPort_, fUserInfo_}; }

template <>
String Authority::As () const
{
    StringBuilder sb;
    if (fUserInfo_) {
        sb += fUserInfo_->AsEncoded () + "@"sv;
    }
    if (fHost_) {
        sb += fHost_->AsEncoded ();
    }
    if (fPort_) {
        sb += Format (L":%d", *fPort_);
    }
    return sb.str ();
}

String Authority::ToString () const { return Characters::ToString (As<String> ()); }

/*
 ********************************************************************************
 *********************************** Query **************************************
 ********************************************************************************
 */
namespace {
    // According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
    // doing % etc substitution
    // Note - not quite the same as PCTDecode (because of + expansion), and because of looking for = and building a map (and cuz = can be pct encoded)
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
    : fMap_{}
{
    InitURLQueryDecoder_ (&fMap_, query.AsASCII ());
}

Query::Query (const string& query)
    : fMap_{}
{
    InitURLQueryDecoder_ (&fMap_, query);
}

void Query::RemoveFieldIfAny (const String& idx) { fMap_.Remove (idx); }

String Query::ComputeQueryString () const
{
    string result;
    for (auto i = fMap_.begin (); i != fMap_.end (); ++i) {
        Containers::Support::ReserveTweaks::Reserve4Add1 (result);
        if (not result.empty ()) {
            result += "&";
        }
        //careful - need to encode first/second
        result += EncodeURLQueryStringField (i->fKey) + "=" + EncodeURLQueryStringField (i->fValue);
    }
    return ASCIIStringToWide (result);
}

String Query::ToString () const
{
    // could use ComputeQueryString, Characters::ToString (ComputeQueryString), or Characters::ToString (fMap_)
    // Chose this representation because it shows most characters 'decoded' (%xy)
    return Characters::ToString (fMap_);
}

strong_ordering Query::TWC_ (const Query& lhs, const Query& rhs)
{
    // Nothing in https://tools.ietf.org/html/rfc3986#section-3.4 appears to indicate case insensative so treat as case sensitive

    // comparing for equals makes full sense. But comparing < really doesn't, because there is no obvious preferred order for query strings
    // So pick a preferred ordering (alphabetical) - and compare one after the other
    for (String i : (Set<String>{lhs.GetMap ().Keys ()} + Set<String>{rhs.GetMap ().Keys ()}).OrderBy (less<String>{})) {
        optional<String> lhsVal = lhs.GetMap ().Lookup (i);
        optional<String> rhsVal = rhs.GetMap ().Lookup (i);
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        strong_ordering cmp = Common::compare_three_way_BWA{}(lhsVal, rhsVal);
#else
        strong_ordering cmp = lhsVal <=> rhsVal;
#endif
        if (cmp != strong_ordering::equal) {
            return cmp;
        }
    }
    return strong_ordering::equal;
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
    u8string utf8Query = s.AsUTF8 ();
    string   result;
    size_t   sLength = utf8Query.length ();
    result.reserve (sLength);
    for (size_t i = 0; i < sLength; ++i) {
        Containers::Support::ReserveTweaks::Reserve4Add1 (result);
        switch (utf8Query[i]) {
            case ' ':
                result += "+";
                break;
            default: {
                char8_t ccode = utf8Query[i];
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

/*
 ********************************************************************************
 ****************** UniformResourceIdentification::PCTEncode ********************
 ********************************************************************************
 */
string UniformResourceIdentification::PCTEncode (const string& s, const PCTEncodeOptions& options)
{
    string result;
    size_t sLength = s.length ();
    result.reserve (sLength);

    PCTEncodeOptions useOptions = options;
    if (useOptions.allowFragOrQueryChars) {
        useOptions.allowPChar = true;
    }
    if (useOptions.allowPChar) {
        useOptions.allowSubDelims = true;
    }
    if (useOptions.allowPathCharacters) {
        useOptions.allowSubDelims = true;
    }

    for (char c : s) {
        bool encode{true};

        // unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
        if (isalpha (c) or isdigit (c)) {
            encode = false;
        }
        switch (c) {
            case '-':
            case '.':
            case '_':
            case '~':
                encode = false;
        }

        if (useOptions.allowFragOrQueryChars) {
            switch (c) {
                case '/':
                case '?':
                    encode = false;
            }
        }
        if (useOptions.allowPathCharacters) {
            switch (c) {
                case '/':
                    encode = false;
            }
        }
        if (useOptions.allowGenDelims) {
            switch (c) {
                case ':':
                case '/':
                case '?':
                case '[':
                case ']':
                case '@':
                    encode = false;
            }
        }
        if (useOptions.allowSubDelims) {
            switch (c) {
                case '!':
                case '$':
                case '&':
                case '\'':
                case '(':
                case ')':
                case '*':
                case '+':
                case ',':
                case ';':
                case '=':
                    encode = false;
            }
        }
        if (encode) {
            Containers::Support::ReserveTweaks::Reserve4AddN (result, 3);
            result += CString::Format ("%%%.2x", c);
        }
        else {
            Containers::Support::ReserveTweaks::Reserve4Add1 (result);
            result += c;
        }
    }
    return result;
}

string UniformResourceIdentification::PCTEncode (const String& s, const PCTEncodeOptions& options)
{
    return PCTEncode (s.AsUTF8<string> (), options);
}

String UniformResourceIdentification::PCTEncode2String (const String& s, const PCTEncodeOptions& options)
{
    return String::FromUTF8 (PCTEncode (s, options));
}

/*
 ********************************************************************************
 ************** UniformResourceIdentification::PCTDecode ************************
 ********************************************************************************
 */
string UniformResourceIdentification::PCTDecode (const string& s)
{
    string result;
    result.reserve (s.length ());
    for (auto p = s.begin (); p != s.end (); ++p) {
        switch (*p) {
            case '%': {
                if (p + 2 < s.end ()) {
                    unsigned char newC = (ConvertReadSingleHexDigit_ (*(p + 1)) << 4) + ConvertReadSingleHexDigit_ (*(p + 2));
                    p += 2;
                    result += (newC);
                }
                else {
                    Execution::Throw (Execution::RuntimeErrorException{"incomplete % encoded character in URI"sv});
                }
            } break;
            default: {
                result += *p;
            } break;
        }
    }
    return result;
}

/*
 ********************************************************************************
 ************** UniformResourceIdentification::PCTDecode2String *****************
 ********************************************************************************
 */
String UniformResourceIdentification::PCTDecode2String (const string& s) { return String::FromUTF8 (PCTDecode (s)); }

String UniformResourceIdentification::PCTDecode2String (const String& s) { return String::FromUTF8 (PCTDecode (s.AsASCII ())); }

/*
 ********************************************************************************
 *********** hash<Stroika::Foundation::IO::Network::URI> ************************
 ********************************************************************************
 */
size_t std::hash<Stroika::Foundation::IO::Network::UniformResourceIdentification::Host>::operator() (
    const Stroika::Foundation::IO::Network::UniformResourceIdentification::Host& arg) const
{
    return hash<Characters::String> () (arg.AsEncoded<Characters::String> ());
}
