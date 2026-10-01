/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_Windows
#include <Windows.h>

#include <shellapi.h>
#include <winerror.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Trace.h"
#if qStroika_Platform_Windows
#include "HRESULTErrorException.h"
#endif
#include "Stroika/Foundation/Time/Realtime.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform;
using namespace Stroika::Foundation::Execution::Platform::Windows;

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
        DbgTrace ("ThrowIfShellExecError (0x{:x}) - throwing exception"_f, errCode);
        switch (errCode) {
            case 0:
                ThrowSystemErrNo (ERROR_NOT_ENOUGH_MEMORY); // The operating system is out of memory or resources.
            case ERROR_FILE_NOT_FOUND:
                ThrowSystemErrNo (ERROR_FILE_NOT_FOUND); // The specified file was not found.
            case ERROR_PATH_NOT_FOUND:
                ThrowSystemErrNo (ERROR_PATH_NOT_FOUND); //  The specified path was not found.
            case ERROR_BAD_FORMAT:
                ThrowSystemErrNo (ERROR_BAD_FORMAT); //  The .exe file is invalid (non-Microsoft Win32 .exe or error in .exe image).
            case SE_ERR_ACCESSDENIED:
                ThrowError (error_code{E_ACCESSDENIED, HRESULT_error_category ()}); //  The operating system denied access to the specified file.
            case SE_ERR_ASSOCINCOMPLETE:
                ThrowSystemErrNo (ERROR_NO_ASSOCIATION); //  The file name association is incomplete or invalid.
            case SE_ERR_DDEBUSY:
                ThrowSystemErrNo (ERROR_DDE_FAIL); //  The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed.
            case SE_ERR_DDEFAIL:
                ThrowSystemErrNo (ERROR_DDE_FAIL); //  The DDE transaction failed.
            case SE_ERR_DDETIMEOUT:
                ThrowSystemErrNo (ERROR_DDE_FAIL); //  The DDE transaction could not be completed because the request timed out.
            case SE_ERR_DLLNOTFOUND:
                ThrowSystemErrNo (ERROR_DLL_NOT_FOUND); //  The specified dynamic-link library (DLL) was not found.
            //case  SE_ERR_FNF:             throw (Platform::Windows::Exception (ERROR_FILE_NOT_FOUND));        //  The specified file was not found.
            case SE_ERR_NOASSOC:
                ThrowSystemErrNo (ERROR_NO_ASSOCIATION); //  There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.
            case SE_ERR_OOM:
                ThrowSystemErrNo (ERROR_NOT_ENOUGH_MEMORY); //  There was not enough memory to complete the operation.
            //case  SE_ERR_PNF:             throw (Platform::Windows::Exception (ERROR_PATH_NOT_FOUND));        //  The specified path was not found.
            case SE_ERR_SHARE:
                ThrowSystemErrNo (ERROR_INVALID_SHARENAME); //
            default: {
                // Not sure what error to report here...
                ThrowSystemErrNo (ERROR_NO_ASSOCIATION);
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
    void invalid_parameter_handler_ ([[maybe_unused]] const wchar_t* expression, [[maybe_unused]] const wchar_t* function,
                                     [[maybe_unused]] const wchar_t* file, [[maybe_unused]] unsigned int line, [[maybe_unused]] uintptr_t pReserved)
    {
        TraceContextBumper trcCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
            L"invalid_parameter_handler", L"Func='{}', expr='{}', file='{}', line={}."_f, function, expression, file, line)};
        Assert (false);
        ThrowSystemErrNo (ERROR_INVALID_PARAMETER);
    }
}
void Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ()
{
    (void)_set_invalid_parameter_handler (invalid_parameter_handler_);
}
