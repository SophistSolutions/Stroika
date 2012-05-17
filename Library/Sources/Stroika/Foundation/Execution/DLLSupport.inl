/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_DLLSupport_inl_
#define _Stroia_Foundation_Execution_DLLSupport_inl_    1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            //  class   DLLLoader
            inline  DLLLoader::operator DLLHandle () {
                EnsureNotNull (fModule);
                return fModule;
            }
            inline  ProcAddress DLLLoader::GetProcAddress (const char* procName) const {
                AssertNotNull (fModule);
                RequireNotNull (procName);
#if     qPlatform_Windows
                return ::GetProcAddress (fModule, procName);
#else
                ProcAddress addr = dlsym (fModule, procName);
                if (addr == nullptr) {
                    dlerror (); // clear any old error
                    addr = dlsym (fModule, procName);
                    // interface seems to be defined only for char*, not wide strings: may need to map procName as well
                    const char* err = dlerror ();
                    if (err != nullptr) {
                        Execution::DoThrow (DLLException (err));
                    }
                }
                return addr;
#endif
            }
            inline  ProcAddress DLLLoader::GetProcAddress (const wchar_t* procName) const {
                AssertNotNull (fModule);
                RequireNotNull (procName);
                return GetProcAddress (Characters::WideStringToASCII (procName).c_str ());
            }
#if     !qPlatform_Windows
            inline  DLLException::DLLException (const char* message) :
                StringException (Characters::NarrowSDKStringToWide (message)) {
            }
#endif
        }
    }
}
#endif  /*_Stroia_Foundation_Execution_DLLSupport_inl_*/
