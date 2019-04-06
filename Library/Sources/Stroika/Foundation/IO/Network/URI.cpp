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
    return Parse (DataExchange::CheckedConverter<String, DataExchange::ASCII> (rawURL));
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
    if (DataExchange::CheckedConverter<String, DataExchange::ASCII> (rawURL).Match (kParseURLRegExp_, nullptr, &scheme, nullptr, &authority, &path, nullptr, &query, nullptr, &fragment)) {
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
        scheme = UniformResourceIdentification::NormalizeScheme (*scheme);
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
