/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <atlbase.h>

#include    <Windows.h>
#include    <URLMon.h>
#endif

#include    "../../Characters/CString/Utilities.h"
#include    "../../Characters/Concrete/String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly.h"
#include    "../../Characters/Format.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/StringException.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif

#include    "URL.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;

#if     qPlatform_Windows
using   Stroika::Foundation::Execution::ThrowIfErrorHRESULT;
#endif

using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   Characters::Concrete::String_Constant;




namespace {
    URL::SchemeType NormalizeScheme_ (const URL::SchemeType& s)
    {
        // replace all uppercase with lowercase - dont validate here
        return s.ToLowerCase ();
    }
    void    ValidateScheme_ (const URL::SchemeType& s)
    {
        // use for (Character c : s) {... when that works -- LGP 2013-05-29)
        for (size_t i = 0; i < s.GetLength (); ++i) {
            if (not s[i].IsAscii () or not (s[i].IsAlphabetic () or s[i].IsDigit () or s[i] == '-' or s[i] == '.' or s[i] == '+')) {
                Execution::DoThrow (Execution::StringException (L"bad character in scheme"));
            }
        }
    }
}



namespace   {
    inline  int ConvertReadSingleHexDigit_ (char digit)
    {
        if (isupper (digit)) {
            digit = tolower (digit);
        }
        if (isdigit (digit)) {
            return digit - '0';
        }
        else if (islower (digit)) {
            return 10 + (digit - 'a');
        }
        else {
            return 0;
        }
    }
}


/*
 ********************************************************************************
 ********************* Network::GetDefaultPortForProtocol ***********************
 ********************************************************************************
 */
uint16_t     Network::GetDefaultPortForProtocol (const String& proto)
{
    // From http://www.iana.org/assignments/port-numbers
    if (proto == String ())                     {   return 80; }
    if (proto == String_Constant (L"http"))     {   return 80; }
    if (proto == String_Constant (L"https"))    {   return 443; }
    if (proto == String_Constant (L"ldap"))     {   return 389; }
    if (proto == String_Constant (L"ldaps"))    {   return 636; }
    if (proto == String_Constant (L"ftp"))      {   return 21; }
    if (proto == String_Constant (L"ftps"))     {   return 990; }

    AssertNotReached (); // if this ever happens - we probably have some work todo - the above list is inadequate
    return 80;  // hack...
}





/*
 ********************************************************************************
 ************************************** URL *************************************
 ********************************************************************************
 */
URL::URL ()
    : fProtocol_ ()
    , fHost_ ()
    , fPort_ (kDefaultPortSentinal_)
    , fRelPath_ ()
    , fQuery_ ()
    , fFragment_ ()
{
}

#if     qPlatform_Windows
namespace   {
    DISABLE_COMPILER_MSC_WARNING_START(6262)
    void    OLD_Cracker (const String& w, String* protocol, String* host, String* port, String* relPath, String* query)
    {
        RequireNotNull (protocol);
        RequireNotNull (host);
        RequireNotNull (relPath);
        RequireNotNull (query);

        DWORD   ingored =   0;
        wchar_t outBuf[10 * 1024];

        String canonical;
        ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, NEltsOf (outBuf), &ingored, 0));
        canonical = outBuf;

        {
            size_t  e   =   canonical.find (':');
            if (e != String::kBadIndex) {
                *protocol = canonical.SubString (0, e);
            }
        }

        if (SUCCEEDED (::CoInternetParseUrl (CComBSTR (canonical.c_str ()), PARSE_DOMAIN, 0, outBuf, NEltsOf (outBuf), &ingored, 0))) {
            *host = outBuf;
        }

        // I cannot see how to get other fields using CoInternetParseURL??? - LGP 2004-04-13...
        {
            String  matchStr        =   *protocol + L"://" + *host;
            size_t  startOfPath     =   canonical.Find (matchStr);
            if (startOfPath == String::kBadIndex) {
                matchStr        =   *protocol + L":";
                startOfPath     =   canonical.Find (matchStr);
            }
            if (startOfPath == String::kBadIndex) {
                startOfPath = canonical.length ();
            }
            else {
                startOfPath += matchStr.length ();
            }
            *relPath = canonical.SubString (startOfPath);

            size_t  startOfQuery    =   relPath->find ('?');
            if (startOfQuery != String::kBadIndex) {
                *query = relPath->SubString (startOfQuery + 1);
                relPath->erase (startOfQuery);
            }
        }
    }
    DISABLE_COMPILER_MSC_WARNING_END(6262)
}
#endif
URL::URL (const String& w)
    : fProtocol_ ()
    , fHost_ ()
    , fPort_ (kDefaultPortSentinal_)
    , fRelPath_ ()
    , fQuery_ ()
    , fFragment_ ()
{
    if (w.empty ()) {
        return;
    }

    /*
     *  We MIGHT need to canonicalize the URL:
     *          ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, NEltsOf (outBuf), &ingored, 0));
     *  But empirically, so far, its slow, and doesn't appear to be doing anything. It MIGHT be used in case where we get a URL
     *  with %nn encodings, and they need to be rewritten. But even that doesn't appear critical here. Consider doing that if we
     *  ever get diffs with the OLD_Cracker () reported.
     */

    {
        size_t  e   =   w.find (':');
        if (e != String::kBadIndex) {
            fProtocol_ = NormalizeScheme_ (w.SubString (0, e));
            ValidateScheme_ (fProtocol_);
        }
    }

    size_t i    =   0;
    {
        size_t  len             =   w.length ();
        size_t  hostNameStart   =   0;      // default with hostname at start of URL, unless there is a PROTOCOL: in front
        size_t  e               =   w.find (':');
        if (e != String::kBadIndex) {
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
                wchar_t c   =   w[i].As<wchar_t> ();
                if (c == ':' or c == '/' or c == '\\') {
                    break;
                }
            }
            size_t  endOfHost       =   i;
            fHost_ = w.SubString (hostNameStart, endOfHost);

            // COULD check right here for port# if c == ':' - but dont bother since never did before - and this is apparantly good enuf for now...
            if (i < w.length ()) {
                if (w[i] == ':') {
                    String num;
                    ++i;
                    while (i < w.length () && w[i].IsDigit ()) {
                        num.push_back (w[i].As<wchar_t> ());
                        ++i;
                    }
                    if (!num.empty ()) {
                        fPort_ = String2Int<PortType> (num);
                    }
                }
            }
        }
    }

    {
        fRelPath_ = w.SubString (i);

        // It should be RELATIVE to that hostname and the slash is the separator character
        // NB: This is a change as of 2008-09-04 - so be careful in case anyone elsewhere dependend
        // on the leading slash!
        //      -- LGP 2008-09-04
        if (not fRelPath_.empty () and fRelPath_[0] == '/') {
            fRelPath_ = fRelPath_.SubString (1);
        }

        size_t  startOfFragment =   fRelPath_.find ('#');
        if (startOfFragment != String::kBadIndex) {
            fRelPath_ = fRelPath_.SubString (startOfFragment + 1);
            fRelPath_.erase (startOfFragment);
        }

        size_t  startOfQuery    =   fRelPath_.find ('?');
        if (startOfQuery != String::kBadIndex) {
            fQuery_ = fRelPath_.substr (startOfQuery + 1);
            fRelPath_.erase (startOfQuery);
        }
    }


#if     qDebug && qPlatform_Windows
    {
        String testProtocol;
        String testHost;
        String testPort;
        String testRelPath;
        String testQuery;
        OLD_Cracker (w, &testProtocol, &testHost, &testPort, &testRelPath, &testQuery);
        Assert (testProtocol == fProtocol_);
        if (testProtocol == L"http") {
            Assert (testHost == fHost_.ToLowerCase ());
            {
                //Assert (testPort == fPort);
                if (fPort_ == 80) {
                    Assert (testPort == L"" or testPort == L"80");
                }
                else {
                    // apparently never really implemented in old cracker...
                    //Assert (fPort == ::_wtoi (testPort.c_str ()));
                }
            }
            Assert (testRelPath == fRelPath_ or testRelPath.find (':') != String::kBadIndex or ((String_Constant (L"/") + fRelPath_) == testRelPath));  //old code didnt handle port#   --LGP 2007-09-20
            Assert (testQuery == fQuery_ or not fFragment_.empty ()); // old code didn't check fragment
        }
    }
#endif
}

URL::URL (const SchemeType& protocol, const String& host, Memory::Optional<PortType> portNumber, const String& relPath, const String& query, const String& fragment)
    : fProtocol_ (NormalizeScheme_ (protocol))
    , fHost_ (host)
    , fPort_ (portNumber.Value (kDefaultPortSentinal_))
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (L"/"));
    Require (not query.StartsWith (L"?"));
    ValidateScheme_ (fProtocol_);
}

URL::URL (const SchemeType& protocol, const String& host, const String& relPath, const String& query, const String& fragment)
    : fProtocol_ (NormalizeScheme_ (protocol))
    , fHost_ (host)
    , fPort_ (kDefaultPortSentinal_)
    , fRelPath_ (relPath)
    , fQuery_ (query)
    , fFragment_ (fragment)
{
    Require (not relPath.StartsWith (L"/"));
    Require (not query.StartsWith (L"?"));
    ValidateScheme_ (fProtocol_);
}

URL URL::ParseHostRelativeURL (const String& w)
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

        size_t  startOfFragment =   url.fRelPath_.find ('#');
        if (startOfFragment != String::kBadIndex) {
            url.fFragment_ = url.fRelPath_.SubString (startOfFragment + 1);
            url.fRelPath_.erase (startOfFragment);
        }

        size_t  startOfQuery    =   url.fRelPath_.find ('?');
        if (startOfQuery != String::kBadIndex) {
            url.fQuery_ = url.fRelPath_.SubString (startOfQuery + 1);
            url.fRelPath_.erase (startOfQuery);
        }
    }
    return url;
}

void    URL::SetProtocol (const SchemeType& protocol)
{
    fProtocol_ = NormalizeScheme_ (protocol);
    ValidateScheme_ (fProtocol_);
}

bool    URL::IsSecure () const
{
    // should be large list of items - and maybe do soemthing to assure case matching handled properly, if needed?
    return fProtocol_ == L"https" or fProtocol_ == L"ftps" or fProtocol_ == L"ldaps";
}

String URL::GetFullURL () const
{
    String result;

    if (fProtocol_.empty ()) {
        result += String_Constant (L"http:");
    }
    else {
        result += fProtocol_ + String_Constant (L":");
    }

    if (not fHost_.empty ()) {
        result += String_Constant (L"//") + fHost_;
        if (fPort_ != kDefaultPortSentinal_ and fPort_ != GetDefaultPortForProtocol (fProtocol_)) {
            result += Format (L":%d", fPort_);
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

String  URL::GetHostRelPathDir () const
{
    String  result  =   fRelPath_;
    size_t  i       =   result.rfind ('/');
    if (i == String::kBadIndex) {
        result.clear ();
    }
    else {
        result.erase (i + 1);
    }
    return result;
}

void    URL::clear ()
{
    fProtocol_.clear ();
    fHost_.clear ();
    fRelPath_.clear ();
    fQuery_.clear ();
    fFragment_.clear ();
    fPort_ = kDefaultPortSentinal_;
    Ensure (empty ());
}

bool    URL::empty () const
{
    return fProtocol_.empty () and fHost_.empty () and fRelPath_.empty () and fQuery_.empty () and fFragment_.empty () and fPort_ == kDefaultPortSentinal_;
}

bool    URL::Equals (const URL& rhs) const
{
    // A simpler way to compare - and probably better - is if they both produce the same URL string, they are the
    // same URL (since GetFullURL () normalizes output)
    //  -- LGP 2009-01-17
    return GetFullURL () == rhs.GetFullURL ();
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
string  Network::EncodeURLQueryStringField (const String& s)
{
    //
    // According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
    // doing % etc substitution
    //
    // From http://tools.ietf.org/html/rfc3986#section-2.3
    //      unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
    string  utf8Query   =   s.AsUTF8 ();
    string  result;
    size_t  sLength = utf8Query.length ();
    result.reserve (sLength);
    for (size_t i = 0; i < sLength; ++i) {
        Containers::ReserveSpeedTweekAdd1 (result);
        switch (utf8Query[i]) {
            case ' ':
                result += "+";
                break;
            default:  {
                    wchar_t ccode   =   utf8Query[i];
                    if (isascii (ccode) and (isalnum (ccode) or (ccode == '-') or (ccode == '.') or (ccode == '_') or (ccode == '~'))) {
                        result += static_cast<char> (utf8Query[i]);
                    }
                    else {
                        result +=  CString::Format ("%%%.2x", ccode);
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
namespace   {
    // According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
    // doing % etc substitution
    void    InitURLQueryDecoder_ (Mapping<String, String>* m, const string& utf8Query)
    {
        size_t  utfqLen =   utf8Query.length ();
        for (size_t i = 0; i < utfqLen; ) {
            size_t  e   =   utf8Query.find ('&', i);
            string  elt =   utf8Query.substr (i, e - i);
            size_t  brk =   elt.find('=');
            if (brk != string::npos) {
                string  val =   elt.substr (brk + 1);
                for (auto i = val.begin (); i != val.end (); ++i) {
                    switch (*i) {
                        case    '+':
                            *i = ' ';
                            break;
                        case    '%': {
                                if (i + 2 < val.end ()) {
                                    unsigned char   newC    =   (ConvertReadSingleHexDigit_ (*(i + 1)) << 4) + ConvertReadSingleHexDigit_ (*(i + 2));
                                    i = val.erase (i, i + 2);
                                    *i = static_cast<char> (newC);
                                }
                                break;
                            }
                    }
                }
                m->Add (UTF8StringToWide (elt.substr (0, brk)), UTF8StringToWide (val));
            }
            if (e == String::kBadIndex) {
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

void    URLQuery::RemoveFieldIfAny (const String& idx)
{
    fMap_.Remove (idx);
}

String URLQuery::ComputeQueryString () const
{
    string  result;
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
