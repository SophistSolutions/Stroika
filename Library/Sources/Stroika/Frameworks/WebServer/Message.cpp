/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

using Debug::AssertExternallySynchronizedMutex;

/*
 ********************************************************************************
 ************************* WebServer::Message ***********************************
 ********************************************************************************
 */
Message::Message (Request&& srcRequest, Response&& srcResponse, const optional<IO::Network::SocketAddress>& peerAddress)
    : peerAddress{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<IO::Network::SocketAddress> {
              const Message*                                 thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::peerAddress);
              AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
              return thisObj->fPeerAddress_;
          }}
    , request{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Request& {
        const Message*                                 thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::request);
        AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fRequest_;
    }}
    , rwRequest{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Request& {
        Message*                                        thisObj = const_cast<Message*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::rwRequest));
        AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fRequest_;
    }}
    , response{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Response& {
        const Message*                                 thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::response);
        AssertExternallySynchronizedMutex::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fResponse_;
    }}
    , rwResponse{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Response& {
        Message*                                        thisObj = const_cast<Message*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::rwResponse));
        AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fResponse_;
    }}
    , fPeerAddress_{peerAddress}
    , fRequest_{move (srcRequest)}
    , fResponse_{move (srcResponse)}
{
#if qStroikaFoundationDebugAssertExternallySynchronizedMutexEnabled
    fRequest_.SetAssertExternallySynchronizedMutexContext (fThisAssertExternallySynchronized_.GetSharedContext ());
    fResponse_.SetAssertExternallySynchronizedMutexContext (fThisAssertExternallySynchronized_.GetSharedContext ());
#endif
}

#if qStroikaFoundationDebugAssertExternallySynchronizedMutexEnabled
void Message::SetAssertExternallySynchronizedMutexContext (const shared_ptr<AssertExternallySynchronizedMutex::SharedContext>& sharedContext)
{
    fThisAssertExternallySynchronized_.SetAssertExternallySynchronizedMutexContext (sharedContext);
    Assert (fThisAssertExternallySynchronized_.GetSharedContext () == sharedContext);
    fRequest_.SetAssertExternallySynchronizedMutexContext (sharedContext);
    fResponse_.SetAssertExternallySynchronizedMutexContext (sharedContext);
}
#endif

String Message::ToString () const
{
    AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
    StringBuilder                                  sb;
    sb += "{";
    sb += "PeerAddress: " + Characters::ToString (fPeerAddress_) + ", ";
    sb += "Request: " + Characters::ToString (fRequest_) + ", ";
    sb += "Response: " + Characters::ToString (fResponse_);
    sb += "}";
    return sb.str ();
}
