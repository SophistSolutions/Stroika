/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_DLLSupport_h_
#define _Stroia_Foundation_Execution_DLLSupport_h_  1

#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#else
#include    <dlfcn.h>
#endif

#include    "../Characters/TString.h"
#include    "../Execution/StringException.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            using   Characters::TString;
            using   Characters::TChar;

#if     qPlatform_Windows
            typedef HMODULE DLLHandle;
            typedef FARPROC ProcAddress;
#else
            typedef void*   DLLHandle;
            typedef void*   ProcAddress;
#endif

#if     !qPlatform_Windows
            class   DLLException : public StringException {
            public:
                DLLException (const char* message);
            };
#endif

            class   DLLLoader {
            public:
                DLLLoader (const TChar* dllName);
                DLLLoader (const TChar* dllName, const vector<TString>& searchPath);
                ~DLLLoader ();

            public:
                operator DLLHandle ();

            public:
                nonvirtual  ProcAddress GetProcAddress (const char* procName) const;
                nonvirtual  ProcAddress GetProcAddress (const wchar_t* procName) const;

#if     !qPlatform_Windows
                // ssw: not sure what to set for flags here, or if we should leave up to user
                // see linux.die.net/man/3/dlopen
                nonvirtual  DLLHandle   LoadDLL (const TChar* dllName, int flags = RTLD_NOW | RTLD_GLOBAL);
#endif
            private:
                DLLHandle   fModule;
            };

        }
    }
}
#endif  /*_Stroia_Foundation_Execution_DLLSupport_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DLLSupport.inl"

