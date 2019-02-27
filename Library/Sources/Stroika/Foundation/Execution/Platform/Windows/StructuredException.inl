/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../../Configuration/Common.h"

#include "../../Exceptions.h"

namespace Stroika::Foundation::Execution {
    using Characters::SDKString;
}

namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     **************** Platform::Windows::StructuredException ************************
     ********************************************************************************
     */
    class [[deprecated ("Since v2.1d18, use SystemErrorException{ hr, StructuredException_error_category () }")]] StructuredException : public Execution::Exception<>
    {
    private:
        using inherited = Execution::Exception<>;

    private:
        unsigned int fSECode;

    public:
        explicit StructuredException (unsigned int n);
        operator unsigned int () const
        {
            return fSECode;
        }

    public:
        static SDKString LookupMessage (unsigned int n);
        nonvirtual SDKString LookupMessage () const
        {
            return LookupMessage (fSECode);
        }

    public:
        /**
         *  Windows generally defaults to having 'structured exceptions' cause the application to crash.
         *  This allows translating those exceptions into C++ exceptions (whe
         *
         *  @see https://msdn.microsoft.com/en-us/library/5z4bw5h5.aspx
         */
        [[deprecated ("Since v2.1d18, use RegisterDefaultHandler_StructuredException")]] static void RegisterHandler ();

    private:
        static void trans_func_ (unsigned int u, EXCEPTION_POINTERS* pExp);
    };

}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_*/
