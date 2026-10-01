/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_Windows
#include <Windows.h>
#include <wininet.h> // for INTERNET_ERROR_BASE
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "Exception.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "HRESULTErrorException.h"

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
            if (HRESULT_CODE (ev) or ev == 0) {
                // a wrapped Win32 code: what it means unwrapped (@see Win32_error_category)
                return Win32_error_category ().default_error_condition (HRESULT_CODE (ev));
            }
            else {
                return {ev, HRESULT_error_category ()}; // special error condition (the immortal object - NOT a temporary)
            }
        }
        virtual bool equivalent (int ev, const error_condition& cond) const noexcept override
        {
            if (HRESULT_CODE (ev) or ev == 0) {
                return Win32_error_category ().equivalent (HRESULT_CODE (ev), cond); // all it means unwrapped - not just its default
            }
            return error_category::equivalent (ev, cond);
        }
        virtual string message (int hr) const override
        {
            switch (hr) {
                case E_FAIL:
                    return "E_FAIL";
                case E_ACCESSDENIED:
                    return "E_ACCESSDENIED";
                case E_INVALIDARG:
                    return "E_INVALIDARG";
                case E_NOINTERFACE:
                    return "E_NOINTERFACE";
                case E_POINTER:
                    return "E_POINTER";
                case E_HANDLE:
                    return "E_HANDLE";
                case E_ABORT:
                    return "E_ABORT";
                case DISP_E_TYPEMISMATCH:
                    return "DISP_E_TYPEMISMATCH";
                case DISP_E_EXCEPTION:
                    return "DISP_E_EXCEPTION";
                case INET_E_RESOURCE_NOT_FOUND:
                    return "INET_E_RESOURCE_NOT_FOUND";
                case REGDB_E_CLASSNOTREG:
                    return "REGDB_E_CLASSNOTREG";
            }
            if (HRESULT_FACILITY (hr) == FACILITY_WIN32) {
                return Win32_error_category ().message (HRESULT_CODE (hr));
            }
            if (HRESULT_FACILITY (hr) == FACILITY_INTERNET) {
                unsigned int wCode = HRESULT_CODE (hr);
                if (wCode < INTERNET_ERROR_BASE) {
                    wCode += INTERNET_ERROR_BASE; // because the HRESULT_CODE doesn't (at least sometimes) include the INTERNET_ERROR_BASE
                    // included in the constants below...
                }
                return Win32_error_category ().message (HRESULT_CODE (wCode));
            }
            char buf[1024];
            (void)::snprintf (buf, std::size (buf), "HRESULT error code: 0x%x", hr);
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
