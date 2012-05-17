/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#include    <wininet.h>     // for INTERNET_ERROR_BASE
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Configuration/Common.h"
#include    "../../../Containers/Common.h"
#include    "../../../Debug/Trace.h"
#include    "../../../Time/Realtime.h"
#include    "Exception.h"

#include    "HRESULTErrorException.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Execution::Platform;
using   namespace   Stroika::Foundation::Execution::Platform::Windows;







/*
 ********************************************************************************
 ********** Platform::Windows::HRESULTErrorException ****************************
 ********************************************************************************
 */
TString Platform::Windows::HRESULTErrorException::LookupMessage (HRESULT hr) {
    switch (hr) {
        case    E_FAIL:
            return TSTR ("HRESULT failure (E_FAIL)");
        case    E_ACCESSDENIED:
            return TSTR ("HRESULT failure (E_ACCESSDENIED)");
        case    E_INVALIDARG:
            return TSTR ("HRESULT failure (E_INVALIDARG)");
        case    E_NOINTERFACE:
            return TSTR ("HRESULT failure (E_NOINTERFACE)");
        case    E_POINTER:
            return TSTR ("HRESULT failure (E_POINTER)");
        case    E_HANDLE:
            return TSTR ("HRESULT failure (E_HANDLE)");
        case    E_ABORT:
            return TSTR ("HRESULT failure (E_ABORT)");
        case    DISP_E_TYPEMISMATCH:
            return TSTR ("HRESULT failure (DISP_E_TYPEMISMATCH)");
        case    DISP_E_EXCEPTION:
            return TSTR ("HRESULT failure (DISP_E_EXCEPTION)");
        case    INET_E_RESOURCE_NOT_FOUND:
            return TSTR ("HRESULT failure (INET_E_RESOURCE_NOT_FOUND)");
        case    REGDB_E_CLASSNOTREG:
            return TSTR ("HRESULT failure (REGDB_E_CLASSNOTREG)");
        case    ERROR_NOT_ENOUGH_MEMORY:
            return TSTR ("Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)");
        case    ERROR_OUTOFMEMORY:
            return TSTR ("Not enough memory to complete that operation (ERROR_OUTOFMEMORY)");
    }
    if (HRESULT_FACILITY (hr) == FACILITY_WIN32) {
        return Exception::LookupMessage (HRESULT_CODE (hr));
    }
    if (HRESULT_FACILITY (hr) == FACILITY_INTERNET) {
        unsigned int    wCode   =       HRESULT_CODE (hr);
        if (wCode < INTERNET_ERROR_BASE) {
            wCode += INTERNET_ERROR_BASE;       // because the HRESULT_CODE doesn't (at least sometimes) include the INTERNET_ERROR_BASE
            // included in the constants below...
        }
        return Exception::LookupMessage (wCode);
    }
    TCHAR   buf[1024];
    (void)::_stprintf_s (buf, TSTR ("HRESULT error code: 0x%x"), hr);
    return buf;
}
