/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <cstdio>

#if     qPlatform_POSIX
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <netinet/in.h>
#include    <netdb.h>
#include    <arpa/inet.h>
#include    <netinet/in.h>
#include    <linux/netlink.h>
#include    <linux/rtnetlink.h>
#include    <linux/ethtool.h>
#include    <linux/sockios.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <WS2tcpip.h>
#include    <Iphlpapi.h>
#include    <netioapi.h>
#endif

#include    "../../Characters/CString/Utilities.h"
#include    "../../Containers/Collection.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Finally.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include    "../../Memory/SmallStackBuffer.h"

#include    "Socket.h"

#include    "DNS.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

