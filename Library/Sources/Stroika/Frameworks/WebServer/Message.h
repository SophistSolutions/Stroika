/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Message_h_
#define _Stroika_Framework_WebServer_Message_h_ 1

#include    "../StroikaPreComp.h"

#include    "Request.h"
#include    "Response.h"


/*
 * TODO:
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {


            using   namespace   Stroika::Foundation;


            /**
             *  For now assume externally sycnhonized
             */
            struct  Message {
            public:
                Message () = delete;
                Message (const Message&) = delete;
                Message (Request&& request, Response&& response, const Memory::Optional<IO::Network::SocketAddress>& peerAddress = Memory::Optional<IO::Network::SocketAddress> {});

            public:
                nonvirtual  const Message& operator= (const Message&) = delete;

            public:
                /**
                    *  @see Socket::GetPeerAddress
                    */
                nonvirtual  Memory::Optional<IO::Network::SocketAddress> GetPeerAddress () const;

            public:
                /**
                 *  \ensure NotNull
                    */
                nonvirtual  const Request*  GetRequest () const;
                nonvirtual  Request*        GetRequest ();

            public:
                /**
                 *  \ensure NotNull
                    */
                nonvirtual  const Response* GetResponse () const;
                nonvirtual  Response*       GetResponse ();

            private:
                Memory::Optional<IO::Network::SocketAddress>    fPeerAddress_;
                Request                                         fRequest_;
                Response                                        fResponse_;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Message.inl"

#endif  /*_Stroika_Framework_WebServer_Message_h_*/
