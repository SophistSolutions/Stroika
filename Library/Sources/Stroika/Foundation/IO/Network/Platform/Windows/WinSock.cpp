/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <winsock2.h>

#include <ws2tcpip.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../../Execution/Platform/Windows/Exception.h"

#include "WinSock.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Platform;
using namespace Stroika::Foundation::IO::Network::Platform::Windows;

// Not sure why this is necessary, but we get link errors sometimes without it... Maybe a windows makefile issue on regtest apps?
// -- LGP 2014-11-06
#pragma comment(lib, "Ws2_32.lib")

/*
 ********************************************************************************
 ******************* Network::Platform::Windows::WinSock ************************
 ********************************************************************************
 */
bool WinSock::sStarted_{false};

void WinSock::ForceStartup ()
{
    Require (not sStarted_);
    WSADATA wsaData; // Initialize Winsock
    int     iResult = ::WSAStartup (MAKEWORD (2, 2), &wsaData);
    if (iResult != 0) {
        Execution::ThrowSystemErrNo (::WSAGetLastError ());
    }
    sStarted_ = true;
}
