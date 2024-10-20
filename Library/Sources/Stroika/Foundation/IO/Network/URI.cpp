/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/CheckedConverter.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"

#include "URI.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using Debug::AssertExternallySynchronizedMutex;

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
            return "/" + s;
        }
        return s;
    }
}

namespace {
    String remove_dot_segments_ (const String& p, URI::NormalizationStyle normalization = URI::NormalizationStyle::eRFC3986)
    {
        // @todo - this is a fairly inefficient implementation, but so far hasn't shown up in profiles
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        //Debug::TraceContextBumper{"remove_dot_segments_", "p={},normalization={}"_f, p, normalization};
#endif
        // from https://tools.ietf.org/html/rfc3986#section-5.2.4
        vector<String> segments; // for our purpose here, segments may (or not in case of first) contain a leading /
        StringBuilder  accumulatingSegment;
        for (Character c : p) {
            if (c == '/' and not accumulatingSegment.empty ()) {
                segments.push_back (accumulatingSegment.str ());
                accumulatingSegment.clear ();
            }
            accumulatingSegment << c;
        }
        if (not accumulatingSegment.empty ()) {
            segments.push_back (accumulatingSegment.str ());
        }
        vector<String> segments2;                         // apply ../. removal
        bool           lastSegmentShouldHaveSlash{false}; // not sure about this
        for (const String& segment : segments) {
            lastSegmentShouldHaveSlash = false;
            if (segment == "."sv or segment == "/."sv) {
                // drop it on the floor
                if (segment[0] == '/') {
                    lastSegmentShouldHaveSlash = true;
                }
            }
            else if (segment == ".."sv or segment == "/.."sv) {
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
        bool          soFarEndsWithSlash = false;
        for (const String& segment : segments2) {
            if (normalization == URI::NormalizationStyle::eAggressive) {
                if (segment.StartsWith ('/') and soFarEndsWithSlash) {
                    String add = segment.SubString (1);
                    if (not add.empty ()) {
                        result << add;
                        soFarEndsWithSlash = add.EndsWith ('/');
                    }
                }
                else {
                    result << segment;
                    soFarEndsWithSlash = segment.EndsWith ('/');
                }
            }
            else {
                result << segment;
            }
        }
        if (lastSegmentShouldHaveSlash and not result.str ().EndsWith ("/"sv)) {
            result << "/"sv;
        }
        return result;
    };
}

URI URI::Parse (const String& rawURL)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper{"IO::Network::URI::Parse", "{}"_f, rawURL};
#endif
    // https://tools.ietf.org/html/rfc3986#appendix-B
    static const RegularExpression kParseURLRegExp_{"^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"_RegEx};
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
    if (rawURL.Matches (kParseURLRegExp_, nullptr, &scheme, nullptr, &authority, &path, nullptr, &query, nullptr, &fragment)) {
        return URI{emptyStr2Missing (scheme), Authority::Parse (authority.value_or (String{})),
                   UniformResourceIdentification::PCTDecode2String (path.value_or (String{})), emptyStr2Missing (query), emptyStr2Missing (fragment)};
    }
    else {
        static const Execution::RuntimeErrorException kException_{"Ill-formed URI"sv};
        Execution::Throw (kException_); // doesn't match regexp in https://tools.ietf.org/html/rfc3986#appendix-B
    }
}

URI URI::ParseRelative (const String& rawRelativeURL)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper{"IO::Network::URI::ParseRelative", "{}"_f, rawRelativeURL};
#endif
    // https://tools.ietf.org/html/rfc3986#appendix-B
    static const RegularExpression kParseRelativeURLRegExp_{"([^?#]*)(\\?([^#]*))?(#(.*))?"_RegEx};
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
    (void)rawRelativeURL.AsASCII (); // for throw check side-effect
    if (rawRelativeURL.Matches (kParseRelativeURLRegExp_, &path, nullptr, &query, nullptr, &fragment)) {
        return URI{nullopt, nullopt, UniformResourceIdentification::PCTDecode2String (path.value_or (String{})), emptyStr2Missing (query),
                   emptyStr2Missing (fragment)};
    }
    else {
        static const Execution::RuntimeErrorException kException_{"Ill-formed relative URI"sv};
        Execution::Throw (kException_); // doesn't match regexp in https://tools.ietf.org/html/rfc3986#appendix-B
    }
}

String URI::AsString_ (optional<StringPCTEncodedFlag> pctEncode) const
{
    // http://stroika-bugs.sophists.com/browse/STK-1000 -- issue about maybe needed more nuanced approach
    StringPCTEncodedFlag                           usingPCTEncodeFlag = pctEncode.value_or (eDecoded);
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    StringBuilder                                  result;
    if (fScheme_) {
        // From https://tools.ietf.org/html/rfc3986#appendix-A
        //      scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        // no need to pct encode this
        Assert (fScheme_->All ([] (Character c) { return c.IsASCII (); }));
        result << *fScheme_ << ":"sv;
    }
    if (fAuthority_) {
        Assert (fAuthority_->As<String> (usingPCTEncodeFlag).All ([] (Character c) { return c.IsASCII (); }));
        result << "//"sv << fAuthority_->As<String> (usingPCTEncodeFlag);
    }

    if (fAuthority_ and not(fPath_.empty () or fPath_.StartsWith ("/"sv))) {
        // NOT SURE HOW TO HANDLE
        static const Execution::RuntimeErrorException kException_{"This is not a legal URI to encode (authority present, but path not empty or absolute)"sv};
        Execution::Throw (kException_);
    }

    if (usingPCTEncodeFlag == eDecoded) {
        result << fPath_;
    }
    else {
        static constexpr UniformResourceIdentification::PCTEncodeOptions kPathEncodeOptions_{
            .allowSubDelims = false, .allowGenDelims = false, .allowPChar = true, .allowFragOrQueryChars = false, .allowPathCharacters = true};
        result << UniformResourceIdentification::PCTEncode2String (fPath_, kPathEncodeOptions_);
    }

    if (fQuery_) {
        static constexpr UniformResourceIdentification::PCTEncodeOptions kQueryEncodeOptions_{
            .allowSubDelims = false, .allowGenDelims = false, .allowPChar = false, .allowFragOrQueryChars = true};
        if (usingPCTEncodeFlag == eDecoded) {
            result << "?"sv << *fQuery_;
        }
        else {
            result << "?"sv << UniformResourceIdentification::PCTEncode2String (*fQuery_, kQueryEncodeOptions_);
        }
    }
    if (fFragment_) {
        if (usingPCTEncodeFlag == eDecoded) {
            result << "#"sv + *fFragment_;
        }
        else {
            static constexpr UniformResourceIdentification::PCTEncodeOptions kFragmentEncodeOptions_{false, false, false, true};
            result << "#"sv + UniformResourceIdentification::PCTEncode2String (*fFragment_, kFragmentEncodeOptions_);
        }
    }
    Ensure (result.str ().All ([] (Character c) { return c.IsASCII (); }));
    return result.str ();
}

URI::operator bool () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
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

String URI::GetAuthorityRelativeResourceDir () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    static const RegularExpression                 kSelectDir_ = "(.*\\/)[^\\/]*"_RegEx;
    optional<String>                               baseDir;
    (void)fPath_.Matches (kSelectDir_, &baseDir);
    return baseDir.value_or (String{});
}

URI URI::Normalize (NormalizationStyle normalization) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    optional<SchemeType>                           scheme = fScheme_;
    if (scheme) {
        scheme = scheme->Normalize ();
    }
    optional<Authority> authority = fAuthority_;
    if (authority) {
        authority = authority->Normalize ();
    }
    String path = remove_dot_segments_ (fPath_, normalization); // review https://tools.ietf.org/html/rfc3986#section-6.2.2.3 - this algorithm for removing dots was from merge code, so not sure it applies here
    return URI{scheme, authority, path, fQuery_, fFragment_};
}

String URI::ToString () const
{
    // dont use As<String> () because this can throw if bad string - and no need to pct-encode here
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    StringBuilder                                  result;
    if (fScheme_) {
        result << *fScheme_ << ":"sv;
    }
    if (fAuthority_) {
        result << "//"sv << fAuthority_->As<String> ();
    }
    result << fPath_;
    if (fQuery_) {
        result << "?"sv << *fQuery_;
    }
    if (fFragment_) {
        result << "#"sv << *fFragment_;
    }
    return result;
}

void URI::CheckValidPathForAuthority_ (const optional<Authority>& authority, const String& path)
{
    /*
     *  https://tools.ietf.org/html/rfc3986#section-3.3
     *      If a URI contains an authority component, then the path component
     *      must either be empty or begin with a slash ("/") character
     */
    if (authority and (not path.empty () and not path.StartsWith ("/"sv))) {
        static const Execution::RuntimeErrorException kException_{"A URI with an authority must have an empty path, or an absolute path"sv};
        Execution::Throw (kException_);
    }
}

URI URI::Combine (const URI& overridingURI) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};

    /*
     *  This is not strictly according to Hoyle, but it avoids a common inconvenience with the Scheme check below. And avoids having to write a lot of
     *  code like:
     *      if (l) {
     *          return l.Combine (r);
     *      }
     *      else {
     *          return r;
     *      }
     */
    if (not *this) {
        return overridingURI;
    }

    /*
     *  From https://tools.ietf.org/html/rfc3986#section-5
     *      "Note that only the scheme component is required to be present in a base URI; the other components may be empty or undefined."
     */
    URI baseURI = Normalize ();
    if (not baseURI.GetScheme ()) {
        static const Execution::RuntimeErrorException kException_{"Scheme is required in base URI to combine with another URI"sv};
        Execution::Throw (kException_);
    }
    auto merge = [&] (const String& base, const String& rhs) -> String {
        // @see https://tools.ietf.org/html/rfc3986#section-5.2.3
        if (baseURI.GetAuthority () and base.empty ()) {
            return "/"sv + rhs;
        }
        static const RegularExpression kSelectDir_ = "(.*\\/)[^\\/]*"_RegEx;
        optional<String>               baseDir;
        (void)base.Matches (kSelectDir_, &baseDir);
        return baseDir.value_or (String{}) + rhs;
    };

    Assert (remove_dot_segments_ ("/a/b/c/./../../g") == "/a/g");    // from https://tools.ietf.org/html/rfc3986#section-5.2.4
    Assert (remove_dot_segments_ ("mid/content=5/../6") == "mid/6"); // ditto

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
    if (overridingURI.GetScheme ()) {
        result.SetScheme (overridingURI.GetScheme ());
        result.SetAuthority (overridingURI.GetAuthority ());
        result.SetPath (remove_dot_segments_ (overridingURI.GetPath ()));
        result.SetQuery (overridingURI.GetQuery<String> ());
    }
    else {
        result.SetScheme (baseURI.GetScheme ());
        if (overridingURI.GetAuthority ()) {
            result.SetAuthority (overridingURI.GetAuthority ());
            result.SetPath (remove_dot_segments_ (overridingURI.GetPath ()));
            result.SetQuery (overridingURI.GetQuery<String> ());
        }
        else {
            result.SetAuthority (baseURI.GetAuthority ());
            if (overridingURI.GetPath ().empty ()) {
                result.SetPath (baseURI.GetPath ());
                result.SetQuery (overridingURI.GetQuery<String> () ? overridingURI.GetQuery<String> () : baseURI.GetQuery<String> ());
            }
            else {
                if (overridingURI.GetPath ().StartsWith ("/"sv)) {
                    result.SetPath (remove_dot_segments_ (overridingURI.GetPath ()));
                }
                else {
                    result.SetPath (remove_dot_segments_ (merge (baseURI.GetPath (), overridingURI.GetPath ())));
                }
                result.SetQuery (overridingURI.GetQuery<String> ());
            }
        }
    }
    result.SetFragment (overridingURI.GetFragment ());
    return result;
}

strong_ordering URI::TWC_ (const URI& lhs, const URI& rhs)
{
    using namespace UniformResourceIdentification;
    if (auto cmp = Configuration::StdCompat::compare_three_way{}(lhs.GetScheme (), rhs.GetScheme ()); cmp != strong_ordering::equal) {
        return cmp;
    }
    if (auto cmp = Configuration::StdCompat::compare_three_way{}(lhs.GetAuthority (), rhs.GetAuthority ()); cmp != strong_ordering::equal) {
        return cmp;
    }
    if (auto cmp = Configuration::StdCompat::compare_three_way{}(lhs.GetPath (), rhs.GetPath ()); cmp != strong_ordering::equal) {
        return cmp;
    }
    if (auto cmp = Configuration::StdCompat::compare_three_way{}(lhs.GetQuery (), rhs.GetQuery ()); cmp != strong_ordering::equal) {
        return cmp;
    }
    if (auto cmp = Configuration::StdCompat::compare_three_way{}(lhs.GetFragment (), rhs.GetFragment ()); cmp != strong_ordering::equal) {
        return cmp;
    }
    return strong_ordering::equal;
}

/*
 ********************************************************************************
 *********** hash<Stroika::Foundation::IO::Network::URI> ************************
 ********************************************************************************
 */
size_t std::hash<Stroika::Foundation::IO::Network::URI>::operator() (const Stroika::Foundation::IO::Network::URI& arg) const
{
    return hash<Characters::String> () (arg.As<Characters::String> ());
}
