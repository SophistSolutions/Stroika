/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_SilentException_h_
#define _Stroia_Foundation_Execution_SilentException_h_ 1

#include    "../StroikaPreComp.h"

#include    "Exceptions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            // Throw this when an error has already been reported - so that it isn't reported again
            class   SilentException {
            };


            template    <>
            void    _NoReturn_  DoThrow (const SilentException& e2Throw);


        }
    }
}



#endif  /*_Stroia_Foundation_Execution_SilentException_h_*/
