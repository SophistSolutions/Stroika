/*/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Concrete_ElfResourceManager_h_
#define _Stroika_Foundation_Execution_Resources_Concrete_ElfResourceManager_h_ 1

#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/String.h"
#include    "../../../Configuration/Common.h"

#include    "../Manager.h"

/**
 *  \file
 *
 * TODO:
 *
 *      @todo   Research elf linker option, and think through how to use elf resource trick I used for
 *              monitoring app here - to provide a resrouce file like mechanism.
 *
 *      @todo   NOTHING implemented so far
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {
                namespace   Concrete {

                    /**
                     * NYI, but this will use the ELF gcc ld feature of letting you bind arbitrary files into an executable...
                     */
                    class   ElfResourceManager : public Manager {
                    public:
                        ElfResourceManager ();
                    };

                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Resources_Concrete_ElfResourceManager_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ElfResourceManager.inl"
