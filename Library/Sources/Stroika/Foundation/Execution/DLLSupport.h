/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_DLLSupport_h_
#define _Stroia_Foundation_Execution_DLLSupport_h_ 1

#include "../StroikaPreComp.h"

#include <filesystem>

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
     *  @todo - probably should have DLL-name embedded in exception message on failure and as a field of DLLException
     *        or maybe use 'declareactivity' while loading DLL?
     */
    class DLLLoader {
    public:
        /**
         *  Throws on failure.
         *  For POSIX, default flags=RTLD_NOW
         */
        DLLLoader (const SDKChar* dllName);
        DLLLoader (const SDKChar* dllName, const vector<filesystem::path>& searchPath);
#if qPlatform_POSIX
        DLLLoader (const SDKChar* dllName, int flags);
        DLLLoader (const SDKChar* dllName, const vector<filesystem::path>& searchPath, int flags);
#endif
        DLLLoader (const DLLLoader&) = delete;
        ~DLLLoader ();
        DLLLoader& operator= (const DLLLoader&) = delete;

    public:
        nonvirtual operator DLLHandle () const;

    public:
        /**
         *  Throws on failure.
         */
        nonvirtual ProcAddress GetProcAddress (const char* procName) const;
        nonvirtual ProcAddress GetProcAddress (const wchar_t* procName) const;

#if !qPlatform_Windows
        /*
         * see linux.die.net/man/3/dlopen for flags
         */
        [[deprecated ("Use DLLLoader::CTOR since Stroika v2.1b2")]] DLLHandle LoadDLL (const SDKChar* dllName, int flags = RTLD_NOW);
#endif

    private:
        DLLHandle fModule_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DLLSupport.inl"

#endif /*_Stroia_Foundation_Execution_DLLSupport_h_*/
