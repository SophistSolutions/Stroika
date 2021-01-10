/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Message_h_
#define _Stroika_Framework_WebServer_Message_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../Foundation/IO/Network/URI.h"

#include "Request.h"
#include "Response.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using IO::Network::URI;

    /**
     *  For now assume externally sycnhronized
     */
    class Message : private Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        Message ()               = delete;
        Message (const Message&) = delete;
        Message (Request&& request, Response&& response, const optional<IO::Network::SocketAddress>& peerAddress = nullopt);

    public:
        nonvirtual Message& operator= (const Message&) = delete;

    public:
        /**
         *  @see Socket::GetPeerAddress
         */
        nonvirtual optional<IO::Network::SocketAddress> GetPeerAddress () const;

    public:
        /**
         *  \ensure NotNull
         */
        nonvirtual const Request* PeekRequest () const;
        nonvirtual Request* PeekRequest ();

    public:
        /**
         *  The reason for this naming is to emphasize that this returns a reference to an internal object (internal pointer) of the message
         */
        nonvirtual const Request& GetRequestReference () const;
        nonvirtual Request& GetRequestReference ();

    public:
        /**
         *  \ensure NotNull
         */
        nonvirtual const Response* PeekResponse () const;
        nonvirtual Response* PeekResponse ();

    public:
        /**
         *  \brief shorthand for "PeekRequest ()->GetURL ()"
         *
         *  \note This shorthand is provided here because its very commonly used from RequestHandlers, and because it provides better threadsafty testing
         */
        nonvirtual URI GetRequestURL () const;

    public:
        /**
         *  \brief shorthand for "PeekRequest ()->GetHTTPMethod ()"
         *
         *  \note This shorthand is provided here because its very commonly used from RequestHandlers, and because it provides better threadsafty testing
         */
        nonvirtual String GetRequestHTTPMethod () const;

    public:
        /**
         *  \brief shorthand for "PeekRequest ()->GetBody ()"
         *
         *  \note This shorthand is provided here because its very commonly used from RequestHandlers, and because it provides better threadsafty testing
         */
        nonvirtual Memory::BLOB GetRequestBody ();

    public:
        /**
         *  \brief shorthand for "PeekResponse ()->SetContentType ()"
         *
         *  \note This shorthand is provided here because its very commonly used from RequestHandlers, and because it provides better threadsafty testing
         */
        nonvirtual void SetResponseContentType (const InternetMediaType& contentType);

    public:
        /**
         *  \brief alias for PeekResponse ()->write (...args...);
         */
        template <typename... ARGS_TYPE>
        nonvirtual void write (ARGS_TYPE&&... args);

    public:
        /**
        *   \brief alias for PeekResponse ()->printf (...args...);
        */
        template <typename... ARGS_TYPE>
        nonvirtual void printf (ARGS_TYPE&&... args);

    public:
        /**
         *  \brief alias for PeekResponse ()->writeln (...args...);
         */
        template <typename... ARGS_TYPE>
        nonvirtual void writeln (ARGS_TYPE&&... args);

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        optional<IO::Network::SocketAddress> fPeerAddress_;
        Request                              fRequest_;
        Response                             fResponse_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Message.inl"

#endif /*_Stroika_Framework_WebServer_Message_h_*/
