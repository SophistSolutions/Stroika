/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/BLOB.h"

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
        using namespace Characters::Literals;
        return Characters::ToString (GetInternetAddress ()) + ":"sv + Characters::Format ("{}"_f, GetPort ());
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
