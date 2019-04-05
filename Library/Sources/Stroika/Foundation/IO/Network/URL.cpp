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

#include "URL.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

namespace {
    URL::SchemeType NormalizeScheme_ (const URL::SchemeType& s)
    {
        // replace all uppercase with lowercase - don't validate here
        return s.ToLowerCase ();
    }
    void ValidateScheme_ (const URL::SchemeType& s)
    {
        // use for (Character c : s) {... when that works -- LGP 2013-05-29)
        for (size_t i = 0; i < s.GetLength (); ++i) {
            if (not s[i].IsASCII () or not(s[i].IsAlphabetic () or s[i].IsDigit () or s[i] == '-' or s[i] == '.' or s[i] == '+'))
                [[UNLIKELY_ATTR]]
                {
                    Execution::Throw (Execution::RuntimeErrorException (L"bad character in scheme"sv));
                }
        }
    }
}

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
 ********************* Network::GetDefaultPortForScheme *************************
 ********************************************************************************
 */
optional<uint16_t> Network::GetDefaultPortForScheme (const optional<String>& scheme)
{
    // From http://www.iana.org/assignments/port-numbers
    static const pair<String, uint16_t> kPredefined_[] = {
        {L"http"sv, static_cast<uint16_t> (80)},
        {L"https"sv, static_cast<uint16_t> (443)},
        {L"ldap"sv, static_cast<uint16_t> (389)},
        {L"ldaps"sv, static_cast<uint16_t> (636)},
        {L"ftp"sv, static_cast<uint16_t> (21)},
        {L"ftps"sv, static_cast<uint16_t> (990)},
    };
    if (scheme) {
        for (auto i : kPredefined_) {
            if (i.first == *scheme) {
                return i.second;
            }
        }
    }
    return nullopt;
}

/*
 ********************************************************************************
 *************************** URL::Authority::Host *******************************
 ********************************************************************************
 */
pair<optional<String>, optional<InternetAddress>> URL::Authority::Host::ParseRaw_ (const String& raw)
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
String URL::Authority::Host::EncodeAsRawURL_ (const String& registeredName)
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

String URL::Authority::Host::EncodeAsRawURL_ (const InternetAddress& ipAddr)
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
 **************************** URL::Authority ************************************
 ********************************************************************************
 */
optional<URL::Authority> URL::Authority::Parse (const String& rawURLAuthorityText)
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
    return URL::Authority{URL::Authority::Host::Parse (remainingString2Parse), port, userInfo};
}

template <>
String URL::Authority::As () const
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
 ************************************** URL *************************************
 ********************************************************************************
 */
URL::URL (const String& urlText, ParseOptions po)
{
    *this = Parse (urlText, po);
}

URL URL::Parse (const String& w, ParseOptions po)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper{L"IO::Network::URL::Parse", L"%s,%d", w.c_str (), int (po)};
#endif
    if (po == URL::eAsRelativeURL) {
        return ParseHostRelativeURL_ (w);
    }
    if (po == URL::eStroikaPre20a50BackCompatMode) {
        return ParseHosteStroikaPre20a50BackCompatMode_ (w);
    }

    URL result;

    // technically, according to http://www.ietf.org/rfc/rfc1738.txt, the BNF for url
    // is
    // genericurl     = scheme ":" schemepart
    // url            = httpurl | ftpurl | newsurl |
    //...
    // httpurl        = "http://" hostport [ "/" hpath [ "?" search ]]
    //
    // however, its common practice to be more fliexible in interpretting urls.
    // @todo EXPOSE THIS AS PARAMETER!!!
    bool flexibleURLParsingMode = (po != URL::eAsFullURL);

    if (w.empty ())
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Execution::RuntimeErrorException (L"Cannot parse empty URL"sv));
            //return result;
        }

    /*
     *  We MIGHT need to canonicalize the URL:
     *          ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, NEltsOf (outBuf), &ignored, 0));
     *  But empirically, so far, its slow, and doesn't appear to be doing anything. It MIGHT be used in case where we get a URL
     *  with %nn encodings, and they need to be rewritten. But even that doesn't appear critical here. Consider doing that if we
     *  ever get diffs with the OLD_Cracker () reported.
     */

    size_t hostNameStart = 0; // default with hostname at start of URL, unless there is a PROTOCOL: in front
    {
        size_t slashshash = w.Find (L"//").value_or (String::npos); // if we have //fooo:304 as ou rurl, treat as hostname fooo, and port 304, and scheme http:
        size_t e          = w.find (':');
        if (e != String::npos and (slashshash == String::npos or e < slashshash)) {
            result.fScheme_ = NormalizeScheme_ (w.SubString (0, e));
            hostNameStart   = e + 1;
        }
        else if (flexibleURLParsingMode) {
            /* 
             *  Until Stroika 2.1d23 this just left blank (so caller parsing can see its missing)
             *  But since Stroika v2.1d23, this flexible parsing mode constructs a full url, by filling in details at parse
             *  time
             */
            result.fScheme_ = L"http"sv;
        }
        else {
            Execution::Throw (Execution::RuntimeErrorException (L"URL missing scheme"sv));
        }
        if (result.fScheme_) {
            ValidateScheme_ (*result.fScheme_);
        }
    }

    size_t i = 0;
    {
        i = hostNameStart;

        /*
         *  According to they BFN in http://www.ietf.org/rfc/rfc1738.txt, the hostname is required, and must contain
         *  at least one character. We interpret that spec loosely, but it appears also the practice of things like chrome in interpretting user typed urls.
         *
         *  The only exception is if the first character is /, but not //, then it really cannot be a hostname, and must be a host-relative pathname.
         *
         *  NOTE - before 2014-11-09, I just interpretted this as the host-relative url part, so this is an incompatible, but probably helpful
         *  change:
         *
         *      OLD COMMENT:
         *      // Look for // and assume whats after is a hostname, and otherwise, the rest is a relative url
         *      // (REALLY UNSURE ABOUT THIS LOGIC - HAVENT FOUND GOOD DOCS ON THE NET FOR THIS FORMAT
         *      //      -- LGP 2006-01-24
         */
        if (w.SubString (hostNameStart).StartsWith (L"//") or not w.SubString (hostNameStart).StartsWith (L"/")) {
            if (w.SubString (hostNameStart).StartsWith (L"//")) {
                // skip '//' before hostname
                hostNameStart++;
                hostNameStart++;
            }

            // then hostname extends to EOS, or first colon (for port#) or first '/'
            i = hostNameStart;
            for (; i != w.length (); ++i) {
                wchar_t c = w[i].As<wchar_t> ();
                if (c == ':' or c == '/' or c == '\\' or c == '?') {
                    break;
                }
            }
            size_t endOfHost         = i;
            result.fAuthority_.fHost = Authority::Host::Parse (w.SubString (hostNameStart, endOfHost));

            // COULD check right here for port# if c == ':' - but don't bother since never did before - and this is apparantly good enuf for now...
            if (i < w.length ()) {
                if (w[i] == ':') {
                    String num;
                    ++i;
                    while (i < w.length () and w[i].IsDigit ()) {
                        num.push_back (w[i].As<wchar_t> ());
                        ++i;
                    }
                    if (!num.empty ()) {
                        result.fAuthority_.fPort = String2Int<PortType> (num);
                    }
                }
            }
        }
    }

    {
        result.fRelPath_ = w.SubString (i);

        // It should be RELATIVE to that hostname and the slash is the separator character
        // NB: This is a change as of 2008-09-04 - so be careful in case anyone elsewhere dependend
        // on the leading slash!
        //      -- LGP 2008-09-04
        if (not result.fRelPath_.empty () and result.fRelPath_[0] == '/') {
            result.fRelPath_ = result.fRelPath_.SubString (1);
        }

        size_t startOfFragment = result.fRelPath_.find ('#');
        if (startOfFragment != String::npos) {
            result.fRelPath_ = result.fRelPath_.SubString (startOfFragment + 1);
            result.fRelPath_.erase (startOfFragment);
        }

        size_t startOfQuery = result.fRelPath_.find ('?');
        if (startOfQuery != String::npos) {
            result.fQuery_ = result.fRelPath_.substr (startOfQuery + 1);
            result.fRelPath_.erase (startOfQuery);
        }
    }
    return result;
}

URL::URL (const SchemeType& scheme, const String& host, const optional<PortType>& portNumber, const String& relPath, const String& query, const String& fragment)
    : fScheme_ (NormalizeScheme_ (scheme))
    , fAuthority_{Authority::Host{host}, portNumber}
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (L"/"sv));
    Require (not query.StartsWith (L"?"sv));
    ValidateScheme_ (*fScheme_);
}

URL::URL (const SchemeType& scheme, const String& host, const String& relPath, const String& query, const String& fragment)
    : fScheme_ (NormalizeScheme_ (scheme))
    , fAuthority_{Authority::Host{host}}
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (L"/"sv));
    Require (not query.StartsWith (L"?"sv));
    ValidateScheme_ (*fScheme_);
}

URL URL::ParseHostRelativeURL_ (const String& w)
{
    URL url;
    {
        url.fRelPath_ = w;

        // It should be RELATIVE to that hostname and the slash is the separator character
        // NB: This is a change as of 2008-09-04 - so be careful in case anyone elsewhere dependend
        // on the leading slash!
        //      -- LGP 2008-09-04
        if (not url.fRelPath_.empty () and url.fRelPath_[0] == '/') {
            url.fRelPath_ = url.fRelPath_.SubString (1);
        }

        size_t startOfFragment = url.fRelPath_.find ('#');
        if (startOfFragment != String::npos) {
            url.fFragment_ = url.fRelPath_.SubString (startOfFragment + 1);
            url.fRelPath_.erase (startOfFragment);
        }

        size_t startOfQuery = url.fRelPath_.find ('?');
        if (startOfQuery != String::npos) {
            url.fQuery_ = url.fRelPath_.SubString (startOfQuery + 1);
            url.fRelPath_.erase (startOfQuery);
        }
    }
    return url;
}

URL URL::ParseHosteStroikaPre20a50BackCompatMode_ (const String& w)
{
    URL url;
    url.fAuthority_.fPort = nullopt;

    if (w.empty ()) {
        return url;
    }

    /*
     *  We MIGHT need to canonicalize the URL:
     *          ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, NEltsOf (outBuf), &ignored, 0));
     *  But empirically, so far, its slow, and doesn't appear to be doing anything. It MIGHT be used in case where we get a URL
     *  with %nn encodings, and they need to be rewritten. But even that doesn't appear critical here. Consider doing that if we
     *  ever get diffs with the OLD_Cracker () reported.
     */

    {
        size_t e = w.find (':');
        if (e != String::npos) {
            url.fScheme_ = NormalizeScheme_ (w.SubString (0, e));
            ValidateScheme_ (*url.fScheme_);
        }
    }

    size_t i = 0;
    {
        size_t len           = w.length ();
        size_t hostNameStart = 0; // default with hostname at start of URL, unless there is a PROTOCOL: in front
        size_t e             = w.find (':');
        if (e != String::npos) {
            hostNameStart = e + 1;
        }
        i = hostNameStart;

        // Look for // and assume whats after is a hostname, and otherwise, the rest is a relative url
        // (REALLY UNSURE ABOUT THIS LOGIC - HAVENT FOUND GOOD DOCS ON THE NET FOR THIS FORMAT
        //      -- LGP 2006-01-24

        if (hostNameStart + 2 < len and w[hostNameStart] == '/' and w[hostNameStart + 1] == '/') {
            // skip '//' before hostname
            hostNameStart++;
            hostNameStart++;

            // then hostname extends to EOS, or first colon (for port#) or first '/'
            i = hostNameStart;
            for (; i != w.length (); ++i) {
                wchar_t c = w[i].As<wchar_t> ();
                if (c == ':' or c == '/' or c == '\\') {
                    break;
                }
            }
            size_t endOfHost      = i;
            url.fAuthority_.fHost = Authority::Host::Parse (w.SubString (hostNameStart, endOfHost));

            // COULD check right here for port# if c == ':' - but don't bother since never did before - and this is apparantly good enuf for now...
            if (i < w.length ()) {
                if (w[i] == ':') {
                    String num;
                    ++i;
                    while (i < w.length () and w[i].IsDigit ()) {
                        num.push_back (w[i].As<wchar_t> ());
                        ++i;
                    }
                    if (!num.empty ()) {
                        url.fAuthority_.fPort = String2Int<PortType> (num);
                    }
                }
            }
        }
    }

    {
        url.fRelPath_ = w.SubString (i);

        // It should be RELATIVE to that hostname and the slash is the separator character
        // NB: This is a change as of 2008-09-04 - so be careful in case anyone elsewhere dependend
        // on the leading slash!
        //      -- LGP 2008-09-04
        if (not url.fRelPath_.empty () and url.fRelPath_[0] == '/') {
            url.fRelPath_ = url.fRelPath_.SubString (1);
        }

        size_t startOfFragment = url.fRelPath_.find ('#');
        if (startOfFragment != String::npos) {
            url.fRelPath_ = url.fRelPath_.SubString (startOfFragment + 1);
            url.fRelPath_.erase (startOfFragment);
        }

        size_t startOfQuery = url.fRelPath_.find ('?');
        if (startOfQuery != String::npos) {
            url.fQuery_ = url.fRelPath_.substr (startOfQuery + 1);
            url.fRelPath_.erase (startOfQuery);
        }
    }
    return url;
}

void URL::SetScheme (const SchemeType& scheme)
{
    fScheme_ = NormalizeScheme_ (scheme);
    ValidateScheme_ (*fScheme_);
}

bool URL::IsSecure () const
{
    optional<SchemeType> scheme = GetScheme ();
    // should be large list of items - and maybe do something to assure case matching handled properly, if needed?
    return scheme == L"https"sv or scheme == L"ftps"sv or scheme == L"ldaps"sv;
}

optional<URL::Authority> URL::GetAuthority () const
{
    return fAuthority_;
}

URL::SchemeType URL::GetSchemeValue () const
{
    static const String kHTTPScheme_{L"http"sv};
    return fScheme_.value_or (kHTTPScheme_);
}

String URL::GetFullURL () const
{
    String result;

    optional<SchemeType> scheme = GetScheme ();
    // @todo - major rework of this for partial URLs
    if (not scheme.has_value ()) {
        scheme = L"http"; // backward compatible but bad design...
    }

    result += *scheme + L":"sv;

    if (fAuthority_.fHost) {
        result += L"//"sv + fAuthority_.fHost->AsEncoded ();
        if (fAuthority_.fPort.has_value () and fAuthority_.fPort != GetDefaultPortForScheme (scheme)) {
            result += Format (L":%d", *fAuthority_.fPort);
        }
        result += L"/"sv;
    }

    result += fRelPath_;

    if (not fQuery_.empty ()) {
        result += L"?"sv + fQuery_;
    }

    if (not fFragment_.empty ()) {
        result += L"#"sv + fFragment_;
    }

    return result;
}

String URL::GetHostRelPathDir () const
{
    String result = fRelPath_;
    size_t i      = result.rfind ('/');
    if (i == String::npos) {
        result.clear ();
    }
    else {
        result.erase (i + 1);
    }
    return result;
}

void URL::clear ()
{
    fScheme_              = nullopt;
    fAuthority_.fHost     = nullopt;
    fAuthority_.fPort     = nullopt;
    fAuthority_.fUserInfo = nullopt;
    fRelPath_.clear ();
    fQuery_.clear ();
    fFragment_.clear ();
    fAuthority_.fPort = nullopt;
    Ensure (empty ());
}

bool URL::empty () const
{
    // @todo consider fAuthority.fUserInfo - and rethink this definition
    return not fScheme_.has_value () and not fAuthority_.fHost and fRelPath_.empty () and fQuery_.empty () and fFragment_.empty () and not fAuthority_.fPort.has_value ();
}

bool URL::Equals (const URL& rhs) const
{
    // No need to compare this case-insensatively, because we tolower the schema on construction
    if (fScheme_ != rhs.fScheme_) {
        return false;
    }

    if (fAuthority_.fHost != rhs.fAuthority_.fHost) {
        return false;
    }

    if (fAuthority_.fPort != rhs.fAuthority_.fPort) {
        return false;
    }
    if (fRelPath_ != rhs.fRelPath_) {
        return false;
    }
    if (fQuery_ != rhs.fQuery_) {
        return false;
    }
    if (fFragment_ != rhs.fFragment_) {
        return false;
    }
    return true;
}

int URL::Compare (const URL& rhs) const
{
    String               lhsF   = GetFullURL ();
    String               rhsF   = rhs.GetFullURL ();
    [[maybe_unused]] int result = lhsF.Compare (rhsF);
    Assert (Equals (rhs) == (result == 0)); // ensure Compare matches Equals
    return lhsF.Compare (rhsF);
}

String URL::GetHostRelURLString () const
{
    String result = GetHostRelativePath ();
    if (not fQuery_.empty ()) {
        result += L"?"sv + fQuery_;
    }

    if (not fFragment_.empty ()) {
        result += L"#"sv + fFragment_;
    }
    return result;
}

String URL::GetHostRelativePathPlusQuery () const
{
    String result = GetHostRelativePath ();
    if (not fQuery_.empty ()) {
        result += L"?"sv + fQuery_;
    }
    return result;
}

String URL::ToString () const
{
    // @todo not sure if this should include 'default scheme' or not. Probably no, but do for now
    return Characters::ToString (GetFullURL ()); // format this string as any other normal string
}

#if 0
HRESULT hr  =   ::CoInternetParseUrl (url.c_str (), ParseAction, dwParseFlags, pwzResult, cchResult, pcchResult, dwReserved);

protocol. This section defines the scheme - specific syntax and
semantics for http URLs.

http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]

               If the port is empty or not given, port 80 is assumed. The semantics
               are that the identified resource is located at the server listening
               for TCP connections on that port of that host, and the Request - URI
               for the resource is abs_path (section 5.1.2). The use of IP addresses
                       in URLs SHOULD be avoided whenever possible (see RFC 1900 [24]). If
                       the abs_path is not present in the URL, it MUST be given as "/" when
                       used as a Request - URI for a resource (section 5.1.2). If a proxy
                           receives a host name which is not a fully qualified domain name, it
                           MAY add its domain to the host name it received. If a proxy receives
                           a fully qualified domain name, the proxy MUST NOT change the host
                           name.








                           Fielding, et al.            Standards Track                    [Page 19]
                           
                           RFC 2616                        HTTP / 1.1                       June 1999


                           3.2.3 URI Comparison

                           When comparing two URIs to decide if they match or not, a client
                       SHOULD use a case-sensitive octet-by-octet comparison of the entire
                               URIs, with these exceptions:

- A port that is empty or not given is equivalent to the defaultport for that URI - reference;

- Comparisons of host names MUST be case-insensitive;

- Comparisons of scheme names MUST be case-insensitive;

- An empty abs_path is equivalent to an abs_path of "/".

Characters other than those in the "reserved" and "unsafe" sets (see
        RFC 2396 [42]) are equivalent to their "" % " HEX HEX" encoding.

For example, the following three URIs are equivalent:

http://abc.com:80/~smith/home.html
http://ABC.com/%7Esmith/home.html
http://ABC.com:/%7esmith/home.html
#endif

/*
 ********************************************************************************
 ************************************** URI *************************************
 ********************************************************************************
 */
URI URI::Parse (const String& rawURL)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper{L"IO::Network::URI::Parse", L"%s", rawURL.c_str ()};
#endif
    // https://tools.ietf.org/html/rfc3986#appendix-B
    static const RegularExpression kParseURLRegExp_{L"^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"_RegEx};
    optional<String>               scheme;
    optional<String>               authority;
    optional<String>               path;
    optional<String>               query;
    optional<String>               fragment;
    auto                           emptyStr2Missing = [] (const optional<String>& s) -> optional<String> {
        if (s) {
            if (not s->empty ()) {
                return s;
            }
        }
        return nullopt;
    };
    if (rawURL.Match (kParseURLRegExp_, nullptr, &scheme, nullptr, &authority, &path, nullptr, &query, nullptr, &fragment)) {
        return URI{emptyStr2Missing (scheme), Authority::Parse (authority.value_or (String{})), path.value_or (String{}), emptyStr2Missing (query), emptyStr2Missing (fragment)};
    }
    else {
        Execution::Throw (Execution::RuntimeErrorException (L"Ill-formed URI"sv)); // doesn't match regexp in https://tools.ietf.org/html/rfc3986#appendix-B
    }
}

template <>
String URI::As () const
{
    // @todo - support %PCT ENCODING
    StringBuilder        result;
    optional<SchemeType> scheme = this->fScheme_;
    if (fScheme_) {
        result += *fScheme_ + L":";
    }
    if (fAuthority_) {
        result += L"//" + fAuthority_->As<String> ();
    }
    // @todo IF we have a HOST, then path must have leading '/' !!!! - NOT SURE HOW TO HANDLE
    result += fPath_;
    if (fQuery_) {
        result += L"?"sv + *fQuery_;
    }
    if (fFragment_) {
        result += L"#"sv + *fFragment_;
    }
    return result.str ();
}

template <>
URL URI::As () const
{
    Require (IsURL ());
    return URL{}; // NYI
}

String URI::ToString () const
{
    return Characters::ToString (As<String> ());
}

/*
 ********************************************************************************
 *********************** EncodeURLQueryStringField ******************************
 ********************************************************************************
 */
string Network::EncodeURLQueryStringField (const String& s)
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
URLQuery::URLQuery (const String& query)
    : fMap_ ()
{
    InitURLQueryDecoder_ (&fMap_, query.AsUTF8 ());
}

URLQuery::URLQuery (const string& query)
    : fMap_ ()
{
    InitURLQueryDecoder_ (&fMap_, query);
}

void URLQuery::RemoveFieldIfAny (const String& idx)
{
    fMap_.Remove (idx);
}

String URLQuery::ComputeQueryString () const
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
