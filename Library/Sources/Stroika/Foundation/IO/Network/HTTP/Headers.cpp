/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Headers.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

using Debug::AssertExternallySynchronizedMutex;

namespace {
    constexpr string_view kKeepAlive_ = "Keep-Alive"sv;
    constexpr string_view kClose_     = "close"sv;
}

/*
 ********************************************************************************
 ******************************** HTTP::Headers *********************************
 ********************************************************************************
 */
Headers::Headers ()
    : acceptEncoding{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<ContentEncodings> {
                         const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::acceptEncoding);
                         AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                         return thisObj->fAcceptEncodings_;
                     },
                     [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newAcceptEncodings) {
                         Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::acceptEncoding);
                         AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                         thisObj->fAcceptEncodings_ = newAcceptEncodings;
                     }}
    , accessControlAllowOrigin{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<String> {
              const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::accessControlAllowOrigin);
              AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
              return thisObj->LookupOne (HeaderName::kAccessControlAllowOrigin);
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& accessControlAllowOrigin) {
              Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::accessControlAllowOrigin);
              AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
              thisObj->SetExtras_ (HeaderName::kAccessControlAllowOrigin, accessControlAllowOrigin);
          }}
    , allow{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<Containers::Set<String>> {
                const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::allow);
                AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                if (auto hdr = thisObj->LookupOne (HeaderName::kAllow)) {
                    return Containers::Set<String>{hdr->Tokenize ({','})};
                }
                return nullopt;
            },
            [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& allowed) {
                Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::allow);
                AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                thisObj->SetExtras_ (HeaderName::kAllow, allowed ? String::Join (*allowed) : optional<String>{});
            }}
    , cacheControl{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<CacheControl> {
                       const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cacheControl);
                       AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                       return thisObj->fCacheControl_;
                   },
                   [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& cacheControl) {
                       Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cacheControl);
                       AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                       thisObj->fCacheControl_ = cacheControl;
                   }}
    , connection{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<ConnectionValue> {
                     const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::connection);
                     AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                     if (auto connectionHdr = thisObj->LookupOne (HeaderName::kConnection)) {
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
                     Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::connection);
                     AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                     optional<String>                                v;
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
    , contentEncoding{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<ContentEncodings> {
                          const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentEncoding);
                          AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                          return thisObj->fContentEncoding_;
                      },
                      [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto newContentEncoding) {
                          Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentEncoding);
                          AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                          thisObj->fContentEncoding_ = newContentEncoding;
                      }}
    , contentLength{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<uint64_t> {
                        const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength);
                        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                        return thisObj->fContentLength_;
                    },
                    [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto contentLength) {
                        Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength);
                        AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                        thisObj->fContentLength_ = contentLength;
                    }}
    , contentType{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentType);
                      AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                      return thisObj->fContentType_;
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& contentType) {
                      Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentType);
                      AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                      thisObj->fContentType_ = contentType;
                  }}
    , date{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
               const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::date);
               AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               return thisObj->fDate_;
           },
           [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& d) {
               Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::date);
               AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               thisObj->fDate_ = d ? d->AsUTC () : d;
           }}
    , cookie{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                 const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cookie);
                 AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                 return Memory::NullCoalesce (thisObj->fCookieList_);
             },
             [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& cookies) {
                 Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::cookie);
                 AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                 thisObj->fCookieList_ = cookies.cookieDetails ().empty () ? optional<CookieList>{} : cookies;
             }}
    , ETag{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
               const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ETag);
               AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               return thisObj->fETag_;
           },
           [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& etag) {
               Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ETag);
               AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               thisObj->fETag_ = etag;
           }}
    , host{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
               const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::host);
               AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               return thisObj->fHost_;
           },
           [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& host) {
               Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::host);
               AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               thisObj->fHost_ = host;
           }}
    , ifNoneMatch{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ifNoneMatch);
                      AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                      return thisObj->fIfNoneMatch_;
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& ifNoneMatch) {
                      Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::ifNoneMatch);
                      AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                      thisObj->fIfNoneMatch_ = ifNoneMatch;
                  }}
    , keepAlive{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<HTTP::KeepAlive> {
                    const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::keepAlive);
                    AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                    if (auto hdrValue = thisObj->LookupOne (HeaderName::kKeepAlive)) {
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
    , location{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<URI> {
                   const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::location);
                   AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                   if (auto hdr = thisObj->LookupOne (HeaderName::kLocation)) {
                       return URI::Parse (*hdr);
                   }
                   return nullopt;
               },
               [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<URI>& location) {
                   Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::location);
                   thisObj->SetExtras_ (HeaderName::kLocation, location ? location->As<String> () : optional<String>{});
               }}
    , origin{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<URI> {
                 const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::origin);
                 AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                 if (auto hdr = thisObj->LookupOne (HeaderName::kOrigin)) {
                     return URI::Parse (*hdr);
                 }
                 return nullopt;
             },
             [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<URI>& origin) {
                 Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::origin);
                 thisObj->SetExtras_ (HeaderName::kOrigin, origin ? origin->As<String> () : optional<String>{});
             }}
    , server{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<String> {
                 const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::server);
                 AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                 return thisObj->LookupOne (HeaderName::kServer);
             },
             [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<String>& server) {
                 Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::server);
                 AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                 thisObj->SetExtras_ (HeaderName::kServer, server);
             }}
    , setCookie{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                    const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::setCookie);
                    AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                    return Memory::NullCoalesce (thisObj->fSetCookieList_);
                },
                [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& cookies) {
                    Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::setCookie);
                    AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                    thisObj->fSetCookieList_ = cookies.cookieDetails ().empty () ? optional<CookieList>{} : cookies;
                }}
    , transferEncoding{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                           const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::transferEncoding);
                           AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                           return thisObj->fTransferEncoding_;
                       },
                       [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newTransferEncodings) {
                           Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::transferEncoding);
                           AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
                           if (newTransferEncodings && newTransferEncodings->length () == 1 &&
                               newTransferEncodings->Contains (TransferEncoding::kIdentity)) {
                               thisObj->fTransferEncoding_ = nullopt;
                           }
                           else {
                               thisObj->fTransferEncoding_ = newTransferEncodings;
                           }
                       }}
    , vary{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<Containers::Set<String>> {
               const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::vary);
               AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               return thisObj->fVary_;
           },
           [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<Containers::Set<String>>& vary) {
               Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::vary);
               AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
               thisObj->fVary_ = vary;
           }}
{
}

Headers::Headers (const Headers& src)
    : Headers{}
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{src.fThisAssertExternallySynchronized_};
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficient way to avoid duplicative code
    fExtraHeaders_     = src.fExtraHeaders_;
    fAcceptEncodings_  = src.fAcceptEncodings_;
    fCacheControl_     = src.fCacheControl_;
    fContentEncoding_  = src.fContentEncoding_;
    fContentType_      = src.fContentType_;
    fCookieList_       = src.fCookieList_;
    fDate_             = src.fDate_;
    fHost_             = src.fHost_;
    fIfNoneMatch_      = src.fIfNoneMatch_;
    fSetCookieList_    = src.fSetCookieList_;
    fTransferEncoding_ = src.fTransferEncoding_;
    fVary_             = src.fVary_;
    // for extended properties, we must call the property function to get the callbacks called
    fContentLength_ = src.contentLength ();
    fETag_          = src.ETag ();
}

Headers::Headers (Headers&& src)
    : Headers{}
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{src.fThisAssertExternallySynchronized_};
    // NOTE properties and fields refer to the same thing. COULD copy properties, but cheaper to just 'initialize' the fields
    // However, cannot mix initialize with calling delegated CTOR, so do the slightly more inefficent way to avoid duplicative code
    fExtraHeaders_     = move (src.fExtraHeaders_);
    fAcceptEncodings_  = move (src.fAcceptEncodings_);
    fCacheControl_     = move (src.fCacheControl_);
    fContentEncoding_  = move (src.fContentEncoding_);
    fContentType_      = move (src.fContentType_);
    fCookieList_       = move (src.fCookieList_);
    fDate_             = move (src.fDate_);
    fHost_             = move (src.fHost_);
    fIfNoneMatch_      = move (src.fIfNoneMatch_);
    fSetCookieList_    = move (src.fSetCookieList_);
    fTransferEncoding_ = move (src.fTransferEncoding_);
    fVary_             = move (src.fVary_);
    // for extended properties, we must call the property function to get the callbacks called
    fContentLength_ = src.contentLength ();
    fETag_          = src.ETag ();
}

Headers::Headers (const Iterable<pair<String, String>>& src)
    : Headers{}
{
    for (const auto& kv : src) {
        Set (kv.first, kv.second);
    }
}

Headers::Headers (const Iterable<KeyValuePair<String, String>>& src)
    : Headers{}
{
    for (const auto& kv : src) {
        Set (kv.fKey, kv.fValue);
    }
}

Headers& Headers::operator= (const Headers& rhs)
{
    AssertExternallySynchronizedMutex::ReadContext  critSec1{rhs.fThisAssertExternallySynchronized_};
    AssertExternallySynchronizedMutex::WriteContext critSec2{fThisAssertExternallySynchronized_};
    if (this != &rhs) {
        fExtraHeaders_     = rhs.fExtraHeaders_;
        fAcceptEncodings_  = rhs.fAcceptEncodings_;
        fCacheControl_     = rhs.fCacheControl_;
        fContentLength_    = rhs.fContentLength_;
        fContentEncoding_  = rhs.fContentEncoding_;
        fContentType_      = rhs.fContentType_;
        fCookieList_       = rhs.fCookieList_;
        fDate_             = rhs.fDate_;
        fETag_             = rhs.fETag_;
        fHost_             = rhs.fHost_;
        fIfNoneMatch_      = rhs.fIfNoneMatch_;
        fSetCookieList_    = rhs.fSetCookieList_;
        fTransferEncoding_ = rhs.fTransferEncoding_;
        fVary_             = rhs.fVary_;
    }
    return *this;
}

Headers& Headers::operator= (Headers&& rhs) noexcept
{
    AssertExternallySynchronizedMutex::WriteContext critSec1{rhs.fThisAssertExternallySynchronized_};
    AssertExternallySynchronizedMutex::WriteContext critSec2{fThisAssertExternallySynchronized_};
    if (this != &rhs) {
        fExtraHeaders_     = move (rhs.fExtraHeaders_);
        fAcceptEncodings_  = move (rhs.fAcceptEncodings_);
        fCacheControl_     = move (rhs.fCacheControl_);
        fContentLength_    = move (rhs.fContentLength_);
        fContentEncoding_  = move (rhs.fContentEncoding_);
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

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
void Headers::SetAssertExternallySynchronizedMutexContext (const shared_ptr<AssertExternallySynchronizedMutex::SharedContext>& sharedContext)
{
    fThisAssertExternallySynchronized_.SetAssertExternallySynchronizedMutexContext (sharedContext);
}
#endif

optional<String> Headers::LookupOne (const String& name) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    if (kHeaderNameEqualsComparer (name, HeaderName::kAcceptEncoding)) {
        return fAcceptEncodings_ ? fAcceptEncodings_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kCacheControl)) {
        return fCacheControl_ ? fCacheControl_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentLength)) {
        auto cl = this->contentLength ();
        return cl ? Characters::Format ("{}"_f, *cl) : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentEncoding)) {
        return fContentEncoding_ ? fContentEncoding_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kContentType)) {
        return fContentType_ ? fContentType_->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kCookie)) {
        return fCookieList_ ? fCookieList_->EncodeForCookieHeader () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kDate)) {
        return fDate_ ? fDate_->Format (Time::DateTime::kRFC1123Format) : optional<String>{};
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
                return i->As<String> ();
            }
        }
        return optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kTransferEncoding)) {
        auto tc = this->transferEncoding ();
        return tc ? tc->As<String> () : optional<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kVary)) {
        return fVary_ ? String::Join (*fVary_) : optional<String>{};
    }
    else {
        // should switch to new non-existent class Association here - and use that...more efficient -
        if (auto ri = fExtraHeaders_.Find ([&] (const auto& i) { return kHeaderNameEqualsComparer (name, i.fKey); })) {
            return ri->fValue;
        }
        return nullopt;
    }
}

Collection<String> Headers::LookupAll (const String& name) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    if (kHeaderNameEqualsComparer (name, HeaderName::kCacheControl) or kHeaderNameEqualsComparer (name, HeaderName::kContentLength) or
        kHeaderNameEqualsComparer (name, HeaderName::kContentType) or kHeaderNameEqualsComparer (name, HeaderName::kCookie) or
        kHeaderNameEqualsComparer (name, HeaderName::kETag) or kHeaderNameEqualsComparer (name, HeaderName::kHost) or
        kHeaderNameEqualsComparer (name, HeaderName::kIfNoneMatch) or kHeaderNameEqualsComparer (name, HeaderName::kTransferEncoding) or
        kHeaderNameEqualsComparer (name, HeaderName::kVary)) {
        auto o = this->LookupOne (name);
        return o ? Collection<String>{*o} : Collection<String>{};
    }
    else if (kHeaderNameEqualsComparer (name, HeaderName::kSetCookie)) {
        // return each 'set' cookie as a separate header value
        if (fSetCookieList_.has_value ()) {
            return fSetCookieList_->cookieDetails ().Map<Iterable<String>> ([] (const Cookie& i) { return i.As<String> (); });
        }
        return Collection<String>{};
    }
    else {
        Collection<String> result;
        // should switch to new non-existent class Association here - and use that...more efficient -
        if (auto ri = fExtraHeaders_.Find ([&] (const auto& i) { return kHeaderNameEqualsComparer (name, i.fKey); })) {
            result += ri->fValue;
        }
        return result;
    }
}

size_t Headers::Remove (const String& headerName)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    size_t                                          nRemovals{};
    if (UpdateBuiltin_ (AddOrSet::eRemove, headerName, nullopt, &nRemovals)) {
        return nRemovals; // could be zero if its builtin, but this doesn't change anything
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    return fExtraHeaders_.RemoveAll ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName); });
}

size_t Headers::Remove (const String& headerName, const String& value)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    size_t                                          nRemovals{};
    if (UpdateBuiltin_ (AddOrSet::eRemove, headerName, value, &nRemovals)) {
        return nRemovals;
    }
    // currently removes all, but later add variant that removes one/all (in fact rename this)
    return fExtraHeaders_.RemoveAll ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName) and value == i.fValue; });
}

void Headers::Add (const String& headerName, const String& value)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    if (UpdateBuiltin_ (AddOrSet::eAdd, headerName, value)) {
        return;
    }
    fExtraHeaders_.Add ({headerName, value});
}

void Headers::AddAll (const Headers& headers)
{
    // more efficient to add each fields than converting arg to collection and then applying each mappnig back
    fExtraHeaders_ += headers.fExtraHeaders_;
    if (headers.fAcceptEncodings_) {
        fAcceptEncodings_ = *headers.fAcceptEncodings_;
    }
    if (headers.fCacheControl_) {
        fCacheControl_ = *headers.fCacheControl_;
    }
    if (auto cl = headers.contentLength ()) {
        this->contentLength = *cl;
    }
    if (headers.fContentEncoding_) {
        fContentEncoding_ = *headers.fContentEncoding_;
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

bool Headers::UpdateBuiltin_ (AddOrSet flag, const String& headerName, const optional<String>& value, size_t* nRemovals)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    if constexpr (qDebug) {
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
    }
    if (kHeaderNameEqualsComparer (headerName, HeaderName::kAcceptEncoding)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fAcceptEncodings_ != nullopt) ? 1 : 0;
        }
        fAcceptEncodings_ = value ? ContentEncodings::Parse (*value) : optional<ContentEncodings>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kCacheControl)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fCacheControl_ != nullopt) ? 1 : 0;
        }
        fCacheControl_ = value ? CacheControl::Parse (*value) : optional<CacheControl>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kContentEncoding)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fContentEncoding_ != nullopt) ? 1 : 0;
        }
        this->contentEncoding = value ? ContentEncoding{*value} : optional<ContentEncoding>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kContentLength)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fContentLength_ != nullopt) ? 1 : 0;
        }
        this->contentLength = value ? String2Int<uint64_t> (*value) : optional<uint64_t>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kContentType)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fContentType_ != nullopt) ? 1 : 0;
        }
        fContentType_ = value ? InternetMediaType{*value} : optional<InternetMediaType>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kCookie)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fCookieList_ != nullopt) ? 1 : 0;
        }
        fCookieList_ = value ? CookieList::Parse (*value) : optional<CookieList>{};
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kDate)) {
        optional<Time::DateTime> useDT;
        // see http://stroika-bugs.sophists.com/browse/STK-731 - should support parsing (not writing) older formats too
        try {
            if (value) {
                useDT = Time::DateTime::Parse (*value, Time::DateTime::kRFC1123Format).AsUTC ();
            }
        }
        catch (...) {
            DbgTrace ("Treating ill-formatted date as missing date header"_f);
        }
        if (nRemovals != nullptr) {
            *nRemovals = (useDT == nullopt and fDate_ != nullopt) ? 1 : 0;
        }
        fDate_ = useDT ? useDT : optional<Time::DateTime>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kETag)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fETag_ != nullopt) ? 1 : 0;
        }
        fETag_ = value ? ETag::Parse (*value) : optional<HTTP::ETag>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kHost)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fHost_ != nullopt) ? 1 : 0;
        }
        fHost_ = value;
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kIfNoneMatch)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fIfNoneMatch_ != nullopt) ? 1 : 0;
        }
        fIfNoneMatch_ = value ? IfNoneMatch::Parse (*value) : optional<IfNoneMatch>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kSetCookie)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fSetCookieList_ != nullopt) ? 1 : 0;
        }
        switch (flag) {
            case AddOrSet::eRemove: {
                if (fSetCookieList_ != nullopt) {
                    if (value) {
                        // then remove a specific one
                        Collection<Cookie> cookieDetails = fSetCookieList_->cookieDetails ();
                        auto               removeMe      = Cookie::Parse (*value);
                        bool               r             = cookieDetails.RemoveIf (removeMe);
                        if (nRemovals != nullptr) {
                            *nRemovals = r ? 1 : 0;
                        }
                    }
                    else {
                        fSetCookieList_ = nullopt; // then remove all
                    }
                }
            } break;
            case AddOrSet::eAdd: {
                Collection<Cookie> cookieDetails = fSetCookieList_ ? fSetCookieList_->cookieDetails () : Collection<Cookie>{};
                cookieDetails += Cookie::Parse (*value);
                fSetCookieList_ = cookieDetails;
            } break;
            case AddOrSet::eSet: {
                fSetCookieList_ = value ? CookieList::Parse (*value) : optional<CookieList>{};
            } break;
        }
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kTransferEncoding)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fTransferEncoding_ != nullopt) ? 1 : 0;
        }
        this->transferEncoding = value ? TransferEncodings::Parse (*value) : optional<TransferEncodings>{};
        return true;
    }
    else if (kHeaderNameEqualsComparer (headerName, HeaderName::kVary)) {
        if (nRemovals != nullptr) {
            *nRemovals = (value == nullopt and fVary_ != nullopt) ? 1 : 0;
        }
        fVary_ = value ? Containers::Set<String>{value->Tokenize ({','})} : optional<Containers::Set<String>>{};
        return true;
    }
    return false;
}

void Headers::SetExtras_ (const String& headerName, const optional<String>& value)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    fExtraHeaders_.RemoveIf ([=] (const auto& i) { return kHeaderNameEqualsComparer (i.fKey, headerName); });
    if (value) {
        fExtraHeaders_.Add ({headerName, *value});
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
Association<String, String> Headers::As () const
{
    return As<Collection<KeyValuePair<String, String>>> ().As<Association<String, String>> (kHeaderNameEqualsComparer);
}

template <>
Mapping<String, String> Headers::As () const
{
    // NOTE - CAN be lossy conversion to Mapping, losing Set-Cookie's
    return As<Collection<KeyValuePair<String, String>>> ().As<Mapping<String, String>> (kHeaderNameEqualsComparer);
}

template <>
Collection<KeyValuePair<String, String>> Headers::As () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    Collection<KeyValuePair<String, String>>       results = fExtraHeaders_;
    if (fAcceptEncodings_) {
        results.Add ({HeaderName::kAcceptEncoding, fAcceptEncodings_->As<String> ()});
    }
    if (fCacheControl_) {
        results.Add ({HeaderName::kCacheControl, fCacheControl_->As<String> ()});
    }
    if (auto cl = this->contentLength ()) {
        results.Add ({HeaderName::kContentLength, Characters::Format ("{}"_f, static_cast<long long> (*cl))});
    }
    if (fContentEncoding_) {
        results.Add ({HeaderName::kContentEncoding, fContentEncoding_->As<String> ()});
    }
    if (fContentType_) {
        results.Add ({HeaderName::kContentType, fContentType_->As<String> ()});
    }
    if (fCookieList_) {
        results.Add ({HeaderName::kCookie, fCookieList_->EncodeForCookieHeader ()});
    }
    if (fDate_) {
        results.Add ({HeaderName::kDate, fDate_->Format (Time::DateTime::kRFC1123Format)});
    }
    if (auto et = ETag ()) {
        results.Add ({HeaderName::kETag, et->As<String> ()});
    }
    if (fHost_) {
        results.Add ({HeaderName::kHost, *fHost_});
    }
    if (fIfNoneMatch_) {
        results.Add ({HeaderName::kIfNoneMatch, fIfNoneMatch_->As<String> ()});
    }
    if (fSetCookieList_) {
        // fSetCookieList_ produces multiple set-headers
        for (const auto& i : fSetCookieList_->cookieDetails ()) {
            results.Add ({HeaderName::kSetCookie, i.As<String> ()});
        }
    }
    if (auto tc = this->transferEncoding ()) {
        results.Add ({HeaderName::kTransferEncoding, tc->As<String> ()});
    }
    if (fVary_) {
        results.Add ({HeaderName::kVary, String::Join (*fVary_)});
    }
    return results;
}

String Headers::ToString () const
{
    return Characters::ToString (As<Association<String, String>> ());
}
