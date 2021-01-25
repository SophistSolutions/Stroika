/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/ToString.h"

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
Headers::Headers ()
    : pCacheControl{
          [this] () {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              return fCacheControl_;
          },
          [this] (const auto& cacheControl) {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              fCacheControl_ = cacheControl;
          }}
    , pContentLength{
          [this] () {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              return fContentLength_;
          },
          [this] (auto contentLength) {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              fContentLength_ = contentLength;
          }}
    , pContentType{
          [this] () {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              return fContentType_;
          },
          [this] (const auto& contentType) {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              fContentType_ = contentType;
          }}
    , pETag{
          [this] () {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              return fETag_;
          },
          [this] (const auto& etag) {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              fETag_ = etag;
          }}
    , pIfNoneMatch{
          [this] () {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              return fIfNoneMatch_;
          },
          [this] (const auto& ifNoneMatch) {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              fIfNoneMatch_ = ifNoneMatch;
          }}
    , pConnection{
          [this] () -> optional<ConnectionValue> {
              using CompareOptions = Characters::CompareOptions;
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              if (auto connectionHdr = As<Mapping<String, String>> ().Lookup (HeaderName::kConnection)) {
                  if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(*connectionHdr, L"Keep-Alive"sv)) {
                      return eKeepAlive;
                  }
                  else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(*connectionHdr, L"close"sv)) {
                      return eClose;
                  }
              }
              return optional<ConnectionValue>{};
          },
          [this] (const auto& connectionValue) {
              lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
              optional<String>                                   v;
              if (connectionValue) {
                  switch (*connectionValue) {
                      case ConnectionValue::eKeepAlive:
                          v = L"Keep-Alive"sv;
                          break;
                      case ConnectionValue::eClose:
                          v = L"close"sv;
                          break;
                  }
              }
              SetHeader (HeaderName::kConnection, v);
          }}
{
}

Headers::Headers (const Headers& src)
    : Headers ()
{
    fExtraHeaders_ = src.fExtraHeaders_;

    // NOTE - cannot INITIALIZE properties with src.Properties values since they are not copy constructible
    // but they are assignable, so do that
    pCacheControl  = src.pCacheControl;
    pContentLength = src.pContentLength;
    pContentType   = src.pContentType;
    pETag          = src.pETag;
    pIfNoneMatch   = src.pIfNoneMatch;
}

Headers::Headers (Headers&& src)
    : Headers ()
{
    fExtraHeaders_ = move (src.fExtraHeaders_);
    // NOTE - cannot INITIALIZE properties with src.Properties values since they are not copy constructible
    // but they are assignable, so do that
    pCacheControl  = src.pCacheControl;
    pContentLength = src.pContentLength;
    pContentType   = src.pContentType;
    pETag          = src.pETag;
    pIfNoneMatch   = src.pIfNoneMatch;
}

Headers::Headers (const Iterable<KeyValuePair<String, String>>& src)
    : Headers ()
{
    for (auto kv : src) {
        SetHeader (kv.fKey, kv.fValue);
    }
}

Headers& Headers::operator= (Headers&& rhs)
{
    fExtraHeaders_ = move (rhs.fExtraHeaders_);
    pCacheControl  = rhs.pCacheControl;
    pContentLength = rhs.pContentLength;
    pContentType   = rhs.pContentType;
    pETag          = rhs.pETag;
    pIfNoneMatch   = rhs.pIfNoneMatch;
    return *this;
}

optional<String> Headers::LookupOne (const String& name) const
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (kHeaderNameComparer_ (name, HeaderName::kCacheControl)) {
        return fCacheControl_ ? fCacheControl_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kContentLength)) {
        return fContentLength_ ? Characters::Format (L"%ld", *fContentLength_) : optional<String>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kContentType)) {
        return fContentType_ ? fContentType_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kETag)) {
        return fETag_ ? fETag_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameComparer_ (name, HeaderName::kIfNoneMatch)) {
        return fIfNoneMatch_ ? fIfNoneMatch_->As<String> () : optional<String>{};
    }
    else {
        // should switch to new non-existent class Assocation here - and use that...more efficeint -
        if (auto ri = fExtraHeaders_.FindFirstThat ([&] (const auto& i) { return kHeaderNameComparer_ (name, i.fKey); })) {
            return ri->fValue;
        }
        return nullopt;
    }
}

void Headers::SetHeader (const String& name, const optional<String>& value)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
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
    else if (kHeaderNameComparer_ (name, HeaderName::kIfNoneMatch)) {
        fIfNoneMatch_ = value ? IfNoneMatch::Parse (*value) : optional<IfNoneMatch>{};
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
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Collection<KeyValuePair<String, String>>           results = fExtraHeaders_;
    if (fCacheControl_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kCacheControl, fCacheControl_->As<String> ()});
    }
    if (fContentLength_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kContentLength, Characters::Format (L"%lld", static_cast<long long> (*fContentLength_))});
    }
    if (fContentType_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kContentType, fContentType_->As<String> ()});
    }
    if (fETag_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kETag, fETag_->As<String> ()});
    }
    if (fIfNoneMatch_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kIfNoneMatch, fIfNoneMatch_->As<String> ()});
    }
    return results;
}

String Headers::ToString () const
{
    return Characters::ToString (As<Mapping<String, String>> ());
}
