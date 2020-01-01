/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_h_
#define _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Configuration/Common.h"

#include "../../Exceptions.h"
#include "../../Throw.h"

namespace Stroika::Foundation::Execution {
    using Characters::SDKString;
}

namespace Stroika::Foundation::Execution::Platform::Windows {

    /**
     *  Return a reference the the HRESULT error category object (windows only). This object lives forever (like other error categories).
     */
    const error_category& HRESULT_error_category () noexcept;

    /**
     *  Translate a standard C++ (or Stroika) exception into the appropriate HRESULT to return
     */
#define CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION()         \
    catch (HRESULT hr)                                            \
    {                                                             \
        return hr;                                                \
    }                                                             \
    catch (const system_error& h)                                 \
    {                                                             \
        if (h.code ().category () == HRESULT_error_category ()) { \
            return h.code ().value ();                            \
        }                                                         \
        if (h.code ().category () == system_category ()) {        \
            return (HRESULT_FROM_WIN32 (h.code ().value ()));     \
        }                                                         \
        return DISP_E_EXCEPTION;                                  \
    }                                                             \
    catch (const bad_alloc&)                                      \
    {                                                             \
        return E_OUTOFMEMORY;                                     \
    }                                                             \
    catch (...)                                                   \
    {                                                             \
        return DISP_E_EXCEPTION;                                  \
    }

    /**
     *  if hr did not SUCCCEED, then throw HRESULTErrorException 
     */
    void ThrowIfErrorHRESULT (HRESULT hr);

}

namespace Stroika::Foundation::Execution {

    /**
     * \brief   Template specialization for ThrowIfNull (), for thing being thrown HRESULT - really throw HRESULTErrorException
     */
    template <>
    void ThrowIfNull (const void* p, const HRESULT& hr);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HRESULTErrorException.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_h_*/
