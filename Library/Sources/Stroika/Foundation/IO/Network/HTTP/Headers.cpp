/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Containers/Set.h"
#include "../../../Streams/TextReader.h"

#include "Headers.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

namespace {
    const auto kHeaderNameComparer_ = String::EqualsComparer{CompareOptions::eCaseInsensitive};
}

/*
 ********************************************************************************
 ******************************** HTTP::Headers *********************************
 ********************************************************************************
 */
Headers::Headers (const Iterable<KeyValuePair<String, String>>& src)
{
    for (auto kv : src) {
        SetHeader (kv.fKey, kv.fValue);
    }
}

void Headers::SetHeader (const String& name, const optional<String>& value)
{
    if (kHeaderNameComparer_ (name, HeaderName::kCacheControl)) {
        fCacheControl_ = value ? CacheControl::Parse (*value) : optional<CacheControl>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kContentLength)) {
        fContentLength_ = value ? String2Int<uint64_t> (*value) : optional<uint64_t>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kContentType)) {
        fContentType_ = value ? InternetMediaType{*value} : optional<InternetMediaType>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kETag)) {
        fETag_ = value ? ETag::Parse (*value) : optional<ETag>{};
    }
    else {
        if (value) {
            fExtraHeaders_.Add (KeyValuePair<String, String>{name, *value});
        }
        else {
            fExtraHeaders_.Remove ([=] (const auto& i) { return kHeaderNameComparer_ (i.fKey, name); });
        }
    }
}

template <>
Mapping<String, String> Headers::As () const
{
    return As<Collection<KeyValuePair<String, String>>> ().As<Mapping<String, String>> ();
}

template <>
Collection<KeyValuePair<String, String>> Headers::As () const
{
    Collection<KeyValuePair<String, String>> results = fExtraHeaders_;
    if (fCacheControl_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kCacheControl, fCacheControl_->As<String> ()});
    }
    if (fContentLength_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kContentLength, Characters::Format (L"%ld", fContentLength_)});
    }
    if (fContentType_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kContentType, fContentType_->As<String> ()});
    }
    if (fETag_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kETag, fETag_->As<String> ()});
    }
    return results;
}
