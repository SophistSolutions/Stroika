/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Message_h_
#define _Stroika_Framework_WebServer_Message_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Common/Property.h"
#include "../../Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "../../Foundation/IO/Network/URI.h"

#include "Request.h"
#include "Response.h"

/*
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using IO::Network::URI;

    /**
     *  A message refers to a single back-and-forth request/response pair exchanged over HTTP.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Message {
    public:
        /**
         */
        Message ()               = delete;
        Message (const Message&) = delete;
        Message (Request&& request, Response&& response, const optional<IO::Network::SocketAddress>& peerAddress = nullopt);

    public:
        nonvirtual Message& operator= (const Message&) = delete;

#if qDebug
    public:
        /**
         *  Allow users of the Connection object to have it share a 'assure externally synchronized' context.
         *
         *  \see AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext
         */
        nonvirtual void SetAssertExternallySynchronizedMutexContext (const shared_ptr<Debug::AssertExternallySynchronizedMutex::SharedContext>& sharedContext);
#endif

    public:
        /**
         *  This is the network address where the message (request) came from and where the message response is
         *  going to be returned to. This can be sometimes omitted/unknown (HOW UNKNOWN - TBD DOCUMENT)
         *
         *  @see ConnectionOrientedStreamSocket::Ptr::GetPeerAddress
         */
        Common::ReadOnlyProperty<optional<IO::Network::SocketAddress>> peerAddress;

    public:
        /**
         *  Returns a read-only reference (so not assignable to) to a const Request& (so cannot modify the request).
         */
        Common::ReadOnlyProperty<const Request&> request;

    public:
        /**
         *  Returns a read-only reference (so not assignable) to a Request& (so CAN modify the request).
         *  This should very rarely be used (just perhaps when constructing the original message).
         *  Generally use request instead.
         */
        Common::ReadOnlyProperty<Request&> rwRequest;

    public:
        /**
         *  Returns a read-only reference (so not assignable) to a const Response& (so cannot modify the response).
         */
        Common::ReadOnlyProperty<const Response&> response;

    public:
        /**
         *  Returns a read-only reference (so not assignable) to a Response& (so CAN modify the response).
         *  Typically, users will want to access the 'rwResponse' not the 'response', since the underlying response object is frequently being updated.
         */
        Common::ReadOnlyProperty<Response&> rwResponse;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        optional<IO::Network::SocketAddress>                           fPeerAddress_;
        Request                                                        fRequest_;
        Response                                                       fResponse_;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Message.inl"

#endif /*_Stroika_Framework_WebServer_Message_h_*/
