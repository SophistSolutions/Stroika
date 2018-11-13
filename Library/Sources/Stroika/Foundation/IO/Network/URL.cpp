/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/String2Int.h"
#include "../../Characters/String_Constant.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/StringException.h"

#include "URL.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using Characters::String_Constant;

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
                    Execution::Throw (Execution::StringException (String_Constant (L"bad character in scheme")));
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
            return static_cast<uint8_t> (10 + (digit - 'a'));
        }
        else {
            return 0u;
        }
    }
}

/*
 ********************************************************************************
 ********************* Network::GetDefaultPortForScheme *************************
 ********************************************************************************
 */
optional<uint16_t> Network::GetDefaultPortForScheme (const String& proto)
{
    // From http://www.iana.org/assignments/port-numbers
    static const pair<String_Constant, uint16_t> kPredefined_[] = {
        {String_Constant{L"http"}, static_cast<uint16_t> (80)},
        {String_Constant{L"https"}, static_cast<uint16_t> (443)},
        {String_Constant{L"ldap"}, static_cast<uint16_t> (389)},
        {String_Constant{L"ldaps"}, static_cast<uint16_t> (636)},
        {String_Constant{L"ftp"}, static_cast<uint16_t> (21)},
        {String_Constant{L"ftps"}, static_cast<uint16_t> (990)},
    };
    for (auto i : kPredefined_) {
        if (i.first == proto) {
            return i.second;
        }
    }
    return nullopt;
}

/*
 ********************************************************************************
 ************************************** URL *************************************
 ********************************************************************************
 */
URL::URL ()
    : fScheme_ ()
    , fHost_ ()
    , fPort_ ()
    , fRelPath_ ()
    , fQuery_ ()
    , fFragment_ ()
{
    Ensure (empty ());
}

URL::URL (const String& urlText, ParseOptions po)
{
    *this = Parse (urlText, po);
}

URL URL::Parse (const String& w, ParseOptions po)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper{L"IO::Network::URL::Parse", L"%s,%d", w.c_str (), int(po)};
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
            Execution::Throw (Execution::StringException (L"Cannot parse empty URL"));
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
            // NO - leave blank, so
            // caller parsing can see its missing.
            // result.fScheme_ = String_Constant (L"http");
        }
        else {
            Execution::Throw (Execution::StringException (L"URL missing scheme"));
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
                if (c == ':' or c == '/' or c == '\\') {
                    break;
                }
            }
            size_t endOfHost = i;
            result.fHost_    = w.SubString (hostNameStart, endOfHost);

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
                        result.fPort_ = String2Int<PortType> (num);
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
    , fHost_ (host)
    , fPort_ (portNumber)
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (String_Constant (L"/")));
    Require (not query.StartsWith (String_Constant (L"?")));
    ValidateScheme_ (*fScheme_);
}

URL::URL (const SchemeType& scheme, const String& host, const String& relPath, const String& query, const String& fragment)
    : fScheme_ (NormalizeScheme_ (scheme))
    , fHost_ (host)
    , fPort_ ()
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (String_Constant (L"/")));
    Require (not query.StartsWith (String_Constant (L"?")));
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
    url.fPort_ = nullopt;

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
            size_t endOfHost = i;
            url.fHost_       = w.SubString (hostNameStart, endOfHost);

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
                        url.fPort_ = String2Int<PortType> (num);
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
    SchemeType scheme = GetSchemeValue ();
    // should be large list of items - and maybe do something to assure case matching handled properly, if needed?
    return scheme == String_Constant (L"https") or scheme == String_Constant (L"ftps") or scheme == String_Constant (L"ldaps");
}

URL::SchemeType URL::GetSchemeValue () const
{
    static const String_Constant kHTTPScheme_{L"http"};
    return fScheme_.value_or (kHTTPScheme_);
}

String URL::GetFullURL () const
{
    String result;

    SchemeType scheme = GetSchemeValue ();

    result += scheme + String_Constant (L":");

    if (not fHost_.empty ()) {
        result += String_Constant (L"//") + fHost_;
        if (fPort_.has_value () and fPort_ != GetDefaultPortForScheme (scheme)) {
            result += Format (L":%d", *fPort_);
        }
        result += String_Constant (L"/");
    }

    result += fRelPath_;

    if (not fQuery_.empty ()) {
        result += String_Constant (L"?") + fQuery_;
    }

    if (not fFragment_.empty ()) {
        result += String_Constant (L"#") + fFragment_;
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
    fScheme_ = nullopt;
    fHost_.clear ();
    fRelPath_.clear ();
    fQuery_.clear ();
    fFragment_.clear ();
    fPort_ = nullopt;
    Ensure (empty ());
}

bool URL::empty () const
{
    return not fScheme_.has_value () and fHost_.empty () and fRelPath_.empty () and fQuery_.empty () and fFragment_.empty () and not fPort_.has_value ();
}

bool URL::Equals (const URL& rhs) const
{
    // No need to compare this case-insensatively, because we tolower the schema on construction
    if (fScheme_ != rhs.fScheme_) {
        return false;
    }

    //@todo - consider if we want to do CASE INSENSIVE COMAPRE - I THINK WE MUST!!!
    if (fHost_ != rhs.fHost_) {
        return false;
    }

    if (fPort_ != rhs.fPort_) {
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
        result += String_Constant (L"?") + fQuery_;
    }

    if (not fFragment_.empty ()) {
        result += String_Constant (L"#") + fFragment_;
    }
    return result;
}

String URL::GetHostRelativePathPlusQuery () const
{
    String result = GetHostRelativePath ();
    if (not fQuery_.empty ()) {
        result += String_Constant (L"?") + fQuery_;
    }
    return result;
}

String URL::ToString () const
{
    // @todo not sure if this should include 'default scheme' or not. Probably no, but do for now
    return GetFullURL ();
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
