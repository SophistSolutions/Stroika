/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Message_h_
#define _Stroika_Framework_WebServer_Message_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/IO/Network/URI.h"

#include "Stroika/Frameworks/WebServer/Request.h"
#include "Stroika/Frameworks/WebServer/Response.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using IO::Network::URI;

    /**
     *  A message refers to a single back-and-forth request/response pair exchanged over HTTP.
     *
     *  \note Satisfies Concepts:
     *      o   not copyable<Message>
     *      o   movable<Message>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Message {
    public:
        /**
         *  \note that a Message is not copyable (but is movable). Typically its passed around by reference.
         */
        Message ()               = delete;
        Message (const Message&) = delete;
        Message (Message&& src) noexcept;
        Message (Request&& request, Response&& response, const optional<IO::Network::SocketAddress>& peerAddress = nullopt);

    public:
        nonvirtual Message& operator= (const Message&) = delete;
        nonvirtual Message& operator= (Message&& rhs) noexcept;

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
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
        optional<IO::Network::SocketAddress> fPeerAddress_;
        Request                              fRequest_;
        Response                             fResponse_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Message.inl"

#endif /*_Stroika_Framework_WebServer_Message_h_*/
