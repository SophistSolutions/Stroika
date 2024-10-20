/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

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
