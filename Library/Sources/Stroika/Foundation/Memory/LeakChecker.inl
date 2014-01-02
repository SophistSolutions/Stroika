/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_LeakChecker_inl_
#define _Stroika_Foundation_Memory_LeakChecker_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/ModuleInit.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            // Module initialization
            namespace   Private_ {
                struct  ModuleData_ {
                    ModuleData_ ();
                    ~ModuleData_ ();
                };
            }


        }
    }
}


namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Memory::Private_::ModuleData_>   _Stroika_Foundation_Memory_LeakChecker_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Memory_LeakChecker_inl_*/
