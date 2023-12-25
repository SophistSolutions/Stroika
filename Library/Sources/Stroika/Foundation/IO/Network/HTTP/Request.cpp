/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/ToString.h"
#include "../../../Containers/Common.h"
#include "../../../Debug/Assertions.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"
#include "../../../IO/Network/HTTP/Headers.h"
#include "../../../IO/Network/HTTP/Versions.h"

#include "Request.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Foundation::IO::Network::HTTP;

using Debug::AssertExternallySynchronizedMutex;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *************************** WebServer::Request *********************************
 ********************************************************************************
 */
Request::Request (Request&& src)
    : Request{}
{
    fHTTPVersion_ = move (src.fHTTPVersion_);
    fMethod_      = move (src.fMethod_);
    fURL_         = move (src.fURL_);
    fHeaders_     = move (src.fHeaders_);
}

Request::Request ()
    : httpVersion{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> String {
                      const Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpVersion);
                      Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                      return thisObj->fHTTPVersion_;
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& versionOrVersionLabel) {
                      Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpVersion);
                      AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                      static const String                             kLabeled_10_{"HTTP/1.0"sv};
                      static const String                             kLabeled_11_{"HTTP/1.1"sv};
                      auto versionStringComparer = String::EqualsComparer{Characters::eCaseInsensitive};
                      if (versionOrVersionLabel == kLabeled_11_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointOne or
                          versionStringComparer (versionOrVersionLabel, kLabeled_11_)) {
                          thisObj->fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointOne;
                      }
                      else if (versionOrVersionLabel == kLabeled_10_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointZero or
                               versionStringComparer (versionOrVersionLabel, kLabeled_10_)) {
                          thisObj->fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointZero;
                      }
                      else if (versionOrVersionLabel.StartsWith ("HTTP/"sv, Characters::eCaseInsensitive)) {
                          thisObj->fHTTPVersion_ = versionOrVersionLabel.SubString (5);
                      }
                      else {
                          thisObj->fHTTPVersion_ = versionOrVersionLabel;
                      }
                  }}
    , httpMethod{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                     const Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpMethod);
                     Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                     return thisObj->fMethod_;
                 },
                 [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& method) {
                     Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpMethod);
                     Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                     thisObj->fMethod_ = method;
                 }}
    , url{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::url);
              Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
              return thisObj->fURL_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& url) {
              Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::url);
              Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
              thisObj->fURL_ = url;
          }}
    , headers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const IO::Network::HTTP::Headers& {
        const Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::headers);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fHeaders_;
    }}
    , rwHeaders{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> IO::Network::HTTP::Headers& {
                    Request* thisObj = const_cast<Request*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::rwHeaders));
                    AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized}; // not shared_lock cuz rw
                    return thisObj->fHeaders_;
                },
                [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newHeaders) {
                    Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::rwHeaders);
                    AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                    thisObj->fHeaders_ = newHeaders;
                }}
    , contentType{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::contentType);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fHeaders_.contentType ();
    }}
{
}

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
void Request::SetAssertExternallySynchronizedMutexContext (const shared_ptr<Debug::AssertExternallySynchronizedMutex::SharedContext>& sharedContext)
{
    _fThisAssertExternallySynchronized.SetAssertExternallySynchronizedMutexContext (sharedContext);
    fHeaders_.SetAssertExternallySynchronizedMutexContext (sharedContext);
}
#endif

String Request::ToString () const
{
    Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
    StringBuilder                                         sb;
    sb << "{"sv;
    sb << "HTTPVersion: "sv << fHTTPVersion_ << ", "sv;
    sb << "Method: "sv << fMethod_ << ", "sv;
    sb << "URL: "sv << Characters::ToString (fURL_) << ", "sv;
    sb << "Headers: "sv << Characters::ToString (fHeaders_);
    sb << "}"sv;
    return sb.str ();
}
