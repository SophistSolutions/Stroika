/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>

#include <shellapi.h>
#include <winerror.h>
#include <wininet.h> // for error codes
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Characters/CString/Utilities.h"
#include "../../../Characters/Format.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Common.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/TimeOutException.h"
#if qPlatform_Windows
#include "HRESULTErrorException.h"
#endif
#include "../../../Time/Realtime.h"

#include "Exception.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform;
using namespace Stroika::Foundation::Execution::Platform::Windows;

// for InternetGetConnectedState
#if _MSC_VER
#pragma comment(lib, "Wininet.lib")
#endif

namespace {
    inline SDKString Win32Error2String_ (DWORD win32Err)
    {
        switch (win32Err) {
            case ERROR_NOT_ENOUGH_MEMORY:
                return SDKSTR ("Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)");
            case ERROR_OUTOFMEMORY:
                return SDKSTR ("Not enough memory to complete that operation (ERROR_OUTOFMEMORY)");
            case WSAEADDRNOTAVAIL:
                return SDKSTR ("Socket address not available (WSAEADDRNOTAVAIL)");
        }
        if (INTERNET_ERROR_BASE <= win32Err and win32Err < INTERNET_ERROR_BASE + INTERNET_ERROR_LAST) {
            switch (win32Err) {
                case ERROR_INTERNET_INVALID_URL:
                    return SDKSTR ("ERROR_INTERNET_INVALID_URL");
                case ERROR_INTERNET_CANNOT_CONNECT:
                    return SDKSTR ("Failed to connect to internet URL (ERROR_INTERNET_CANNOT_CONNECT)");
                case ERROR_INTERNET_NAME_NOT_RESOLVED:
                    return SDKSTR ("ERROR_INTERNET_NAME_NOT_RESOLVED");
                case ERROR_INTERNET_INCORRECT_HANDLE_STATE:
                    return SDKSTR ("ERROR_INTERNET_INCORRECT_HANDLE_STATE");
                case ERROR_INTERNET_TIMEOUT:
                    return SDKSTR ("Operation timed out (ERROR_INTERNET_TIMEOUT)");
                case ERROR_INTERNET_CONNECTION_ABORTED:
                    return SDKSTR ("ERROR_INTERNET_CONNECTION_ABORTED");
                case ERROR_INTERNET_CONNECTION_RESET:
                    return SDKSTR ("ERROR_INTERNET_CONNECTION_RESET");
                case ERROR_HTTP_INVALID_SERVER_RESPONSE:
                    return SDKSTR ("Invalid Server Response (ERROR_HTTP_INVALID_SERVER_RESPONSE)");
                case ERROR_INTERNET_PROTOCOL_NOT_FOUND: {
                    DWORD r = 0;
                    if (::InternetGetConnectedState (&r, 0) and (r & INTERNET_CONNECTION_OFFLINE) == 0) {
                        return SDKSTR ("ERROR_INTERNET_PROTOCOL_NOT_FOUND");
                    }
                    else {
                        return SDKSTR ("ERROR_INTERNET_PROTOCOL_NOT_FOUND (offline mode)");
                    }
                }
                default: {
                    TCHAR buf[1024];
                    (void)::_stprintf_s (buf, SDKSTR ("INTERNET error code: %d"), win32Err);
                    return buf;
                }
            }
        }
        TCHAR* lpMsgBuf = nullptr;
        if (not ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr,
                                 win32Err,
                                 MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                 reinterpret_cast<TCHAR*> (&lpMsgBuf),
                                 0,
                                 nullptr)) {
            return CString::Format (SDKSTR ("Win32 error# %d"), static_cast<DWORD> (win32Err));
        }
        SDKString result = lpMsgBuf;
        ::LocalFree (lpMsgBuf);
        return CString::Trim (result);
    }
}

/*
 ********************************************************************************
 ***************************** ThrowIfShellExecError ****************************
 ********************************************************************************
 */
void Execution::Platform::Windows::ThrowIfShellExecError (HINSTANCE r)
{
    DISABLE_COMPILER_MSC_WARNING_START (4302)
    DISABLE_COMPILER_MSC_WARNING_START (4311)
    int errCode = reinterpret_cast<int> (r);
    DISABLE_COMPILER_MSC_WARNING_END (4311)
    DISABLE_COMPILER_MSC_WARNING_END (4302)
    if (errCode <= 32) {
        DbgTrace ("ThrowIfShellExecError (0x%x) - throwing exception", errCode);
        switch (errCode) {
            case 0:
                Execution::ThrowSystemErrNo (ERROR_NOT_ENOUGH_MEMORY); // The operating system is out of memory or resources.
            case ERROR_FILE_NOT_FOUND:
                Execution::ThrowSystemErrNo (ERROR_FILE_NOT_FOUND); // The specified file was not found.
            case ERROR_PATH_NOT_FOUND:
                Execution::ThrowSystemErrNo (ERROR_PATH_NOT_FOUND); //  The specified path was not found.
            case ERROR_BAD_FORMAT:
                Execution::ThrowSystemErrNo (ERROR_BAD_FORMAT); //  The .exe file is invalid (non-Microsoft Win32� .exe or error in .exe image).
            case SE_ERR_ACCESSDENIED:
                Execution::Throw (SystemErrorException{E_ACCESSDENIED, HRESULT_error_category ()}); //  The operating system denied access to the specified file.
            case SE_ERR_ASSOCINCOMPLETE:
                Execution::ThrowSystemErrNo (ERROR_NO_ASSOCIATION); //  The file name association is incomplete or invalid.
            case SE_ERR_DDEBUSY:
                Execution::ThrowSystemErrNo (ERROR_DDE_FAIL); //  The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed.
            case SE_ERR_DDEFAIL:
                Execution::ThrowSystemErrNo (ERROR_DDE_FAIL); //  The DDE transaction failed.
            case SE_ERR_DDETIMEOUT:
                Execution::ThrowSystemErrNo (ERROR_DDE_FAIL); //  The DDE transaction could not be completed because the request timed out.
            case SE_ERR_DLLNOTFOUND:
                Execution::ThrowSystemErrNo (ERROR_DLL_NOT_FOUND); //  The specified dynamic-link library (DLL) was not found.
            //case  SE_ERR_FNF:             throw (Platform::Windows::Exception (ERROR_FILE_NOT_FOUND));        //  The specified file was not found.
            case SE_ERR_NOASSOC:
                Execution::ThrowSystemErrNo (ERROR_NO_ASSOCIATION); //  There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.
            case SE_ERR_OOM:
                Execution::ThrowSystemErrNo (ERROR_NOT_ENOUGH_MEMORY); //  There was not enough memory to complete the operation.
            //case  SE_ERR_PNF:             throw (Platform::Windows::Exception (ERROR_PATH_NOT_FOUND));        //  The specified path was not found.
            case SE_ERR_SHARE:
                Execution::ThrowSystemErrNo (ERROR_INVALID_SHARENAME); //
            default: {
                // Not sure what error to report here...
                Execution::ThrowSystemErrNo (ERROR_NO_ASSOCIATION);
            }
        }
    }
}

/*
 ********************************************************************************
 *********** Execution::RegisterDefaultHandler_invalid_parameter ****************
 ********************************************************************************
 */
namespace {
    /*
     *  Because of Microsoft's new secure-runtime-lib  - we must provide a handler to catch errors (shouldn't occur - but in case.
     *  We treat these largely like ASSERTION errors, but then translate them into a THROW of an exception - since that is
     *  probably more often the right thing todo.
     */
    void invalid_parameter_handler_ ([[maybe_unused]] const wchar_t* expression, [[maybe_unused]] const wchar_t* function, [[maybe_unused]] const wchar_t* file, [[maybe_unused]] unsigned int line, [[maybe_unused]] uintptr_t pReserved)
    {
        TraceContextBumper trcCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"invalid_parameter_handler", L"Func='%s', expr='%s', file='%s', line=%d.", function, expression, file, line)};
        Assert (false);
        Execution::ThrowSystemErrNo (ERROR_INVALID_PARAMETER);
    }
}
void Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ()
{
    (void)_set_invalid_parameter_handler (invalid_parameter_handler_);
}
