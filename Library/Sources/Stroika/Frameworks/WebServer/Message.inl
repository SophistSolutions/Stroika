/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Message_inl_
#define _Stroika_Frameworks_WebServer_Message_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Containers/Common.h"

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            /*
             ********************************************************************************
             ******************************** WebServer::Message ****************************
             ********************************************************************************
             */
            inline Message::Message (Request&& request, Response&& response, const Memory::Optional<IO::Network::SocketAddress>& peerAddress)
                : fPeerAddress_ (peerAddress)
                , fRequest_ (move (request))
                , fResponse_ (move (response))
            {
            }
            inline Memory::Optional<IO::Network::SocketAddress> Message::GetPeerAddress () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                return fPeerAddress_;
            }
            inline const Request* Message::PeekRequest () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // inadequate testing - but best we can do with this API
                return &fRequest_;
            }
            inline Request* Message::PeekRequest ()
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this}; // inadequate testing - but best we can do with this API
                return &fRequest_;
            }
            inline const Response* Message::PeekResponse () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // inadequate testing - but best we can do with this API
                return &fResponse_;
            }
            inline Response* Message::PeekResponse ()
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this}; // inadequate testing - but best we can do with this API
                return &fResponse_;
            }
        }
    }
}
#endif /*_Stroika_Frameworks_WebServer_Message_inl_*/
