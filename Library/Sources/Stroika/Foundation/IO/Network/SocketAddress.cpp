/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Characters/ToString.h"
#include "../../Containers/Collection.h"
#include "../../Execution/Exceptions.h"
#include "../../Memory/BLOB.h"

#include "SocketAddress.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

/*
 ********************************************************************************
 ********************* IO::Network::SocketAddress *******************************
 ********************************************************************************
 */
#if qPlatform_Windows
SocketAddress::SocketAddress (const SOCKET_ADDRESS& sockaddr)
    : fSocketAddress_{}
{
    if (sockaddr.iSockaddrLength > sizeof (fSocketAddressStorage_)) [[unlikely]] {
        Execution::Throw (Execution::Exception{"bad socket address size"sv});
    }
    (void)::memcpy (&fSocketAddressStorage_, sockaddr.lpSockaddr, sockaddr.iSockaddrLength);
}
#endif

String SocketAddress::ToString () const
{
    if (IsInternetAddress ()) {
        return Characters::ToString (GetInternetAddress ()) + ":" + Characters::Format (L"%d", static_cast<int> (GetPort ()));
    }
    else {
        return Characters::ToString (Memory::BLOB{reinterpret_cast<const byte*> (&fSocketAddress_),
                                                  reinterpret_cast<const byte*> (&fSocketAddress_) + GetRequiredSize ()});
    }
}

/*
 ********************************************************************************
 ********************* IO::Network::SocketAddresses *****************************
 ********************************************************************************
 */
Traversal::Iterable<SocketAddress> Network::SocketAddresses (const Traversal::Iterable<InternetAddress>& internetAddresses, PortType portNumber)
{
    Containers::Collection<SocketAddress> tmp;
    for (const auto& i : internetAddresses) {
        tmp += SocketAddress{i, portNumber};
    }
    return move (tmp);
}
