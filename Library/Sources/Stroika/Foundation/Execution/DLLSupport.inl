/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_DLLSupport_inl_
#define _Stroia_Foundation_Execution_DLLSupport_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include "../Execution/Throw.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *********************************** DLLLoader **********************************
     ********************************************************************************
     */
    inline DLLLoader::operator DLLHandle () const
    {
        EnsureNotNull (fModule_);
        return fModule_;
    }
    inline ProcAddress DLLLoader::GetProcAddress (const char* procName) const
    {
        AssertNotNull (fModule_);
        RequireNotNull (procName);
#if qPlatform_Windows
        auto result{::GetProcAddress (fModule_, procName)};
        if (result == nullptr) {
            Execution::ThrowSystemErrNo ();
        }
        return result;
#else
        ProcAddress addr = dlsym (fModule_, procName);
        if (addr == nullptr) {
            dlerror (); // clear any old error
            addr = dlsym (fModule_, procName);
            // interface seems to be defined only for char*, not wide strings: may need to map procName as well
            const char* err = dlerror ();
            if (err != nullptr) [[unlikely]] {
                Execution::Throw (DLLException (err));
            }
        }
        return addr;
#endif
    }
    inline ProcAddress DLLLoader::GetProcAddress (const wchar_t* procName) const
    {
        AssertNotNull (fModule_);
        RequireNotNull (procName);
        return GetProcAddress (Characters::String{procName}.AsNarrowSDKString (Characters::eIgnoreErrors).c_str ());
    }
#if !qPlatform_Windows
    inline DLLException::DLLException (const char* message)
        : Execution::RuntimeErrorException<>{Characters::String::FromNarrowSDKString (message)}
    {
    }
#endif

}

#endif /*_Stroia_Foundation_Execution_DLLSupport_inl_*/
