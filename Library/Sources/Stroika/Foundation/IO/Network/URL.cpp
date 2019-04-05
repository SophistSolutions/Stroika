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
using namespace Stroika::Foundation::IO::Network::UniformResourceIdentification;

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
            result.fScheme_ = NormalizeScheme (w.SubString (0, e));
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
            ValidateScheme (*result.fScheme_);
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
            result.fAuthority_.fHost = Host::Parse (w.SubString (hostNameStart, endOfHost));

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
    : fScheme_ (NormalizeScheme (scheme))
    , fAuthority_{Host{host}, portNumber}
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (L"/"sv));
    Require (not query.StartsWith (L"?"sv));
    ValidateScheme (*fScheme_);
}

URL::URL (const SchemeType& scheme, const String& host, const String& relPath, const String& query, const String& fragment)
    : fScheme_ (NormalizeScheme (scheme))
    , fAuthority_{Host{host}}
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (L"/"sv));
    Require (not query.StartsWith (L"?"sv));
    ValidateScheme (*fScheme_);
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
            url.fScheme_ = NormalizeScheme (w.SubString (0, e));
            ValidateScheme (*url.fScheme_);
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
            url.fAuthority_.fHost = Host::Parse (w.SubString (hostNameStart, endOfHost));

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
    fScheme_ = NormalizeScheme (scheme);
    ValidateScheme (*fScheme_);
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
