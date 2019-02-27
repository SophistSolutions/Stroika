/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_
#define _Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_ 1

#include "../../../StroikaPreComp.h"

#include <system_error>

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

namespace Stroika::Foundation::Execution::Platform::Windows {

    /**
     *  Return a reference the the HRESULT error category object (windows only). This object lives forever (like other error categories).
     */
    const std::error_category& StructuredException_error_category () noexcept;

    /**
     *  Windows generally defaults to having 'structured exceptions' cause the application to crash.
     *  This allows translating those exceptions into C++ exceptions.
     *
     *  @see https://msdn.microsoft.com/en-us/library/5z4bw5h5.aspx
     */
    void RegisterDefaultHandler_StructuredException ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StructuredException.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_*/
