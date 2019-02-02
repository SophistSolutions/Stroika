/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
    */
    class HRESULTErrorException : public Execution::Exception<> {
    private:
        using inherited = Execution::Exception<>;

    public:
        HRESULTErrorException (HRESULT hresult);

    public:
        operator HRESULT () const;

    public:
        static SDKString LookupMessage (HRESULT hr);
        nonvirtual SDKString LookupMessage () const;

    private:
        HRESULT fHResult;
    };

    /**
    */
#define CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION()                                     \
    catch (HRESULT hr)                                                                        \
    {                                                                                         \
        return hr;                                                                            \
    }                                                                                         \
    catch (const Stroika::Foundation::Execution::Platform::Windows::Exception& we)            \
    {                                                                                         \
        return (HRESULT_FROM_WIN32 (we));                                                     \
    }                                                                                         \
    catch (const Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException& h) \
    {                                                                                         \
        return static_cast<HRESULT> (h);                                                      \
    }                                                                                         \
    catch (const bad_alloc&)                                                                  \
    {                                                                                         \
        return E_OUTOFMEMORY;                                                                 \
    }                                                                                         \
    catch (...)                                                                               \
    {                                                                                         \
        return DISP_E_EXCEPTION;                                                              \
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
