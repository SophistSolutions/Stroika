/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Connection_h_
#define _Stroika_Framework_WebServer_Connection_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/IO/Network/SocketStream.h"

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
             *  \brief  A Connection object represents the state (and socket) for an ongoing, active, HTTP Connection, managed by the ConnectionManager class
             *
             *  TODO:
             *      \todo REDO Connection object as Connection::REp and Ptr and what we store is conneciton ptr... using SmartPtr pattern
             *            used elsewhere in stroika - like Stream, Socket::Ptr etc...
             *      \note Connection doesn't CURRENTLY support HTTP keepalives (https://stroika.atlassian.net/browse/STK-637)
             *      \note @todo https://stroika.atlassian.net/browse/STK-638 - WebServer ConnectionManager (and Connection) handling restructure - so while reading headers, only one thread used
             */
            class Connection {
            public:
                Connection ()                  = delete;
                Connection (const Connection&) = delete;
                explicit Connection (const ConnectionOrientedSocket::Ptr& s, const InterceptorChain& interceptorChain = InterceptorChain{});

            public:
                ~Connection ();

            public:
                nonvirtual const Connection& operator= (const Connection&) = delete;

            public:
                // Must rethink this organization -but for now - call this once at start of connection to fill in details in
                // the  Request object
                nonvirtual void ReadHeaders ();

            public:
                // not sure we want this
                nonvirtual void Close ();

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

            private:
                InterceptorChain              fInterceptorChain_;
                ConnectionOrientedSocket::Ptr fSocket_;
                SocketStream::Ptr             fSocketStream_;
                Message                       fMessage_;

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
