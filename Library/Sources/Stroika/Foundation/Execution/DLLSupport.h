/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_DLLSupport_h_
#define _Stroia_Foundation_Execution_DLLSupport_h_ 1

#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include "../Characters/SDKString.h"
#include "../Execution/Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::SDKChar;
    using Characters::SDKString;

#if qPlatform_Windows
    using DLLHandle   = HMODULE;
    using ProcAddress = FARPROC;
#else
    using DLLHandle   = void*;
    using ProcAddress = void*;
#endif

#if !qPlatform_Windows
    class DLLException : public Execution::RuntimeErrorException<> {
    public:
        DLLException (const char* message);
    };
#endif

    /**
     */
    class DLLLoader {
    public:
        DLLLoader (const SDKChar* dllName);
        DLLLoader (const SDKChar* dllName, const vector<SDKString>& searchPath);
        ~DLLLoader ();

    public:
        nonvirtual operator DLLHandle () const;

    public:
        nonvirtual ProcAddress GetProcAddress (const char* procName) const;
        nonvirtual ProcAddress GetProcAddress (const wchar_t* procName) const;

#if !qPlatform_Windows
        /*
         * see linux.die.net/man/3/dlopen for flags
         */
        nonvirtual DLLHandle LoadDLL (const SDKChar* dllName, int flags = RTLD_NOW);
#endif
    private:
        DLLHandle fModule;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DLLSupport.inl"

#endif /*_Stroia_Foundation_Execution_DLLSupport_h_*/
