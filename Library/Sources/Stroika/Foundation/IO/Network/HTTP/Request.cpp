/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
#include "../../../Memory/SmallStackBuffer.h"

#include "Request.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Foundation::IO::Network::HTTP;

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
    : httpVersion{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> String {
              const Request*                                       thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpVersion);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fHTTPVersion_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& versionOrVersionLabel) {
              Request*                                            thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpVersion);
              lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              static const String                                 kLabeled_10_{L"HTTP/1.0"sv};
              static const String                                 kLabeled_11_{L"HTTP/1.1"sv};
              auto                                                versionStringComparer = String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive};
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
              const Request*                                       thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpMethod);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fMethod_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& method) {
              Request*                                            thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::httpMethod);
              lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              thisObj->fMethod_ = method;
          }}
    , url{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Request*                                       thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::url);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fURL_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& url) {
              Request*                                            thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::url);
              lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              thisObj->fURL_ = url;
          }}
    , headers{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const IO::Network::HTTP::Headers& {
              const Request*                                       thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::headers);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fHeaders_;
          }}
    , rwHeaders{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> IO::Network::HTTP::Headers& {
                    const Request*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::rwHeaders);
                    lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj}; // not shared_lock cuz rw
                    return const_cast<IO::Network::HTTP::Headers&> (thisObj->fHeaders_);
                },
                [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newHeaders) {
                    Request*                                            thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::rwHeaders);
                    lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                    thisObj->fHeaders_ = newHeaders;
                }}
    , contentType{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Request*                                       thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::contentType);
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
        return thisObj->fHeaders_.contentType ();
    }}
{
}

#if qDebug
void Request::SetAssertExternallySynchronizedMutexContext (const shared_ptr<SharedContext>& sharedContext)
{
    AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext (sharedContext);
    fHeaders_.SetAssertExternallySynchronizedMutexContext (sharedContext);
}
#endif

String Request::ToString () const
{
    shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
    StringBuilder                                        sb;
    sb += L"{";
    sb += L"HTTPVersion: " + fHTTPVersion_ + L", ";
    sb += L"Method: " + fMethod_ + L", ";
    sb += L"URL: " + Characters::ToString (fURL_) + L", ";
    sb += L"Headers: " + Characters::ToString (fHeaders_) + L", ";
    sb += L"}";
    return sb.str ();
}

/// HACK putting in this file
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
#include "Methods.h"
const RegularExpression MethodsRegEx::kGet       = L"GET"_RegEx;
const RegularExpression MethodsRegEx::kPut       = L"PUT"_RegEx;
const RegularExpression MethodsRegEx::kPatch     = L"PATCH"_RegEx;
const RegularExpression MethodsRegEx::kPost      = L"POST"_RegEx;
const RegularExpression MethodsRegEx::kPostOrPut = L"PUT|POST"_RegEx;
const RegularExpression MethodsRegEx::kDelete    = L"DELETE"_RegEx;
#endif