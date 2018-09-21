/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Characters/ToString.h"
#include "../../Containers/Collection.h"
#include "../../Execution/ErrNoException.h"
#include "../../Execution/StringException.h"
#include "../../Memory/BLOB.h"

#include "SocketAddress.h"

using std::byte;

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
    : fSocketAddress_ ()
{
    if (sockaddr.iSockaddrLength > sizeof (fSocketAddress_))
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Execution::StringException (L"bad socket address size"));
        }
    (void)::memcpy (&fSocketAddress_, sockaddr.lpSockaddr, sockaddr.iSockaddrLength);
}
#endif

String SocketAddress::ToString () const
{
    if (IsInternetAddress ()) {
        return Characters::ToString (GetInternetAddress ()) + L":" + Characters::Format (L"%d", static_cast<int> (GetPort ()));
    }
    else {
        return Characters::ToString (Memory::BLOB (reinterpret_cast<const byte*> (&fSocketAddress_), reinterpret_cast<const byte*> (&fSocketAddress_) + sizeof (fSocketAddress_)));
    }
}

/*
 ********************************************************************************
 ********************* IO::Network::SocketAddresses *****************************
 ********************************************************************************
 */
Traversal::Iterable<SocketAddress> Network::SocketAddresses (const Traversal::Iterable<InternetAddress>& internetAddresses, uint16_t portNumber)
{
    Containers::Collection<SocketAddress> tmp;
    for (auto i : internetAddresses) {
        tmp += SocketAddress{i, portNumber};
    }
    return tmp;
}
