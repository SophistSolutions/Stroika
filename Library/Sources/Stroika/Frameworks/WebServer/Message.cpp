/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"

#include "Message.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ************************* WebServer::Message ***********************************
 ********************************************************************************
 */
Message::Message (Request&& request, Response&& response, const optional<IO::Network::SocketAddress>& peerAddress)
    : peerAddress{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<IO::Network::SocketAddress> {
              const Message*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::peerAddress);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fPeerAddress_;
          }}
    , request{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Request& {
        const Message*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::request);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return thisObj->fRequest_;
    }}
    , rwRequest{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Request& {
        const Message*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::rwRequest);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return const_cast<Request&> (thisObj->fRequest_);
    }}
    , response{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Response& {
        const Message*                                      thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::response);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return thisObj->fResponse_;
    }}
    , rwResponse{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Response& {
        const Message*                                     thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::rwResponse);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return const_cast<Response&> (thisObj->fResponse_);
    }}
    , fPeerAddress_{peerAddress}
    , fRequest_{move (request)}
    , fResponse_{move (response)}
{
}

String Message::ToString () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    sb += L"{";
    sb += L"PeerAddress: " + Characters::ToString (fPeerAddress_) + L", ";
    sb += L"Request: " + Characters::ToString (fRequest_) + L", ";
    sb += L"Response: " + Characters::ToString (fResponse_);
    sb += L"}";
    return sb.str ();
}
