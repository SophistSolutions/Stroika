/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_Exception_h_
#define _Stroika_Foundation_Execution_Platform_Windows_Exception_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Configuration/Common.h"
#include "../../../Execution/Exceptions.h"

namespace Stroika::Foundation::Execution::Platform::Windows {

    /**
     *  Most Windows (Win32) APIs return a BOOL (or sometimes int) - which if zero (false) means an error. Kinda the opposite of
     *  POSIX APIs.
     *
     *  These APIS retrieve their underlying error number (when the return value is zero) from ::GetLastError ().
     *
     *
     *  This function simplifies use of such functions, trnaslating their results into the appropriate exception (call to Execution::ThrowSystemErrNo)
     */
    template <typename WINDOWS_API_RESULT = int>
    void ThrowIfZeroGetLastError (WINDOWS_API_RESULT test);

    /**
     *  Some Windows APIs return ERROR_SUCCESSS (0) on success, and non-zero on failure (e.g https://docs.microsoft.com/en-us/windows/desktop/api/winreg/nf-winreg-regcreatekeyexa)
     *
     *  In that case, the function argument contains the Windows error code.
     */
    void ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);

    /**
     *  Some Windows APIs return NO_ERROR (0) on success, and non-zero on failure (e.g https://docs.microsoft.com/en-us/windows/desktop/api/iphlpapi/nf-iphlpapi-getipstatistics)
     *
     *  In that case, the function argument contains the Windows error code.
     */
    void ThrowIfNot_NO_ERROR (DWORD win32ErrCode);

    /**
     *  ShellExecute () has weird idiosyncratic error results, handled by this function.
     *
     *  @see https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/nf-shellapi-shellexecutea
     *
     *  \note @todo - this could possibly be reimplemented using a custom error category, but for now it translates its errors to Win32 standard error
     *        numbers and calls ThrowSystemErrNo ()
     */
    void ThrowIfShellExecError (HINSTANCE r);

    /**
     *  If you call RegisterDefaultHandler_invalid_parameter at application startup, subsequent calls
     *  to some C-runtime funtions withh invalid parameters will be translated into Platform::Windows::Exception (ERROR_INVALID_PARAMETER)
     *
     *  @see https://msdn.microsoft.com/en-us/library/a9yf33zb.aspx
     */
    void RegisterDefaultHandler_invalid_parameter ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_Exception_h_*/
