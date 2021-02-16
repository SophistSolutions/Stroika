/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/ToString.h"
#include "../../../Containers/Set.h"

#include "Headers.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

namespace {
    constexpr wstring_view kKeepAlive_ = L"Keep-Alive"sv;
    constexpr wstring_view kClose_     = L"close"sv;
}

/*
 ********************************************************************************
 ******************************** HTTP::Headers *********************************
 ********************************************************************************
 */
Headers::Headers ()
    : accessControlAllowOrigin{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<String> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::accessControlAllowOrigin);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kAccessControlAllowOrigin);
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& accessControlAllowOrigin) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::accessControlAllowOrigin);
              thisObj->SetExtras_ (HeaderName::kAccessControlAllowOrigin, accessControlAllowOrigin);
          }}
    , allow{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<Containers::Set<String>> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::allow);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              if (auto hdr = thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kAllow)) {
                  return Containers::Set<String>{hdr->Tokenize ({','})};
              }
              return nullopt;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& allowed) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::allow);
              thisObj->SetExtras_ (HeaderName::kAllow, allowed ? String::Join (*allowed) : optional<String>{});
          }}
    , cacheControl{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cacheControl);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fCacheControl_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& cacheControl) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cacheControl);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fCacheControl_ = cacheControl;
          }}
    , connection{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<ConnectionValue> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::connection);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              if (auto connectionHdr = thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kConnection)) {
                  if (kHeaderNameEqualsComparer (*connectionHdr, kKeepAlive_)) {
                      return eKeepAlive;
                  }
                  else if (kHeaderNameEqualsComparer (*connectionHdr, kClose_)) {
                      return eClose;
                  }
              }
              return nullopt;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& connectionValue) {
              Headers*         thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::connection);
              optional<String> v;
              if (connectionValue) {
                  switch (*connectionValue) {
                      case ConnectionValue::eKeepAlive:
                          v = kKeepAlive_;
                          break;
                      case ConnectionValue::eClose:
                          v = kClose_;
                          break;
                  }
              }
              thisObj->SetExtras_ (HeaderName::kConnection, v);
          }}
    , contentLength{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fContentLength_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto contentLength) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fContentLength_ = contentLength;
          }}
    , contentType{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentType);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fContentType_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& contentType) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentType);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fContentType_ = contentType;
          }}
    , cookie{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cookie);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return Memory::NullCoalesce (thisObj->fCookieList_);
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& cookies) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cookie);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fCookieList_ = cookies.cookieDetails ().empty () ? optional<CookieList>{} : cookies;
          }}
    , ETag{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ETag);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fETag_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& etag) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ETag);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fETag_ = etag;
          }}
    , host{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::host);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fHost_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& host) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::host);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fHost_ = host;
          }}
    , ifNoneMatch{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ifNoneMatch);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fIfNoneMatch_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& ifNoneMatch) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ifNoneMatch);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fIfNoneMatch_ = ifNoneMatch;
          }}
    , keepAlive{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<HTTP::KeepAlive> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::keepAlive);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              if (auto hdrValue = thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kKeepAlive)) {
                  return HTTP::KeepAlive::Parse (*hdrValue);
              }
              return nullopt;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& keepAliveValue) {
              Headers*         thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::keepAlive);
              optional<String> v;
              if (keepAliveValue) {
                  v = keepAliveValue->AsValue ();
              }
              thisObj->SetExtras_ (HeaderName::kKeepAlive, v);
          }}
    , location{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<URI> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::location);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              if (auto hdr = thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kLocation)) {
                  return URI::Parse (*hdr);
              }
              return nullopt;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<URI>& location) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::location);
              thisObj->SetExtras_ (HeaderName::kLocation, location ? location->As<String> () : optional<String>{});
          }}
    , origin{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<URI> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::origin);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              if (auto hdr = thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kOrigin)) {
                  return URI::Parse (*hdr);
              }
              return nullopt;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<URI>& origin) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::origin);
              thisObj->SetExtras_ (HeaderName::kOrigin, origin ? origin->As<String> () : optional<String>{});
          }}
    , server{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<String> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::server);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->As<Mapping<String, String>> ().Lookup (HeaderName::kServer);
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<String>& server) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::server);
              thisObj->SetExtras_ (HeaderName::kServer, server);
          }}
    , setCookie{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::setCookie);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return Memory::NullCoalesce (thisObj->fSetCookieList_);
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& cookies) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::setCookie);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fSetCookieList_ = cookies.cookieDetails ().empty () ? optional<CookieList>{} : cookies;
          }}
    , vary{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<Containers::Set<String>> {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::vary);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fVary_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<Containers::Set<String>>& vary) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::vary);
              thisObj->fVary_  = vary;
          }}
{
}

Headers::Headers (const Headers& src)
    : Headers{}
{
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_  = src.fExtraHeaders_;
    fCacheControl_  = src.fCacheControl_;
    fContentLength_ = src.fContentLength_;
    fContentType_   = src.fContentType_;
    fCookieList_    = src.fCookieList_;
    fETag_          = src.fETag_;
    fHost_          = src.fHost_;
    fIfNoneMatch_   = src.fIfNoneMatch_;
    fSetCookieList_ = src.fSetCookieList_;
    fVary_          = src.fVary_;
}

Headers::Headers (Headers&& src)
    : Headers{}
{
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_  = move (src.fExtraHeaders_);
    fCacheControl_  = move (src.fCacheControl_);
    fContentLength_ = move (src.fContentLength_);
    fContentType_   = move (src.fContentType_);
    fCookieList_    = move (src.fCookieList_);
    fETag_          = move (src.fETag_);
    fHost_          = move (src.fHost_);
    fIfNoneMatch_   = move (src.fIfNoneMatch_);
    fSetCookieList_ = move (src.fSetCookieList_);
    fVary_          = move (src.fVary_);
}

Headers::Headers (const Iterable<KeyValuePair<String, String>>& src)
    : Headers{}
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
    fCookieList_    = move (rhs.fCookieList_);
    fETag_          = move (rhs.fETag_);
    fHost_          = move (rhs.fHost_);
    fIfNoneMatch_   = move (rhs.fIfNoneMatch_);
    fSetCookieList_ = move (rhs.fSetCookieList_);
    fVary_          = move (rhs.fVary_);
    return *this;
}

optional<String> Headers::LookupOne (const String& name) const
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (kHeaderNameEqualsComparer (name, HeaderName::kCacheControl)) {
        return fCacheControl_ ? fCacheControl_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentLength)) {
        return fContentLength_ ? Characters::Format (L"%ld", *fContentLength_) : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentType)) {
        return fContentType_ ? fContentType_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kCookie)) {
        return fCookieList_ ? fCookieList_->EncodeForCookieHeader () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kETag)) {
        return fETag_ ? fETag_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kHost)) {
        return fHost_;
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kIfNoneMatch)) {
        return fIfNoneMatch_ ? fIfNoneMatch_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kSetCookie)) {
        // we can only return the first setCookie here... (see LookupAll)
        if (fSetCookieList_.has_value ()) {
            if (auto i = fSetCookieList_->cookieDetails ().First ()) {
                return i->Encode ();
            }
        }
        return optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kVary)) {
        return fVary_ ? String::Join (*fVary_) : optional<String>{};
    }
    else {
        // should switch to new non-existent class Assocation here - and use that...more efficeint -
        if (auto ri = fExtraHeaders_.FindFirstThat ([&] (const auto& i) { return kHeaderNameEqualsComparer (name, i.fKey); })) {
            return ri->fValue;
        }
        return nullopt;
    }
}

Collection<String> Headers::LookupAll (const String& name) const
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (kHeaderNameEqualsComparer (name, HeaderName::kCacheControl)) {
        return fCacheControl_ ? Collection<String>{fCacheControl_->As<String> ()} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentLength)) {
        return fContentLength_ ? Collection<String>{Characters::Format (L"%ld", *fContentLength_)} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentType)) {
        return fContentType_ ? Collection<String>{fContentType_->As<String> ()} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kCookie)) {
        return fCookieList_ ? Collection<String>{fCookieList_->EncodeForCookieHeader ()} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kETag)) {
        return fETag_ ? Collection<String>{fETag_->As<String> ()} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kHost)) {
        return fHost_ ? Collection<String>{*fHost_} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kIfNoneMatch)) {
        return fIfNoneMatch_ ? Collection<String>{fIfNoneMatch_->As<String> ()} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kSetCookie)) {
        // return each 'set' cookie as a separate header value
        if (fSetCookieList_.has_value ()) {
            return fSetCookieList_->cookieDetails ().Select<String> ([] (const auto& i) { return i.Encode (); });
        }
        return Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kVary)) {
        return fVary_ ? Collection<String>{String::Join (*fVary_)} : Collection<String>{};
    }
    else {
        Collection<String> result;
        // should switch to new non-existent class Assocation here - and use that...more efficeint -
        if (auto ri = fExtraHeaders_.FindFirstThat ([&] (const auto& i) { return kHeaderNameEqualsComparer (name, i.fKey); })) {
            result += ri->fValue;
        }
        return result;
    }
}

size_t Headers::Remove (const String& headerName)
{
    size_t nRemoveals{};
    if (UpdateBuiltin_ (AddOrSet::eRemove, headerName, nullopt, &nRemoveals)) {
        return nRemoveals; // could be zero if its builtin, but this doesn't change anything
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    return fExtraHeaders_.RemoveAll ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName); });
}

size_t Headers::Remove (const String& headerName, const String& value)
{
    size_t nRemoveals{};
    if (UpdateBuiltin_ (AddOrSet::eRemove, headerName, value, &nRemoveals)) {
        return nRemoveals;
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    return fExtraHeaders_.RemoveAll ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName) and value == i.fValue; });
}

void Headers::Add (const String& headerName, const String& value)
{
    if (UpdateBuiltin_ (AddOrSet::eAdd, headerName, value)) {
        return;
    }
    fExtraHeaders_.Add (KeyValuePair<String, String>{headerName, value});
}

bool Headers::UpdateBuiltin_ (AddOrSet flag, const String& headerName, const optional<String>& value, size_t* nRemoveals)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if qDebug
    if (value == nullopt) {
        Require (flag == AddOrSet::eRemove or flag == AddOrSet::eSet);
    }
    if (flag == AddOrSet::eRemove) {
        // GENERALLY value will be none, but for some cases (like Set-Cookie) with multiple values, it can be the value you want removed
    }
    if (flag == AddOrSet::eAdd) {
        Require (value.has_value ());
    }
    if (flag == AddOrSet::eSet) {
        // nullopt means remove, and value is what we replace with
    }
#endif
    if (kHeaderNameEqualsComparer (headerName, HeaderName::kCacheControl)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fCacheControl_ != nullopt) ? 1 : 0;
        }
        fCacheControl_ = value ? CacheControl::Parse (*value) : optional<CacheControl>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kContentLength)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fContentLength_ != nullopt) ? 1 : 0;
        }
        fContentLength_ = value ? String2Int<uint64_t> (*value) : optional<uint64_t>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kContentType)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fContentType_ != nullopt) ? 1 : 0;
        }
        fContentType_ = value ? InternetMediaType{*value} : optional<InternetMediaType>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kCookie)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fCookieList_ != nullopt) ? 1 : 0;
        }
        fCookieList_ = value ? CookieList::Decode (*value) : optional<CookieList>{};
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kETag)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fETag_ != nullopt) ? 1 : 0;
        }
        fETag_ = value ? ETag::Parse (*value) : optional<HTTP::ETag>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kHost)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fHost_ != nullopt) ? 1 : 0;
        }
        fHost_ = value;
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kIfNoneMatch)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fIfNoneMatch_ != nullopt) ? 1 : 0;
        }
        fIfNoneMatch_ = value ? IfNoneMatch::Parse (*value) : optional<IfNoneMatch>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kSetCookie)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fSetCookieList_ != nullopt) ? 1 : 0;
        }
        switch (flag) {
            case AddOrSet::eRemove: {
                if (fSetCookieList_ != nullopt) {
                    if (value) {
                        // then remove a specific one
                        Collection<Cookie> cookieDetails = fSetCookieList_->cookieDetails ();
                        auto               removeMe      = Cookie::Decode (*value);
                        auto r = cookieDetails.Remove (removeMe);
                        if (nRemoveals != nullptr) {
                            *nRemoveals = r ? 1 : 0;
                        }
                    }
                    else {
                        fSetCookieList_ = nullopt; // then remove all
                    }
                }
            } break;
            case AddOrSet::eAdd: {
                Collection<Cookie> cookieDetails = fSetCookieList_ ? fSetCookieList_->cookieDetails () : Collection<Cookie>{};
                cookieDetails += Cookie::Decode (*value);
                fSetCookieList_ = cookieDetails;
            } break;
            case AddOrSet::eSet: {
                fSetCookieList_ = value ? CookieList::Decode (*value) : optional<CookieList>{};
            } break;
        }
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kVary)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fVary_ != nullopt) ? 1 : 0;
        }
        fVary_ = value ? Containers::Set<String>{value->Tokenize ({','})} : optional<Containers::Set<String>>{};
        return true;
    }
    return false;
}

void Headers::SetExtras_ (const String& headerName, const optional<String>& value)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    fExtraHeaders_.Remove ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName); });
    if (value) {
        fExtraHeaders_.Add (KeyValuePair<String, String>{headerName, *value});
    }
}

void Headers::Set (const String& headerName, const optional<String>& value)
{
    if (UpdateBuiltin_ (AddOrSet::eSet, headerName, value)) {
        return;
    }
    SetExtras_ (headerName, value);
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
    if (fCookieList_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kCookie, fCookieList_->EncodeForCookieHeader ()});
    }
    if (fETag_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kETag, fETag_->As<String> ()});
    }
    if (fIfNoneMatch_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kIfNoneMatch, fIfNoneMatch_->As<String> ()});
    }
    if (fSetCookieList_) {
        // fSetCookieList_ produces multiple set-headers
        for (auto i : fSetCookieList_->cookieDetails ()) {
            results.Add (KeyValuePair<String, String>{HeaderName::kSetCookie, i.Encode ()});
        }
    }
    if (fVary_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kVary, String::Join (*fVary_)});
    }
    return results;
}

String Headers::ToString () const
{
    return Characters::ToString (As<Mapping<String, String>> ());
}
