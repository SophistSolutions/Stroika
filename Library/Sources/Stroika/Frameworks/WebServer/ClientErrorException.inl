/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_ClientErrorException_inl_
#define _Stroika_Frameworks_WebServer_ClientErrorException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Debug/Assertions.h"

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            /*
             ********************************************************************************
             ******************************* ClientErrorException ***************************
             ********************************************************************************
             */
            inline ClientErrorException::ClientErrorException (Status httpStatus)
                : inherited (httpStatus)
            {
                Require (400 <= httpStatus and httpStatus < 500);
            }
            inline ClientErrorException::ClientErrorException (const String& message, Status httpStatus)
                : inherited (httpStatus, message)
            {
                Require (400 <= httpStatus and httpStatus < 500);
            }
        }
    }
}
#endif /*_Stroika_Frameworks_WebServer_ClientErrorException_inl_*/
