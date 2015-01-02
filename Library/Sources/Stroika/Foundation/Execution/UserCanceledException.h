/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_UserCanceledException_h_
#define _Stroia_Foundation_Execution_UserCanceledException_h_   1

#include    "../StroikaPreComp.h"

#include    "SilentException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            // mostly treat the same as SilentException
            class   UserCanceledException : public SilentException {
            };


            template    <>
            void    _NoReturn_  DoThrow (const UserCanceledException& e2Throw);


        }
    }
}



#endif  /*_Stroia_Foundation_Execution_UserCanceledException_h_*/
