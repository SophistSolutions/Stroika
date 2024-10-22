/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** GetCurrentProcessID ******************************
     ********************************************************************************
     */
    inline pid_t GetCurrentProcessID ()
    {
#if qStroika_Foundation_Common_Platform_Windows
        return GetCurrentProcessId ();
#elif qStroika_Foundation_Common_Platform_POSIX
        return getpid ();
#else
        AssertNotImplemented ();
#endif
    }

}
