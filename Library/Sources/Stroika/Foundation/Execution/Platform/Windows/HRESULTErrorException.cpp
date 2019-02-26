/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#include <wininet.h> // for INTERNET_ERROR_BASE
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Characters/String.h"
#include "../../../Common/Immortalize.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Common.h"
#include "../../../Debug/Trace.h"
#include "../../../Time/Realtime.h"
#include "Exception.h"

#include "HRESULTErrorException.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform;
using namespace Stroika::Foundation::Execution::Platform::Windows;

namespace {
    class HRESULT_error_category_ : public error_category {
    public:
        virtual const char* name () const noexcept override
        {
            return "HRESULT error"; // used to return return "getaddrinfo"; - but the name DNS is more widely recognized, and even though this could be from another source, this name is more clear
        }
        virtual error_condition default_error_condition ([[maybe_unused]] int ev) const noexcept override
        {
            //switch (ev) {
            //}
            return error_condition (errc::bad_message); // no idea what to return here
        }
        virtual string message (int hr) const override
        {
            switch (hr) {
                case E_FAIL:
                    return ("E_FAIL");
                case E_ACCESSDENIED:
                    return ("E_ACCESSDENIED");
                case E_INVALIDARG:
                    return ("E_INVALIDARG");
                case E_NOINTERFACE:
                    return ("E_NOINTERFACE");
                case E_POINTER:
                    return ("E_POINTER");
                case E_HANDLE:
                    return ("E_HANDLE");
                case E_ABORT:
                    return ("E_ABORT");
                case DISP_E_TYPEMISMATCH:
                    return ("DISP_E_TYPEMISMATCH");
                case DISP_E_EXCEPTION:
                    return ("DISP_E_EXCEPTION");
                case INET_E_RESOURCE_NOT_FOUND:
                    return ("INET_E_RESOURCE_NOT_FOUND");
                case REGDB_E_CLASSNOTREG:
                    return ("REGDB_E_CLASSNOTREG");
            }
            if (HRESULT_FACILITY (hr) == FACILITY_WIN32) {
                return system_category ().message (HRESULT_CODE (hr));
            }
            if (HRESULT_FACILITY (hr) == FACILITY_INTERNET) {
                unsigned int wCode = HRESULT_CODE (hr);
                if (wCode < INTERNET_ERROR_BASE) {
                    wCode += INTERNET_ERROR_BASE; // because the HRESULT_CODE doesn't (at least sometimes) include the INTERNET_ERROR_BASE
                    // included in the constants below...
                }
                return system_category ().message (HRESULT_CODE (wCode));
            }
            char buf[1024];
            (void)::snprintf (buf, NEltsOf (buf), "HRESULT error code: 0x%x", hr);
            return buf;
        }
    };
}

/*
 ********************************************************************************
 ************* Platform::Windows::HRESULT_error_category ************************
 ********************************************************************************
 */
const error_category& Execution::Platform::Windows::HRESULT_error_category () noexcept
{
    return Common::Immortalize<HRESULT_error_category_> ();
}

/*
 ********************************************************************************
 ************* Platform::Windows::HRESULTErrorException *************************
 ********************************************************************************
 */
HRESULTErrorException::HRESULTErrorException (HRESULT hresult)
    : inherited (String::FromSDKString (LookupMessage (hresult)))
    , fHResult (hresult)
{
}

SDKString HRESULTErrorException::LookupMessage (HRESULT hr)
{
    switch (hr) {
        case E_FAIL:
            return SDKSTR ("HRESULT failure (E_FAIL)");
        case E_ACCESSDENIED:
            return SDKSTR ("HRESULT failure (E_ACCESSDENIED)");
        case E_INVALIDARG:
            return SDKSTR ("HRESULT failure (E_INVALIDARG)");
        case E_NOINTERFACE:
            return SDKSTR ("HRESULT failure (E_NOINTERFACE)");
        case E_POINTER:
            return SDKSTR ("HRESULT failure (E_POINTER)");
        case E_HANDLE:
            return SDKSTR ("HRESULT failure (E_HANDLE)");
        case E_ABORT:
            return SDKSTR ("HRESULT failure (E_ABORT)");
        case DISP_E_TYPEMISMATCH:
            return SDKSTR ("HRESULT failure (DISP_E_TYPEMISMATCH)");
        case DISP_E_EXCEPTION:
            return SDKSTR ("HRESULT failure (DISP_E_EXCEPTION)");
        case INET_E_RESOURCE_NOT_FOUND:
            return SDKSTR ("HRESULT failure (INET_E_RESOURCE_NOT_FOUND)");
        case REGDB_E_CLASSNOTREG:
            return SDKSTR ("HRESULT failure (REGDB_E_CLASSNOTREG)");
        case ERROR_NOT_ENOUGH_MEMORY:
            return SDKSTR ("Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)");
        case ERROR_OUTOFMEMORY:
            return SDKSTR ("Not enough memory to complete that operation (ERROR_OUTOFMEMORY)");
    }
    if (HRESULT_FACILITY (hr) == FACILITY_WIN32) {
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        return Execution::Platform::Windows::Exception::LookupMessage (HRESULT_CODE (hr));
        DISABLE_COMPILER_MSC_WARNING_END (4996);
    }
    if (HRESULT_FACILITY (hr) == FACILITY_INTERNET) {
        unsigned int wCode = HRESULT_CODE (hr);
        if (wCode < INTERNET_ERROR_BASE) {
            wCode += INTERNET_ERROR_BASE; // because the HRESULT_CODE doesn't (at least sometimes) include the INTERNET_ERROR_BASE
            // included in the constants below...
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        return Execution::Platform::Windows::Exception::LookupMessage (wCode);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
    }
    TCHAR buf[1024];
    (void)::_stprintf_s (buf, SDKSTR ("HRESULT error code: 0x%x"), hr);
    return buf;
}
