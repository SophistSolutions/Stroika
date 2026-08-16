/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "Message.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using Debug::AssertExternallySynchronizedChecker;

/*
 ********************************************************************************
 ************************* WebServer::Message ***********************************
 ********************************************************************************
 */
static_assert (not copyable<Message>); // enforce docs Satisfies Concepts:
static_assert (movable<Message>);

Message::Message (Request&& srcRequest, Response&& srcResponse, const optional<IO::Network::SocketAddress>& peerAddress)
    : peerAddress{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<IO::Network::SocketAddress> {
        const Message* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::peerAddress);
        AssertExternallySynchronizedChecker::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fPeerAddress_;
    }}
    , request{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Request& {
        const Message* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::request);
        AssertExternallySynchronizedChecker::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fRequest_;
    }}
    , rwRequest{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Request& {
        Message* thisObj = const_cast<Message*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::rwRequest));
        AssertExternallySynchronizedChecker::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fRequest_;
    }}
    , response{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Response& {
        const Message* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::response);
        AssertExternallySynchronizedChecker::ReadContext readLock{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fResponse_;
    }}
    , rwResponse{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Response& {
        Message* thisObj = const_cast<Message*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Message::rwResponse));
        AssertExternallySynchronizedChecker::WriteContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        return thisObj->fResponse_;
    }}
    , fPeerAddress_{peerAddress}
    , fRequest_{move (srcRequest)}
    , fResponse_{move (srcResponse)}
{
#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
    fRequest_.SetAssertExternallySynchronizedCheckerContext (fThisAssertExternallySynchronized_.GetSharedContext ());
    fResponse_.SetAssertExternallySynchronizedCheckerContext (fThisAssertExternallySynchronized_.GetSharedContext ());
#endif
}

#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
void Message::SetAssertExternallySynchronizedCheckerContext (const shared_ptr<AssertExternallySynchronizedChecker::SharedContext>& sharedContext)
{
    fThisAssertExternallySynchronized_.SetAssertExternallySynchronizedCheckerContext (sharedContext);
    Assert (fThisAssertExternallySynchronized_.GetSharedContext () == sharedContext);
    fRequest_.SetAssertExternallySynchronizedCheckerContext (sharedContext);
    fResponse_.SetAssertExternallySynchronizedCheckerContext (sharedContext);
}
#endif

String Message::ToString () const
{
    AssertExternallySynchronizedChecker::ReadContext readLock{fThisAssertExternallySynchronized_};
    StringBuilder                                    sb;
    sb << "{"sv;
    sb << "peerAddress: "sv << fPeerAddress_;
    sb << ", request: "sv << fRequest_;
    sb << ", response: "sv << fResponse_;
    sb << "}"sv;
    return sb;
}
