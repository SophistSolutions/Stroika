/*/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Concrete_WindowsResourceManager_h_
#define _Stroika_Foundation_Execution_Resources_Concrete_WindowsResourceManager_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Execution/Resources/Manager.h"

namespace Stroika::Foundation::Execution::Resources::Concrete {

    /**
     * Provide ResourceManager access to a Windows resource section of a Windows executable.
     */
    class WindowsResourceManager : public Manager {
    public:
        WindowsResourceManager (HMODULE hModule);

    private:
        class Rep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WindowsResourceManager.inl"

#endif /*_Stroika_Foundation_Execution_Resources_Concrete_WindowsResourceManager_h_*/
