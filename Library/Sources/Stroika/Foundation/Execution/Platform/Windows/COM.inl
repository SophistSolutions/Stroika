/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "HRESULTErrorException.h"

namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     ********************************** COMInitializer ******************************
     ********************************************************************************
     */
    inline COMInitializer::COMInitializer (COINIT initFlag, bool worksWithAnyCoInitFlag)
    {
        using namespace Characters::Literals;
        Debug::TraceContextBumper ctx{"Execution::Platform::Windows::COMInitializer::CTOR"};
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
                    DbgTrace ("RPC_E_CHANGED_MODE being ignored"_f);
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
        Debug::TraceContextBumper ctx{"Execution::Platform::Windows::COMInitializer::~COMInitializer"};
        if (fNeedsShutdown_) {
            ::CoUninitialize ();
        }
    }

}
