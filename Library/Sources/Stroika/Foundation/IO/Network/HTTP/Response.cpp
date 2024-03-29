/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "../../../Characters/Format.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/ToString.h"
#include "../../../Containers/Common.h"
#include "../../../Debug/Assertions.h"
#include "../../../Debug/Trace.h"
#include "../../../IO/Network/HTTP/Headers.h"

#include "Response.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Foundation::IO::Network::HTTP;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************** IO::Network::HTTP::Response *****************************
 ********************************************************************************
 */
Response::Response (Response&& src)
    : Response{}
{
    fStatusAndOverrideReason_ = move (src.fStatusAndOverrideReason_);
    fHeaders_                 = move (src.fHeaders_);
}

Response::Response (const optional<Headers>& initialHeaders)
    : headers{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const IO::Network::HTTP::Headers& {
              const Response*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::headers);
              shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
              return thisObj->fHeaders_;
          }}
    , rwHeaders{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> IO::Network::HTTP::Headers& {
                    const Response*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::rwHeaders);
                    lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj}; // not shared_lock cuz rw
                    return const_cast<IO::Network::HTTP::Headers&> (thisObj->fHeaders_);
                },
                [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newHeaders) {
                    Response*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::rwHeaders);
                    lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                    thisObj->fHeaders_ = newHeaders;
                }}
    , status{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                 const Response*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::status);
                 shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                 return get<0> (thisObj->fStatusAndOverrideReason_);
             },
             [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto newStatus) {
                 Response*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::status);
                 lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                 thisObj->fStatusAndOverrideReason_ = make_tuple (newStatus, optional<String>{}); // if setting status, clear override string
             }}
    , statusAndOverrideReason{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                                  const Response*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::statusAndOverrideReason);
                                  shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                                  return thisObj->fStatusAndOverrideReason_;
                              },
                              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newStatusAndOverride) {
                                  Response*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::statusAndOverrideReason);
                                  lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                                  thisObj->fStatusAndOverrideReason_ = newStatusAndOverride;
                              }}
    , contentType{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Response*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::contentType);
                      shared_lock<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                      return thisObj->headers ().contentType ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newCT) {
                      Response*                                           thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::contentType);
                      lock_guard<const AssertExternallySynchronizedMutex> critSec{*thisObj};
                      thisObj->rwHeaders ().contentType = newCT;
                  }}
    , fStatusAndOverrideReason_{make_tuple (StatusCodes::kOK, optional<String>{})}
    , fHeaders_{Memory::NullCoalesce (initialHeaders)}
{
}

#if qDebug
void Response::SetAssertExternallySynchronizedMutexContext (const shared_ptr<SharedContext>& sharedContext)
{
    AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext (sharedContext);
    fHeaders_.SetAssertExternallySynchronizedMutexContext (sharedContext);
}
#endif

String Response::ToString () const
{
    shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
    StringBuilder                                        sb;
    sb += L"{";
    sb += L"Status-And-Override-Reason: " + Characters::ToString (fStatusAndOverrideReason_) + L", ";
    sb += L"Headers: " + Characters::ToString (this->headers ()) + L", ";
    sb += L"}";
    return sb.str ();
}
