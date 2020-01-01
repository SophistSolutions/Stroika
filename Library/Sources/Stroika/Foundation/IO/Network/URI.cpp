/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
namespace {
    String PatchOldStroikaURLPath2NewPath_ (const optional<URI::Authority>& a, const String& s)
    {
        if (s.empty ()) {
            return s;
        }
        if (s.StartsWith (L"/")) {
            return s;
        }
        if (a) {
            return L"/" + s;
        }
        return s;
    }
}

namespace {
    String remove_dot_segments_ (const String& p)
    {
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
        return URI{emptyStr2Missing (scheme), Authority::Parse (authority.value_or (String{})), UniformResourceIdentification::PCTDecode2String (path.value_or (String{})), emptyStr2Missing (query), emptyStr2Missing (fragment)};
    }
    else {
        Execution::Throw (Execution::RuntimeErrorException (L"Ill-formed URI"sv)); // doesn't match regexp in https://tools.ietf.org/html/rfc3986#appendix-B
    }
}

template <>
String URI::As () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       result;
    if (fScheme_) {
        // From https://tools.ietf.org/html/rfc3986#appendix-A
        //      scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        // no need to pct encode this
        Assert (fScheme_->All ([] (Character c) { return c.IsASCII (); }));
        result += *fScheme_ + L":";
    }
    if (fAuthority_) {
        Assert (fAuthority_->As<String> ().All ([] (Character c) { return c.IsASCII (); }));
        result += L"//" + fAuthority_->As<String> (); // this already produces 'raw' (pct encoded) format
    }

    if (fAuthority_ and not(fPath_.empty () or fPath_.StartsWith (L"/"))) {
        // NOT SURE HOW TO HANDLE
        Execution::Throw (Execution::RuntimeErrorException (L"This is not a legal URI to encode (authority present, but path not empty or absolute)"));
    }

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
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    // @todo - Could be more efficient doing String algorithm directly
    return As<String> ().AsASCII ();
}

URI::operator bool () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (fScheme_) {
        return true;
    }
    if (fAuthority_) {
        return true;
    }
    if (not fPath_.empty ()) {
        return true;
    }
    if (fQuery_) {
        return true;
    }
    if (fFragment_) {
        return true;
    }
    return false;
}

template <>
String URI::GetAuthorityRelativeResource () const
{
    shared_lock<const AssertExternallySynchronizedLock>              critSec{*this};
    static constexpr UniformResourceIdentification::PCTEncodeOptions kPathEncodeOptions_{false, false, false, false, true};
    Characters::StringBuilder                                        result = UniformResourceIdentification::PCTEncode2String (fPath_, kPathEncodeOptions_);
    if (fQuery_) {
        static constexpr UniformResourceIdentification::PCTEncodeOptions kQueryEncodeOptions_{false, false, false, true};
        result += L"?"sv + UniformResourceIdentification::PCTEncode2String (*fQuery_, kQueryEncodeOptions_);
    }
    return result.str ();
}

String URI::GetAuthorityRelativeResourceDir () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    static const RegularExpression                      kSelectDir_ = L"(.*\\/)[^\\/]*"_RegEx;
    optional<String>                                    baseDir;
    (void)fPath_.Match (kSelectDir_, &baseDir);
    return baseDir.value_or (String{});
}

URI URI::Normalize () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    optional<SchemeType>                                scheme = fScheme_;
    if (scheme) {
        scheme = scheme->Normalize ();
    }
    optional<Authority> authority = fAuthority_;
    if (authority) {
        authority = authority->Normalize ();
    }
    String path = remove_dot_segments_ (fPath_); // review https://tools.ietf.org/html/rfc3986#section-6.2.2.3 - this algorithm for removing dots was from merge code, so not sure it applies here

    return URI{scheme, authority, path, fQuery_, fFragment_};
}

String URI::ToString () const
{
    // dont use As<String> () because this can throw if bad string - and no need to pct-encode here
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       result;
    if (fScheme_) {
        result += *fScheme_ + L":";
    }
    if (fAuthority_) {
        result += L"//" + fAuthority_->As<String> ();
    }
    result += fPath_;
    if (fQuery_) {
        result += L"?"sv + *fQuery_;
    }
    if (fFragment_) {
        result += L"#"sv + *fFragment_;
    }
    return Characters::ToString (result.str ());
}

void URI::CheckValidPathForAuthority_ (const optional<Authority>& authority, const String& path)
{
    /*
     *  https://tools.ietf.org/html/rfc3986#section-3.3
     *      If a URI contains an authority component, then the path component
     *      must either be empty or begin with a slash ("/") character
     */
    if (authority and (not path.empty () and not path.StartsWith (L"/"))) {
        Execution::Throw (Execution::RuntimeErrorException (L"A URI with an authority must have an empty path, or an absolute path"sv));
    }
}

URI URI::Combine (const URI& uri) const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};

    /*
     *  This is not stricly according to Hoyle, but it avoids a common inconvenience with the Scheme check below. And avoids having to write alot of
     *  code like:
     *      if (l) {
     *          return l.Combine (r);
     *      }
     *      else {
     *          return r;
     *      }
     */
    if (not*this) {
        return uri;
    }

    /*
     *  From https://tools.ietf.org/html/rfc3986#section-5
     *      "Note that only the scheme component is required to be present in a base URI; the other components may be empty or undefined."
     */
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

    Assert (remove_dot_segments_ (L"/a/b/c/./../../g") == L"/a/g");    // from https://tools.ietf.org/html/rfc3986#section-5.2.4
    Assert (remove_dot_segments_ (L"mid/content=5/../6") == L"mid/6"); // ditto

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
        result.SetPath (remove_dot_segments_ (uri.GetPath ()));
        result.SetQuery (uri.GetQuery<String> ());
    }
    else {
        result.SetScheme (baseURI.GetScheme ());
        if (uri.GetAuthority ()) {
            result.SetAuthority (uri.GetAuthority ());
            result.SetPath (remove_dot_segments_ (uri.GetPath ()));
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
                    result.SetPath (remove_dot_segments_ (uri.GetPath ()));
                }
                else {
                    result.SetPath (remove_dot_segments_ (merge (baseURI.GetPath (), uri.GetPath ())));
                }
                result.SetQuery (uri.GetQuery<String> ());
            }
        }
    }
    result.SetFragment (uri.GetFragment ());
    return result;
}

/*
 ********************************************************************************
 **************************** URI::ThreeWayComparer *****************************
 ********************************************************************************
 */
int URI::ThreeWayComparer::operator() (const URI& lhs, const URI& rhs) const
{
    using namespace UniformResourceIdentification;
    if (int cmp = Common::ThreeWayCompare (lhs.GetScheme (), rhs.GetScheme ())) {
        return cmp;
    }
    if (int cmp = Common::ThreeWayCompare (lhs.GetAuthority (), rhs.GetAuthority ())) {
        return cmp;
    }
    if (int cmp = Common::ThreeWayCompare (lhs.GetPath (), rhs.GetPath ())) {
        return cmp;
    }
    if (int cmp = Common::ThreeWayCompare (lhs.GetQuery (), rhs.GetQuery ())) {
        return cmp;
    }
    if (int cmp = Common::ThreeWayCompare (lhs.GetFragment (), rhs.GetFragment ())) {
        return cmp;
    }
    return 0;
}
