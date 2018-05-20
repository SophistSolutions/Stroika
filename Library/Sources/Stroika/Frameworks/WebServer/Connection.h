/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Connection_h_
#define _Stroika_Framework_WebServer_Connection_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/IO/Network/SocketStream.h"
#include "../../Foundation/Streams/TextWriter.h"

#include "InterceptorChain.h"
#include "Message.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;
            using namespace Stroika::Foundation::IO;
            using Characters::String;

            using Stroika::Foundation::IO::Network::ConnectionOrientedSocket;
            using Stroika::Foundation::IO::Network::SocketStream;

            class ConnectionManager;

            /**
             *  Write out files to %TEMP% dir, with logs of the details of the HTTP conversation, for debugging
             *  HTTP conversations.
             */
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
             *                                  response->SetContentType (DataExchange::PredefinedInternetMediaType::kText_XML);
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
                explicit Connection (const ConnectionOrientedSocket::Ptr& s, const InterceptorChain& interceptorChain = InterceptorChain{});

            public:
                ~Connection ();

            public:
                nonvirtual Connection& operator= (const Connection&) = delete;

            private:
                // Must rethink this organization -but for now - call this once at start of connection to fill in details in
                // the  Request object
                //
                // Return false if 'silent exception' - like empty connection
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                nonvirtual bool ReadHeaders_ (Message* msg);
#else
                static bool ReadHeaders_ (Message* msg);
#endif

            public:
                // Cannot do here becuase request maybe already sent...
                [[deprecated ("use - Response.MarkConnectionClose in Stroika v2.0a230")]] nonvirtual void Close ();

            public:
                /**
                 */
                nonvirtual ConnectionOrientedSocket::Ptr GetSocket () const;

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
                 *  Return true if 'keep alive'
                 */
                nonvirtual bool ReadAndProcessMessage ();

            public:
                /**
                 */
                struct Remaining {
                    Memory::Optional<unsigned int>              fMessages;
                    Memory::Optional<Time::DurationSecondsType> fTimeoutAt;

                public:
                    /**
                     *  @see Characters::ToString ();
                     */
                    nonvirtual String ToString () const;
                };

            public:
                /**
                 */
                nonvirtual Memory::Optional<Remaining> GetRemainingConnectionLimits () const;

            public:
                /**
                 *  \note set Remaining::fMessages := 0 to prevent keep-alives.
                 *
                 *  \par Example Usage
                 *      \code
                 *          conn.SetRemainingConnectionMessages (Connection::Remaining{0, 0}); // disable keep-alives
                 *      \endcode
                 */
                nonvirtual void SetRemainingConnectionMessages (const Memory::Optional<Remaining>& remainingConnectionLimits);

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
                InterceptorChain                              fInterceptorChain_;
                ConnectionOrientedSocket::Ptr                 fSocket_;
                Streams::InputOutputStream<Memory::Byte>::Ptr fSocketStream_;
                shared_ptr<Message>                           fMessage_; // always there, but ptr so it can be replaced
                Memory::Optional<Remaining>                   fRemaining_;
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                Streams::TextWriter::Ptr fLogConnectionState_;
#endif

            private:
                friend ConnectionManager;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Framework_WebServer_Connection_h_*/
