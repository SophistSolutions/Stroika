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
    StringBuilder result;
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

URI URI::GetNormalized () const
{
    optional<SchemeType> scheme = fScheme_;
    if (scheme) {
        scheme = UniformResourceIdentification::NormalizeScheme (*scheme);
    }
    optional<Authority> authority = fAuthority_;
    if (authority) {
        //@todo NORMALIZE--- authority = authority->ToLowerCase ();
    }
    String path = fPath_; // @todo https://tools.ietf.org/html/rfc3986#section-6.2.2.3

    return URI{scheme, authority, path, fQuery_, fFragment_};
}

String URI::ToString () const
{
    return Characters::ToString (As<String> ());
}
