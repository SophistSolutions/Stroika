/*/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Concrete_WindowsResourceManager_h_
#define _Stroika_Foundation_Execution_Resources_Concrete_WindowsResourceManager_h_ 1

#include    "../../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Characters/String.h"
#include    "../../../Configuration/Common.h"

#include    "../Manager.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {
                namespace   Concrete {


                    /**
                     * Provide ResourceManager access to a Windows resource section of a Windows executable.
                     */
                    class   WindowsResourceManager : public Manager {
                    public:
                        WindowsResourceManager (HMODULE hModule);

                    private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                    public:
#endif
                        class   IRep_;
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "WindowsResourceManager.inl"

#endif  /*_Stroika_Foundation_Execution_Resources_Concrete_WindowsResourceManager_h_*/
