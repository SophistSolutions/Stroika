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
    , date{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::date);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fDate_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& d) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::date);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fDate_ = d ? d->AsUTC () : d;
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
    , transferEncoding{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Headers*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::transferEncoding);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fTransferEncoding_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newTransferEncodings) {
              Headers*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::transferEncoding);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              if (newTransferEncodings && newTransferEncodings->length () == 1 && newTransferEncodings->Contains (TransferEncoding::eIdentity)) {
                  thisObj->fTransferEncoding_ = nullopt;
              }
              else {
                  thisObj->fTransferEncoding_ = newTransferEncodings;
              }
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

Headers::Headers (const Headers& src, CopyFlags flags)
    : Headers{}
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{src};
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_     = src.fExtraHeaders_;
    fCacheControl_     = src.fCacheControl_;
    fContentType_      = src.fContentType_;
    fCookieList_       = src.fCookieList_;
    fDate_             = src.fDate_;
    fHost_             = src.fHost_;
    fIfNoneMatch_      = src.fIfNoneMatch_;
    fSetCookieList_    = src.fSetCookieList_;
    fTransferEncoding_ = src.fTransferEncoding_;
    fVary_             = src.fVary_;
    if (flags == CopyFlags::eOnlyBaseValue) {
        fContentLength_ = src.fContentLength_;
        fETag_          = src.fETag_;
    }
    else {
        fContentLength_ = src.contentLength ();
        fETag_          = src.ETag ();
    }
}

Headers::Headers (Headers&& src, CopyFlags flags)
    : Headers{}
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{src};
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_     = move (src.fExtraHeaders_);
    fCacheControl_     = move (src.fCacheControl_);
    fContentType_      = move (src.fContentType_);
    fCookieList_       = move (src.fCookieList_);
    fDate_             = move (src.fDate_);
    fHost_             = move (src.fHost_);
    fIfNoneMatch_      = move (src.fIfNoneMatch_);
    fSetCookieList_    = move (src.fSetCookieList_);
    fTransferEncoding_ = move (src.fTransferEncoding_);
    fVary_             = move (src.fVary_);
    if (flags == CopyFlags::eOnlyBaseValue) {
        fContentLength_ = src.fContentLength_;
        fETag_          = src.fETag_;
    }
    else {
        fContentLength_ = src.contentLength ();
        fETag_          = src.ETag ();
    }
}

Headers::Headers (const Iterable<KeyValuePair<String, String>>& src)
    : Headers{}
{
    for (auto kv : src) {
        Set (kv.fKey, kv.fValue);
    }
}

Headers& Headers::operator= (const Headers& rhs)
{
    shared_lock<const AssertExternallySynchronizedLock> critSec1{rhs};
    lock_guard<const AssertExternallySynchronizedLock>  critSec2{*this};
    if (this != &rhs) {
        fExtraHeaders_     = move (rhs.fExtraHeaders_);
        fCacheControl_     = move (rhs.fCacheControl_);
        fContentLength_    = rhs.fContentLength_;
        fContentType_      = move (rhs.fContentType_);
        fCookieList_       = move (rhs.fCookieList_);
        fDate_             = move (rhs.fDate_);
        fETag_             = rhs.fETag_;
        fHost_             = move (rhs.fHost_);
        fIfNoneMatch_      = move (rhs.fIfNoneMatch_);
        fSetCookieList_    = move (rhs.fSetCookieList_);
        fTransferEncoding_ = move (rhs.fTransferEncoding_);
        fVary_             = move (rhs.fVary_);
    }
    return *this;
}

Headers& Headers::operator= (Headers&& rhs)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec1{rhs};
    lock_guard<const AssertExternallySynchronizedLock> critSec2{*this};
    if (this != &rhs) {
        fExtraHeaders_     = move (rhs.fExtraHeaders_);
        fCacheControl_     = move (rhs.fCacheControl_);
        fContentLength_    = move (rhs.fContentLength_);
        fContentType_      = move (rhs.fContentType_);
        fCookieList_       = move (rhs.fCookieList_);
        fDate_             = move (rhs.fDate_);
        fETag_             = move (rhs.fETag_);
        fHost_             = move (rhs.fHost_);
        fIfNoneMatch_      = move (rhs.fIfNoneMatch_);
        fSetCookieList_    = move (rhs.fSetCookieList_);
        fTransferEncoding_ = move (rhs.fTransferEncoding_);
        fVary_             = move (rhs.fVary_);
    }
    return *this;
}

optional<String> Headers::LookupOne (const String& name) const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (kHeaderNameEqualsComparer (name, HeaderName::kCacheControl)) {
        return fCacheControl_ ? fCacheControl_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentLength)) {
        auto cl = this->contentLength ();
        return cl ? Characters::Format (L"%ld", *cl) : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentType)) {
        return fContentType_ ? fContentType_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kCookie)) {
        return fCookieList_ ? fCookieList_->EncodeForCookieHeader () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kDate)) {
        return fDate_ ? fDate_->Format (Time::DateTime::PrintFormat::eRFC1123) : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kETag)) {
        auto e = this->ETag ();
        return e ? e->As<String> () : optional<String>{};
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
    else if (kHeaderNameEqualsComparer (name, HeaderName::kTransferEncoding)) {
        auto tc = this->transferEncoding ();
        return tc ? tc->Encode () : optional<String>{};
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
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (kHeaderNameEqualsComparer (name, HeaderName::kCacheControl) or kHeaderNameEqualsComparer (name, HeaderName::kContentLength) or kHeaderNameEqualsComparer (name, HeaderName::kContentType) or kHeaderNameEqualsComparer (name, HeaderName::kCookie) or kHeaderNameEqualsComparer (name, HeaderName::kETag) or kHeaderNameEqualsComparer (name, HeaderName::kHost) or kHeaderNameEqualsComparer (name, HeaderName::kIfNoneMatch) or kHeaderNameEqualsComparer (name, HeaderName::kTransferEncoding) or kHeaderNameEqualsComparer (name, HeaderName::kVary)) {
        auto o = this->LookupOne (name);
        return o ? Collection<String>{*o} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kSetCookie)) {
        // return each 'set' cookie as a separate header value
        if (fSetCookieList_.has_value ()) {
            return fSetCookieList_->cookieDetails ().Select<String> ([] (const auto& i) { return i.Encode (); });
        }
        return Collection<String>{};
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
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    size_t                                             nRemoveals{};
    if (UpdateBuiltin_ (AddOrSet::eRemove, headerName, nullopt, &nRemoveals)) {
        return nRemoveals; // could be zero if its builtin, but this doesn't change anything
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    return fExtraHeaders_.RemoveAll ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName); });
}

size_t Headers::Remove (const String& headerName, const String& value)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    size_t                                             nRemoveals{};
    if (UpdateBuiltin_ (AddOrSet::eRemove, headerName, value, &nRemoveals)) {
        return nRemoveals;
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    return fExtraHeaders_.RemoveAll ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName) and value == i.fValue; });
}

void Headers::Add (const String& headerName, const String& value)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (UpdateBuiltin_ (AddOrSet::eAdd, headerName, value)) {
        return;
    }
    fExtraHeaders_.Add (KeyValuePair<String, String>{headerName, value});
}

void Headers::AddAll (const Headers& headers)
{
    // more efficient to add each fields than converting arg to collection and then applying each mappnig back
    fExtraHeaders_ += headers.fExtraHeaders_;
    if (headers.fCacheControl_) {
        fCacheControl_ = *headers.fCacheControl_;
    }
    if (auto cl = headers.contentLength ()) {
        this->contentLength = *cl;
    }
    if (headers.fContentType_) {
        fContentType_ = *headers.fContentType_;
    }
    if (headers.fCookieList_) {
        fCookieList_ = *headers.fCookieList_;
    }
    if (headers.fDate_) {
        fDate_ = *headers.fDate_;
    }
    if (auto et = headers.ETag ()) {
        fETag_ = *et;
    }
    if (headers.fHost_) {
        fHost_ = *headers.fHost_;
    }
    if (headers.fIfNoneMatch_) {
        fIfNoneMatch_ = *headers.fIfNoneMatch_;
    }
    if (headers.fSetCookieList_) {
        fSetCookieList_ = *headers.fSetCookieList_;
    }
    if (auto tc = headers.transferEncoding ()) {
        fTransferEncoding_ = *tc;
    }
    if (headers.fVary_) {
        fVary_ = *headers.fVary_;
    }
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
        this->contentLength = value ? String2Int<uint64_t> (*value) : optional<uint64_t>{};
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
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kDate)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fDate_ != nullopt) ? 1 : 0;
        }
        fDate_ = value ? Time::DateTime::Parse (*value, Time::DateTime::ParseFormat::eRFC1123).AsUTC () : optional<Time::DateTime>{};
        return true;
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
                        auto               r             = cookieDetails.Remove (removeMe);
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
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kTransferEncoding)) {
        if (nRemoveals != nullptr) {
            *nRemoveals = (value == nullopt and fTransferEncoding_ != nullopt) ? 1 : 0;
        }
        this->transferEncoding = value ? TransferEncodings::Decode (*value) : optional<TransferEncodings>{};
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
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    Collection<KeyValuePair<String, String>>            results = fExtraHeaders_;
    if (fCacheControl_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kCacheControl, fCacheControl_->As<String> ()});
    }
    if (auto cl = this->contentLength ()) {
        results.Add (KeyValuePair<String, String>{HeaderName::kContentLength, Characters::Format (L"%lld", static_cast<long long> (*cl))});
    }
    if (fContentType_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kContentType, fContentType_->As<String> ()});
    }
    if (fCookieList_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kCookie, fCookieList_->EncodeForCookieHeader ()});
    }
    if (fDate_) {
        results.Add (KeyValuePair<String, String>{HeaderName::kDate, fDate_->Format (Time::DateTime::PrintFormat::eRFC1123)});
    }
    if (auto et = ETag ()) {
        results.Add (KeyValuePair<String, String>{HeaderName::kETag, et->As<String> ()});
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
    if (auto tc = this->transferEncoding ()) {
        results.Add (KeyValuePair<String, String>{HeaderName::kTransferEncoding, tc->Encode ()});
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
