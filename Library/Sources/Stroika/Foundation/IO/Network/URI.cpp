/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/RegularExpression.h"
#include "../../Characters/String2Int.h"
#include "../../Characters/ToString.h"
#include "../../DataExchange/CheckedConverter.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"

#include "URL.h"

#include "URI.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

/*
 ********************************************************************************
 ************************************** URI *************************************
 ********************************************************************************
 */
URI::URI (const URL& url)
    : fScheme_{url.GetScheme ()}
    , fAuthority_{url.GetAuthority ()}
    , fPath_{url.GetHostRelativePath ()}
    , fQuery_{url.GetQueryString ()}
    , fFragment_{url.GetFragment ()}
{
}

URI URI::Parse (const string& rawURL)
{
    return Parse (String::FromASCII (rawURL));
}

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
    (void)rawURL.AsASCII (); // for throw check side-effect
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
    StringBuilder result;
    if (fScheme_) {
        // From https://tools.ietf.org/html/rfc3986#appendix-A
        //      scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        // no need to pct encode this
        Assert (fScheme_->All ([] (Character c) { return c.IsASCII (); }));
        result += *fScheme_ + L":";
    }
    if (fAuthority_) {
        Assert (fAuthority_->As<String> ().All ([] (Character c) { return c.IsASCII (); }));
        result += L"//" + fAuthority_->As<String> (); // this already produces 'raw' format
    }

    // @todo IF we have a HOST, then path must have leading '/' !!!! - NOT SURE HOW TO HANDLE

    static constexpr UniformResourceIdentification::PCTEncodeOptions kPathEncodeOptions_{false, false, false, false, true};
    result += UniformResourceIdentification::PCTEncode2String (fPath_, kPathEncodeOptions_);
    if (fQuery_) {
        static constexpr UniformResourceIdentification::PCTEncodeOptions kQueryEncodeOptions_{false, false, false, true};
        result += L"?"sv + UniformResourceIdentification::PCTEncode2String (*fQuery_, kQueryEncodeOptions_);
    }
    if (fFragment_) {
        static constexpr UniformResourceIdentification::PCTEncodeOptions kFragmentEncodeOptiosn_{false, false, false, true};
        result += L"#"sv + UniformResourceIdentification::PCTEncode2String (*fFragment_, kFragmentEncodeOptiosn_);
    }
    Ensure (result.str ().All ([] (Character c) { return c.IsASCII (); }));
    return result.str ();
}

template <>
string URI::As () const
{
    // @todo - Could be more efficient duing String algorithm directly
    return As<String> ().AsASCII ();
}

template <>
URL URI::As () const
{
    Require (IsURL ());
    return URL{}; // NYI
}

URI URI::Normalize () const
{
    optional<SchemeType> scheme = fScheme_;
    if (scheme) {
        scheme = scheme->Normalize ();
    }
    optional<Authority> authority = fAuthority_;
    if (authority) {
        authority = authority->Normalize ();
    }
    String path = fPath_; // @todo https://tools.ietf.org/html/rfc3986#section-6.2.2.3

    return URI{scheme, authority, path, fQuery_, fFragment_};
}

String URI::ToString () const
{
    return Characters::ToString (As<String> ());
}

URI URI::Combine (const URI& uri) const
{
    // From https://tools.ietf.org/html/rfc3986#section-5
    //      "Note that only the scheme component is required to be present in a base URI; the other components may be empty or undefined."
    URI baseURI = Normalize ();
    if (not baseURI.GetScheme ()) {
        Execution::Throw (Execution::RuntimeErrorException (L"Scheme is required in base URI to combine with another URI"sv));
    }
    auto merge = [&] (const String& base, const String& rhs) {
        // @see https://tools.ietf.org/html/rfc3986#section-5.2.3
        if (baseURI.GetAuthority () and base.empty ()) {
            return L"/" + rhs;
        }
        static const RegularExpression kSelectDir_ = L"(.*\\/)[^\\/]*"_RegEx;
        optional<String>               baseDir;
        (void)base.Match (kSelectDir_, &baseDir);
        return baseDir.value_or (String{}) + rhs;
    };
    auto remove_dot_segments = [] (const String& p) {
        // from https://tools.ietf.org/html/rfc3986#section-5.2.4
        vector<String> segments; // for our purpose here, segments may (or not in case of first) contain a leading /
        StringBuilder  accumulatingSegment;
        for (Character c : p) {
            if (c == '/' and not accumulatingSegment.empty ()) {
                segments.push_back (accumulatingSegment.str ());
                accumulatingSegment.clear ();
            }
            accumulatingSegment += c;
        }
        if (not accumulatingSegment.empty ()) {
            segments.push_back (accumulatingSegment.str ());
        }
        vector<String> segments2;                         // apply ../. removeal
        bool           lastSegmentShouldHaveSlash{false}; // not sure about this
        for (String segment : segments) {
            lastSegmentShouldHaveSlash = false;
            if (segment == L"." or segment == L"/.") {
                // drop it on the floor
                if (segment[0] == '/') {
                    lastSegmentShouldHaveSlash = true;
                }
            }
            else if (segment == L".." or segment == L"/..") {
                if (not segments2.empty ()) {
                    segments2.pop_back ();
                }
                if (segment[0] == '/') {
                    lastSegmentShouldHaveSlash = true;
                }
            }
            else {
                segments2.push_back (segment);
            }
        }

        StringBuilder result;
        for (String segment : segments2) {
            result += segment;
        }
        if (lastSegmentShouldHaveSlash and not result.str ().EndsWith (L"/")) {
            result += L"/";
        }
        return result.str ();
    };

    Assert (remove_dot_segments (L"/a/b/c/./../../g") == L"/a/g");    // from https://tools.ietf.org/html/rfc3986#section-5.2.4
    Assert (remove_dot_segments (L"mid/content=5/../6") == L"mid/6"); // ditto

    // Algorithm copied from https://tools.ietf.org/html/rfc3986#section-5.2.2
    URI result;

    /*
     * Skipped this part
     *    -- A non-strict parser may ignore a scheme in the reference
     *    -- if it is identical to the base URI's scheme.
     *    --
     *    if ((not strict) and (R.scheme == Base.scheme)) then
     *       undefine(R.scheme);
     *    endif;
     */
    if (uri.GetScheme ()) {
        result.SetScheme (uri.GetScheme ());
        result.SetAuthority (uri.GetAuthority ());
        result.SetPath (remove_dot_segments (uri.GetPath ()));
        result.SetQuery (uri.GetQuery<String> ());
    }
    else {
        result.SetScheme (baseURI.GetScheme ());
        if (uri.GetAuthority ()) {
            result.SetAuthority (uri.GetAuthority ());
            result.SetPath (remove_dot_segments (uri.GetPath ()));
            result.SetQuery (uri.GetQuery<String> ());
        }
        else {
            result.SetAuthority (baseURI.GetAuthority ());
            if (uri.GetPath ().empty ()) {
                result.SetPath (baseURI.GetPath ());
                result.SetQuery (uri.GetQuery<String> () ? uri.GetQuery<String> () : baseURI.GetQuery<String> ());
            }
            else {
                if (uri.GetPath ().StartsWith (L"/")) {
                    result.SetPath (remove_dot_segments (uri.GetPath ()));
                }
                else {
                    result.SetPath (remove_dot_segments (merge (baseURI.GetPath (), uri.GetPath ())));
                }
                result.SetQuery (uri.GetQuery<String> ());
            }
        }
    }
    result.SetFragment (uri.GetFragment ());
    return result;
}
