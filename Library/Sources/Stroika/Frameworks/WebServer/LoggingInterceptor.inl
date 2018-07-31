/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_LoggingInterceptor_inl_
#define _Stroika_Frameworks_WebServer_LoggingInterceptor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include "../../Foundation/Containers/Common.h"

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    inline ILogHandler::MessageInstance::MessageInstance (Message* m, Time::DurationSecondsType startedAt)
        : fMessage (m)
        , fStartedAt (startedAt)
    {
    }

}

#endif /*_Stroika_Frameworks_WebServer_LoggingInterceptor_inl_*/
