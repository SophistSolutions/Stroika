/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_COM_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_COM_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../../Debug/Trace.h"

#include "HRESULTErrorException.h"

namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    inline COMInitializer::COMInitializer (COINIT initFlag, bool worksWithAnyCoInitFlag)
    {
        Debug::TraceContextBumper ctx{L"Execution::Platform::Windows::COMInitializer::CTOR"};
        HRESULT                   hr = ::CoInitializeEx (nullptr, initFlag);
        switch (hr) {
            case S_OK:
                fNeedsShutdown_ = true;
                return;
            case S_FALSE:
                fNeedsShutdown_ = true;
                return; // harmless, already initialized
            case RPC_E_CHANGED_MODE: {
                if (worksWithAnyCoInitFlag) {
                    fNeedsShutdown_ = true;
                    DbgTrace ("RPC_E_CHANGED_MODE being ignored");
                    return; // harmless, already initialized and we don't care how
                }
                else {
                    ThrowIfErrorHRESULT (RPC_E_CHANGED_MODE);
                }
            }
            default:
                ThrowIfErrorHRESULT (hr);
        }
    }
    inline COMInitializer::~COMInitializer ()
    {
        Debug::TraceContextBumper ctx{L"Execution::Platform::Windows::COMInitializer::~COMInitializer"};
        if (fNeedsShutdown_) {
            ::CoUninitialize ();
        }
    }

}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_COM_inl_*/
