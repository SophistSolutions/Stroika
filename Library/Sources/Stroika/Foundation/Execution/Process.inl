/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Process_inl_
#define _Stroika_Foundation_Execution_Process_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** GetCurrentProcessID ******************************
     ********************************************************************************
     */
    inline pid_t GetCurrentProcessID ()
    {
#if qPlatform_Windows
        return GetCurrentProcessId ();
#elif qPlatform_POSIX
        return getpid ();
#else
        AssertNotImplemented ();
#endif
    }

}

#endif /*_Stroika_Foundation_Execution_Process_inl_*/
