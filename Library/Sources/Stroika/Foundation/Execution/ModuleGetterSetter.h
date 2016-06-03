/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_h_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"



/**
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO
 *
 *      @todo   NEEDS EXAMPLE - better docs
 *      @todo   cleanup operaotr-> crap - really sloppy impl!!!!
 *      @todo   Consider addiing Update method (with func arg taking T by reference?/optr or taking and returing
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  \breif  Helper to define synchonized, lazy constructed, module initialization (intended to work with DataExchange::OptionFile)
             *
             * Features:
             *      o   Simple API - get/set
             *      o   auto intrindically threadsafe
             *      o   Init underling object on first access, so easy to declare globally (static init) and less worry about running before main
             */
            template    <typename T, typename IMPL>
            struct  ModuleGetterSetter {
                /**
                 */
                nonvirtual  T       Get ();

                /**
                 */
                nonvirtual  void    Set (const T& v);

            private:
                Execution::Synchronized<Memory::Optional<IMPL>>   fIndirect_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ModuleGetterSetter.inl"

#endif  /*_Stroika_Foundation_Execution_ModuleGetterSetter_h_*/
