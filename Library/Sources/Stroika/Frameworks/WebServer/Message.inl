/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Message_inl_
#define _Stroika_Frameworks_WebServer_Message_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include "../../Foundation/Containers/Common.h"

namespace Stroika::Frameworks::WebServer {

    /*
        ********************************************************************************
        ******************************** WebServer::Message ****************************
        ********************************************************************************
        */
    inline Message::Message (Request&& request, Response&& response, const optional<IO::Network::SocketAddress>& peerAddress)
        : fPeerAddress_ (peerAddress)
        , fRequest_ (move (request))
        , fResponse_ (move (response))
    {
    }
    inline optional<IO::Network::SocketAddress> Message::GetPeerAddress () const
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
    inline const Request& Message::GetRequestReference () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // inadequate testing - but best we can do with this API
        return fRequest_;
    }
    inline Request& Message::GetRequestReference ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this}; // inadequate testing - but best we can do with this API
        return fRequest_;
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
    inline URL Message::GetRequestURL () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRequest_.GetURL ();
    }
    inline String Message::GetRequestHTTPMethod () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRequest_.GetHTTPMethod ();
    }
    inline Memory::BLOB Message::GetRequestBody ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fRequest_.GetBody ();
    }
    inline void Message::SetResponseContentType (const InternetMediaType& contentType)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fResponse_.SetContentType (contentType);
    }
    template <typename... ARGS_TYPE>
    inline void Message::write (ARGS_TYPE&&... args)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fResponse_.write (forward<ARGS_TYPE> (args)...);
    }
    template <typename... ARGS_TYPE>
    inline void Message::printf (ARGS_TYPE&&... args)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fResponse_.write (forward<ARGS_TYPE> (args)...);
    }
    template <typename... ARGS_TYPE>
    inline void Message::writeln (ARGS_TYPE&&... args)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fResponse_.write (forward<ARGS_TYPE> (args)...);
    }

}
#endif /*_Stroika_Frameworks_WebServer_Message_inl_*/
