/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "Response.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Foundation::IO::Network::HTTP;

using Debug::AssertExternallySynchronizedChecker;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************** IO::Network::HTTP::Response *****************************
 ********************************************************************************
 */
static_assert (not copyable<Response>); // ensure 'Satisfies Concepts' valid
static_assert (movable<Response>);

Response::Response (const optional<Headers>& initialHeaders)
    : headers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const IO::Network::HTTP::Headers& {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::headers);
        AssertExternallySynchronizedChecker::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fHeaders_;
    }}
    , rwHeaders{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> IO::Network::HTTP::Headers& {
                    const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::rwHeaders);
                    AssertExternallySynchronizedChecker::WriteContext declareContext{
                        const_cast<Response*> (thisObj)->_fThisAssertExternallySynchronized}; // not ReadContext cuz rw object returned
                    return const_cast<IO::Network::HTTP::Headers&> (thisObj->fHeaders_);
                },
                [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newHeaders) {
                    Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::rwHeaders);
                    AssertExternallySynchronizedChecker::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                    thisObj->fHeaders_ = newHeaders;
                }}
    , status{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                 const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::status);
                 AssertExternallySynchronizedChecker::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                 return get<0> (thisObj->fStatusAndOverrideReason_);
             },
             [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto newStatus) {
                 Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::status);
                 AssertExternallySynchronizedChecker::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                 thisObj->fStatusAndOverrideReason_ = make_tuple (newStatus, optional<String>{}); // if setting status, clear override string
             }}
    , statusAndOverrideReason{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::statusAndOverrideReason);
              AssertExternallySynchronizedChecker::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
              return thisObj->fStatusAndOverrideReason_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newStatusAndOverride) {
              Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::statusAndOverrideReason);
              AssertExternallySynchronizedChecker::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
              thisObj->fStatusAndOverrideReason_ = newStatusAndOverride;
          }}
    , contentType{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::contentType);
                      AssertExternallySynchronizedChecker::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                      return thisObj->headers ().contentType ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newCT) {
                      Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::contentType);
                      AssertExternallySynchronizedChecker::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                      thisObj->rwHeaders ().contentType = newCT;
                  }}
    , fStatusAndOverrideReason_{make_tuple (StatusCodes::kOK, optional<String>{})}
    , fHeaders_{Memory::NullCoalesce (initialHeaders)}
{
}

#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
void Response::SetAssertExternallySynchronizedCheckerContext (const shared_ptr<AssertExternallySynchronizedChecker::SharedContext>& sharedContext)
{
    _fThisAssertExternallySynchronized.SetAssertExternallySynchronizedCheckerContext (sharedContext);
    fHeaders_.SetAssertExternallySynchronizedCheckerContext (sharedContext);
}
#endif

String Response::ToString () const
{
    AssertExternallySynchronizedChecker::ReadContext declareContext{_fThisAssertExternallySynchronized};
    StringBuilder                                    sb;
    sb << "{"sv;
    sb << "statusAndOverrideReason: "sv << fStatusAndOverrideReason_;
    sb << ", headers: "sv << this->headers ();
    sb << "}"sv;
    return sb;
}
