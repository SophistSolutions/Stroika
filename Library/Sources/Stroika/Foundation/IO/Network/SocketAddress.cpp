/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include     "../../Characters/ToString.h"
#include     "../../Memory/BLOB.h"
#include     "../../Execution/ErrNoException.h"
#include     "../../Execution/StringException.h"

#include    "SocketAddress.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;




/*
 ********************************************************************************
 ********************* IO::Network::SocketAddress *******************************
 ********************************************************************************
 */
#if     qPlatform_Windows
SocketAddress::SocketAddress (const SOCKET_ADDRESS& sockaddr)
    : fSocketAddress_ ()
{
    if (sockaddr.iSockaddrLength > sizeof (fSocketAddress_)) {
        Execution::Throw (Execution::StringException (L"bad socket address size"));
    }
    (void)::memcpy (&fSocketAddress_, sockaddr.lpSockaddr, sockaddr.iSockaddrLength);
}
#endif

String  SocketAddress::ToString () const
{
    if (IsInternetAddress ()) {
        return Characters::ToString (GetInternetAddress ()) + L":" + Characters::ToString (GetPort ());
    }
    else {
        return Characters::ToString (Memory::BLOB (reinterpret_cast<const Byte*> (&fSocketAddress_), reinterpret_cast<const Byte*> (&fSocketAddress_) + sizeof (fSocketAddress_)));
    }
}
