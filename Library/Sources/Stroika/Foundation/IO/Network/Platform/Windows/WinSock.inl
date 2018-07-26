/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Platform_Windows_WinSock_inl_
#define _Stroika_Foundation_IO_Network_Platform_Windows_WinSock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../../../Debug/Assertions.h"

namespace Stroika::Foundation::IO::Network::Platform::Windows {

    /*
     ********************************************************************************
     ************************ Network::Platform::Windows::WinSock *******************
     ********************************************************************************
     */
    inline void WinSock::NoteExternallyStarted ()
    {
        Require (not sStarted_);
        sStarted_ = true;
    }
    inline bool WinSock::IsStarted ()
    {
        return sStarted_;
    }
    inline void WinSock::AssureStarted ()
    {
        if (not sStarted_) {
            ForceStartup ();
        }
    }

}

#endif /*_Stroika_Foundation_IO_Network_Platform_Windows_WinSock_inl_*/
