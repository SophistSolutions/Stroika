/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_COM_h_
#define _Stroika_Foundation_Execution_Platform_Windows_COM_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>

#include <objbase.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Configuration/Common.h"

namespace Stroika::Foundation::Execution::Platform::Windows {

    /**
     */
    class COMInitializer {
    public:
        /**
         *  Declare one of these objects to have the scope/lifetime for which you want COM initialized.
         *
         *  if (worksWithAnyCoInitFlag) then ignore RPC_E_CHANGED_MODE, and allow any successful COM initialization
         *
         *  \par Example Usage
         *      \code
         *          COMInitializer initCOMForScope { COINIT_APARTMENTTHREADED };
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          COMInitializer initCOMForScope { COINIT_MULTITHREADED };
         *      \endcode
         *
         *  \note - if using MFC and/or OleInitialize(), users may wish to create a COMInitializer instance (with COINIT_APARTMENTTHREADED)
         *      very early, to assure application reliably sets up use of COINIT_APARTMENTTHREADED initialization.
         */
        COMInitializer () = delete;
        COMInitializer (COINIT initFlag = COINIT_APARTMENTTHREADED, bool worksWithAnyCoInitFlag = true);
        COMInitializer (const COMInitializer&) = delete;
        ~COMInitializer ();

    public:
        nonvirtual COMInitializer& operator= (const COMInitializer&) = delete;

    private:
        bool fNeedsShutdown_{false};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "COM.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_COM_h_*/
