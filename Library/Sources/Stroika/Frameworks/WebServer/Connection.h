/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Connection_h_
#define _Stroika_Framework_WebServer_Connection_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "../../Foundation/IO/Network/SocketStream.h"
#include "../../Foundation/Streams/TextWriter.h"

#include "InterceptorChain.h"
#include "Message.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO;
    using Characters::String;

    using Stroika::Foundation::IO::Network::ConnectionOrientedStreamSocket;
    using Stroika::Foundation::IO::Network::SocketStream;

    class ConnectionManager;

    /**
     *  Write out files to %TEMP% dir, with logs of the details of the HTTP conversation, for debugging
     *  HTTP conversations.
     */
//#define qStroika_Framework_WebServer_Connection_DetailedMessagingLog 1
#ifndef qStroika_Framework_WebServer_Connection_DetailedMessagingLog
#define qStroika_Framework_WebServer_Connection_DetailedMessagingLog 0
#endif

    /**
     *  \brief  A Connection object represents the state (and socket) for an ongoing, active, HTTP Connection, managed by the ConnectionManager class
     *
     *  TODO:
     *      \note @todo https://stroika.atlassian.net/browse/STK-638 - WebServer ConnectionManager (and Connection) handling restructure - so while reading headers, only one thread used
     *
     *  This tends to get used internally by the ConnectionManager, but you can use it directly. For example:
     *
     *  \par Example Usage
     *      \code
     *          Connection conn{acceptedSocketConnection,
     *                       Sequence<Interceptor>{
     *                          Interceptor (
     *                              [=](Message* m) {
     *                                  Response* response = m->PeekResponse ();
     *                                  response->AddHeader (IO::Network::HTTP::HeaderName::kServer, L"stroika-ssdp-server-demo");
     *                                  response->write (Stroika::Frameworks::UPnP::Serialize (d, dd));
     *                                  response->SetContentType (DataExchange::InternetMediaTypes::kText_XML);
     *                               })}};
     *          conn.SetRemainingConnectionMessages (Connection::Remaining{0, 0}); // disable keep-alives
     *          conn.ReadAndProcessMessage ();
     *      \endcode
     */
    class Connection {
    public:
        /**
         */
        Connection ()                  = delete;
        Connection (const Connection&) = delete;
        explicit Connection (const ConnectionOrientedStreamSocket::Ptr& s, const InterceptorChain& interceptorChain = InterceptorChain{});

    public:
        ~Connection ();

    public:
        nonvirtual Connection& operator= (const Connection&) = delete;

    public:
        /**
         */
        nonvirtual ConnectionOrientedStreamSocket::Ptr GetSocket () const;

    public:
        /**
         */
        nonvirtual Request& GetRequest ();
        nonvirtual const Request& GetRequest () const;

    public:
        /**
         */
        nonvirtual Response& GetResponse ();
        nonvirtual const Response& GetResponse () const;

    public:
        /**
         */
        enum ReadAndProcessResult {
            eTryAgainLater, // Could mean success or sonme kinds of failure (like incomplete header/data), but try again later (so keep-alive results in this)
            eClose,
        };

    public:
        /**
         *  Return eTryAgainLater if 'keep alive' (or otherwise should try again - like incomplete input).
         */
        nonvirtual ReadAndProcessResult ReadAndProcessMessage () noexcept;

    public:
        /**
         */
        struct Remaining {
            optional<unsigned int>              fMessages;
            optional<Time::DurationSecondsType> fTimeoutAt;

        public:
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         */
        nonvirtual optional<Remaining> GetRemainingConnectionLimits () const;

    public:
        /**
         *  \note set Remaining::fMessages := 0 to prevent keep-alives.
         *
         *  \par Example Usage
         *      \code
         *          conn.SetRemainingConnectionMessages (Connection::Remaining{0, 0}); // disable keep-alives
         *      \endcode
         */
        nonvirtual void SetRemainingConnectionMessages (const optional<Remaining>& remainingConnectionLimits);

#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    private:
        nonvirtual void WriteLogConnectionMsg_ (const String& msg) const;
#endif

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString (bool abbreviatedOutput = true) const;

    private:
        struct MyMessage_ : Message {
            MyMessage_ (const ConnectionOrientedStreamSocket::Ptr& socket, const Streams::InputOutputStream<byte>::Ptr& socketStream);

            // Only valid until the end of a successful ReadHeaders
            MessageStartTextInputStreamBinaryAdapter::Ptr fMsgHeaderInTextStream;

            // If result bad, throw exception
            enum ReadHeadersResult { eIncompleteButMoreMayBeAvailable,
                                     eIncompleteDeadEnd,
                                     eCompleteGood };
            nonvirtual ReadHeadersResult ReadHeaders (
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                const function<void (const String&)>& logMsg
#endif
            );
        };

    private:
        InterceptorChain                      fInterceptorChain_;
        ConnectionOrientedStreamSocket::Ptr   fSocket_;
        Streams::InputOutputStream<byte>::Ptr fSocketStream_;
        shared_ptr<MyMessage_>                fMessage_; // always there, but ptr so it can be replaced
        optional<Remaining>                   fRemaining_;
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        Streams::TextWriter::Ptr fLogConnectionState_;
#endif

    private:
        friend ConnectionManager;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Framework_WebServer_Connection_h_*/
