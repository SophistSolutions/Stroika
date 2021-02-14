/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String2Int.h"
#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Versions.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"
#include "../../Foundation/Streams/InputSubStream.h"
#include "../../Foundation/Streams/MemoryStream.h"

#include "Request.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *************************** WebServer::Request *********************************
 ********************************************************************************
 */
Request::Request (Request&& src)
    : Request{src.fInputStream_}
{
    fHTTPVersion_     = src.fHTTPVersion_;
    fMethod_          = src.fMethod_;
    fURL_             = src.fURL_;
    fHeaders_         = src.fHeaders_;
    fBodyInputStream_ = src.fBodyInputStream_;
    fBody_            = src.fBody_;
}

Request::Request (const Streams::InputStream<byte>::Ptr& inStream)
    : httpVersion{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> String {
              const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpVersion);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fHTTPVersion_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& versionOrVersionLabel) {
              Request*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpVersion);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              static const String                                kLabeled_10_{L"HTTP/1.0"sv};
              static const String                                kLabeled_11_{L"HTTP/1.1"sv};
              auto                                               versionStringComparer = String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive};
              if (versionOrVersionLabel == kLabeled_11_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointOne or versionStringComparer (versionOrVersionLabel, kLabeled_11_)) {
                  thisObj->fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointOne;
              }
              else if (versionOrVersionLabel == kLabeled_10_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointZero or versionStringComparer (versionOrVersionLabel, kLabeled_10_)) {
                  thisObj->fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointZero;
              }
              else if (versionOrVersionLabel.StartsWith (L"HTTP/", Characters::CompareOptions::eCaseInsensitive)) {
                  thisObj->fHTTPVersion_ = versionOrVersionLabel.SubString (5);
              }
              else {
                  thisObj->fHTTPVersion_ = versionOrVersionLabel;
              }
          }}
    , httpMethod{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpMethod);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fMethod_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& method) {
              Request*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpMethod);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fMethod_ = method;
          }}
    , url{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::url);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fURL_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& url) {
              Request*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::url);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fURL_ = url;
          }}
    , headers{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const IO::Network::HTTP::Headers& {
              const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::headers);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fHeaders_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newHeaders) {
              Request*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::headers);
              lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
              thisObj->fHeaders_ = newHeaders;
          }}
    , contentType{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::contentType);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fHeaders_.contentType ();
          }}
    , keepAliveRequested{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::keepAliveRequested);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        using ConnectionValue = IO::Network::HTTP::Headers::ConnectionValue;
        if (thisObj->fHTTPVersion_ == IO::Network::HTTP::Versions::kOnePointZero) {
            return thisObj->fHeaders_.connection ().value_or (ConnectionValue::eClose) == ConnectionValue::eKeepAlive;
        }
        if (thisObj->fHTTPVersion_ == IO::Network::HTTP::Versions::kOnePointOne) {
            return thisObj->fHeaders_.connection ().value_or (ConnectionValue::eKeepAlive) == ConnectionValue::eKeepAlive;
        }
        return true; // for HTTP 2.0 and later, keep alive is always assumed (double check/reference?)
    }}
    , fInputStream_{inStream}
{
}

#if qDebug
void Request::SetAssertExternallySynchronizedLockContext (shared_ptr<SharedContext>& sharedContext)
{
    AssertExternallySynchronizedLock::SetAssertExternallySynchronizedLockContext (sharedContext);
    fHeaders_.SetAssertExternallySynchronizedLockContext (sharedContext);
}
#endif

void Request::SetHTTPVersion (const String& versionOrVersionLabel)
{
    //***DEPRECATED***
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    static const String                                kLabeled_10_{L"HTTP/1.0"sv};
    static const String                                kLabeled_11_{L"HTTP/1.1"sv};
    auto                                               versionStringComparer = String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive};
    if (versionOrVersionLabel == kLabeled_11_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointOne or versionStringComparer (versionOrVersionLabel, kLabeled_11_)) {
        fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointOne;
    }
    else if (versionOrVersionLabel == kLabeled_10_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointZero or versionStringComparer (versionOrVersionLabel, kLabeled_10_)) {
        fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointZero;
    }
    else if (versionOrVersionLabel.StartsWith (L"HTTP/", Characters::CompareOptions::eCaseInsensitive)) {
        fHTTPVersion_ = versionOrVersionLabel.SubString (5);
    }
    else {
        fHTTPVersion_ = versionOrVersionLabel;
    }
}

bool Request::GetKeepAliveRequested () const
{
    //***DEPRECATED***
    using ConnectionValue = IO::Network::HTTP::Headers::ConnectionValue;
    if (fHTTPVersion_ == IO::Network::HTTP::Versions::kOnePointZero) {
        return fHeaders_.connection ().value_or (ConnectionValue::eClose) == ConnectionValue::eKeepAlive;
    }
    if (fHTTPVersion_ == IO::Network::HTTP::Versions::kOnePointOne) {
        return fHeaders_.connection ().value_or (ConnectionValue::eKeepAlive) == ConnectionValue::eKeepAlive;
    }
    return true; // for HTTP 2.0 and later, keep alive is always assumed (double check/reference?)
}

Memory::BLOB Request::GetBody ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"Request::GetBody"};
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (not fBody_.has_value ()) {
        fBody_ = GetBodyStream ().ReadAll ();
    }
    return *fBody_;
}

optional<uint64_t> Request::GetContentLength () const
{
    //***DEPRECATED***
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return fHeaders_.contentLength;
}

optional<InternetMediaType> Request::GetContentType () const
{
    //***DEPRECATED***
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return fHeaders_.contentType;
}

Streams::InputStream<byte>::Ptr Request::GetBodyStream ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"Request::GetBodyStream"};
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (fBodyInputStream_ == nullptr) {
        /*
         *  According to https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html:
         *      The presence of a message-body in a request is signaled by the inclusion of
         *      a Content-Length or Transfer-Encoding header field in the request's message-headers
         */
        // if we have a content-length, read that many bytes.
        if (optional<uint64_t> cl = fHeaders_.contentLength ()) {
            fBodyInputStream_ = InputSubStream<byte>::New (fInputStream_, {}, fInputStream_.GetOffset () + static_cast<size_t> (*cl));
        }
        else {
            /*
             *  @todo FIX - WRONG!!!! - MUST SUPPORT TRANSFER ENCODING IN THIS CASE OR NOTHING
             *
             *  https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
             *      The rules for when a message-body is allowed in a message differ for requests and responses.
             *
             *      The presence of a message-body in a request is signaled by the inclusion of a Content-Length 
             *      or Transfer-Encoding header field in the request's message-headers/
             *
             *  For now - EMPTY is a better failure mode than reading everything and hanging...
             */
            fBodyInputStream_ = MemoryStream<byte>::New ();
        }
    }
    return fBodyInputStream_;
}

String Request::ToString () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    sb += L"{";
    sb += L"HTTPVersion: " + fHTTPVersion_ + L", ";
    sb += L"Method: " + fMethod_ + L", ";
    sb += L"URL: '" + Characters::ToString (fURL_) + L"', ";
    sb += L"Headers: " + Characters::ToString (fHeaders_) + L", ";
    sb += L"}";
    return sb.str ();
}
