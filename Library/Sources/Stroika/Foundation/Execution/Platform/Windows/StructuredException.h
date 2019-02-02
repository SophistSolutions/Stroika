/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_
#define _Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Configuration/Common.h"

#include "../../Exception.h"
#include "../../Exceptions.h"

namespace Stroika::Foundation::Execution {
    using Characters::SDKString;
}

namespace Stroika::Foundation::Execution::Platform::Windows {

    class StructuredException : public Execution::Exception<> {
    private:
        using inherited = Execution::Exception<>;

    private:
        unsigned int fSECode;

    public:
        explicit StructuredException (unsigned int n);
        operator unsigned int () const;

    public:
        static SDKString LookupMessage (unsigned int n);
        nonvirtual SDKString LookupMessage () const;

    public:
        /**
         *  Windows generally defaults to having 'structured exceptions' cause the application to crash.
         *  This allows translating those exceptions into C++ exceptions (whe
         *
         *  @see https://msdn.microsoft.com/en-us/library/5z4bw5h5.aspx
         */
        static void RegisterHandler ();

    private:
        static void trans_func_ (unsigned int u, EXCEPTION_POINTERS* pExp);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StructuredException.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_h_*/
