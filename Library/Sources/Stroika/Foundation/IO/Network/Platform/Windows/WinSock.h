/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Platform_Windows_WinSock_h_
#define _Stroika_Foundation_IO_Network_Platform_Windows_WinSock_h_ 1

#include "../../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

namespace Stroika::Foundation::IO::Network::Platform::Windows {

    /**
     *  Support single call to WSAStartup () for windows
     */
    struct WinSock {
        static void ForceStartup ();
        static void NoteExternallyStarted ();
        static bool IsStarted ();
        static void AssureStarted ();

    private:
        static bool sStarted_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WinSock.inl"

#endif /*_Stroika_Foundation_IO_Network_Platform_Windows_WinSock_h_*/
