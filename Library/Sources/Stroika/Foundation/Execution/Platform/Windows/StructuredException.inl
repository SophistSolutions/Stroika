/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     **************** Platform::Windows::StructuredException ************************
     ********************************************************************************
     */
    inline StructuredException::operator unsigned int () const
    {
        return fSECode;
    }
    inline SDKString StructuredException::LookupMessage () const
    {
        return LookupMessage (fSECode);
    }

}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_StructuredException_inl_*/
