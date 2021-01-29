/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/ToString.h"
#include "../../../Memory/ObjectFieldUtilities.h"

#include "Headers.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

namespace {
    constexpr auto kHeaderNameComparer_ = String::EqualsComparer{CompareOptions::eCaseInsensitive};
}

/*
 ********************************************************************************
 ******************************** HTTP::Headers *********************************
 ********************************************************************************
 */
Headers::Headers ()
    : pCacheControl{
          [] (const auto* property) {
              const Headers*                                     headerObj = Memory::GetObjectOwningField (property, &Headers::pCacheControl);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              return headerObj->fCacheControl_;
          },
          [] (auto* property, const auto& cacheControl) {
              Headers*                                           headerObj = Memory::GetObjectOwningField (property, &Headers::pCacheControl);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              headerObj->fCacheControl_ = cacheControl;
          }}
    , pConnection{
          [] (const auto* property) -> optional<ConnectionValue> {
              const Headers* headerObj = Memory::GetObjectOwningField (property, &Headers::pConnection);
              using CompareOptions     = Characters::CompareOptions;
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              if (auto connectionHdr = headerObj->As<Mapping<String, String>> ().Lookup (HeaderName::kConnection)) {
                  if (kHeaderNameComparer_ (*connectionHdr, L"Keep-Alive"sv)) {
                      return eKeepAlive;
                  }
                  else if (kHeaderNameComparer_ (*connectionHdr, L"close"sv)) {
                      return eClose;
                  }
              }
              return nullopt;
          },
          [] (auto* property, const auto& connectionValue) {
              Headers*                                           headerObj = Memory::GetObjectOwningField (property, &Headers::pConnection);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
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
              headerObj->Set (HeaderName::kConnection, v);
          }}
    , pContentLength{
          [] (const auto* property) {
              const Headers*                                     headerObj = Memory::GetObjectOwningField (property, &Headers::pContentLength);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              return headerObj->fContentLength_;
          },
          [] (auto* property, auto contentLength) {
              Headers*                                           headerObj = Memory::GetObjectOwningField (property, &Headers::pContentLength);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              headerObj->fContentLength_ = contentLength;
          }}
    , pContentType{
          [] (const auto* property) {
              const Headers*                                     headerObj = Memory::GetObjectOwningField (property, &Headers::pContentType);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              return headerObj->fContentType_;
          },
          [] (auto* property, const auto& contentType) {
              Headers*                                           headerObj = Memory::GetObjectOwningField (property, &Headers::pContentType);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              headerObj->fContentType_ = contentType;
          }}
    , pETag{
          [] (const auto* property) {
              const Headers*                                     headerObj = Memory::GetObjectOwningField (property, &Headers::pETag);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              return headerObj->fETag_;
          },
          [] (auto* property, const auto& etag) {
              Headers*                                           headerObj = Memory::GetObjectOwningField (property, &Headers::pETag);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              headerObj->fETag_ = etag;
          }}
    , pIfNoneMatch{
          [] (const auto* property) {
              const Headers*                                     headerObj = Memory::GetObjectOwningField (property, &Headers::pIfNoneMatch);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              return headerObj->fIfNoneMatch_;
          },
          [] (auto* property, const auto& ifNoneMatch) {
              Headers*                                           headerObj = Memory::GetObjectOwningField (property, &Headers::pIfNoneMatch);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*headerObj};
              headerObj->fIfNoneMatch_ = ifNoneMatch;
          }}
{
}

Headers::Headers (const Headers& src)
    : Headers ()
{
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_  = src.fExtraHeaders_;
    fCacheControl_  = src.fCacheControl_;
    fContentLength_ = src.fContentLength_;
    fContentType_   = src.fContentType_;
    fETag_          = src.fETag_;
    fIfNoneMatch_   = src.fIfNoneMatch_;
}

Headers::Headers (Headers&& src)
    : Headers ()
{
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_  = move (src.fExtraHeaders_);
    fCacheControl_  = move (src.fCacheControl_);
    fContentLength_ = move (src.fContentLength_);
    fContentType_   = move (src.fContentType_);
    fETag_          = move (src.fETag_);
    fIfNoneMatch_   = move (src.fIfNoneMatch_);
}

Headers::Headers (const Iterable<KeyValuePair<String, String>>& src)
    : Headers ()
{
    for (auto kv : src) {
        Set (kv.fKey, kv.fValue);
    }
}

Headers& Headers::operator= (Headers&& rhs)
{
    fExtraHeaders_  = move (rhs.fExtraHeaders_);
    fCacheControl_  = move (rhs.fCacheControl_);
    fContentLength_ = move (rhs.fContentLength_);
    fContentType_   = move (rhs.fContentType_);
    fETag_          = move (rhs.fETag_);
    fIfNoneMatch_   = move (rhs.fIfNoneMatch_);
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

void Headers::Remove (const String& headerName)
{
    if (SetBuiltin_ (headerName, nullopt)) {
        return;
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    fExtraHeaders_.Remove ([=] (const auto& i) { return kHeaderNameComparer_ (i.fKey, headerName); });
}

void Headers::Add (const String& headerName, const String& value)
{
    if (SetBuiltin_ (headerName, value)) {
        return;
    }
    fExtraHeaders_.Add (KeyValuePair<String, String>{headerName, value});
}

bool Headers::SetBuiltin_ (const String& headerName, const optional<String>& value)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (kHeaderNameComparer_ (headerName, HeaderName::kCacheControl)) {
        fCacheControl_ = value ? CacheControl::Parse (*value) : optional<CacheControl>{};
        return true;
    }
    else if (kHeaderNameComparer_ (headerName, HeaderName::kContentLength)) {
        fContentLength_ = value ? String2Int<uint64_t> (*value) : optional<uint64_t>{};
        return true;
    }
    else if (kHeaderNameComparer_ (headerName, HeaderName::kContentType)) {
        fContentType_ = value ? InternetMediaType{*value} : optional<InternetMediaType>{};
        return true;
    }
    else if (kHeaderNameComparer_ (headerName, HeaderName::kETag)) {
        fETag_ = value ? ETag::Parse (*value) : optional<ETag>{};
        return true;
    }
    else if (kHeaderNameComparer_ (headerName, HeaderName::kIfNoneMatch)) {
        fIfNoneMatch_ = value ? IfNoneMatch::Parse (*value) : optional<IfNoneMatch>{};
        return true;
    }
    return false;
}

void Headers::Set (const String& headerName, const optional<String>& value)
{
    if (SetBuiltin_ (headerName, value)) {
        return;
    }
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    fExtraHeaders_.Remove ([=] (const auto& i) { return kHeaderNameComparer_ (i.fKey, headerName); });
    if (value) {
        fExtraHeaders_.Add (KeyValuePair<String, String>{headerName, *value});
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
