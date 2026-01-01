/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ************************* ILogHandler::MessageInstance *************************
     ********************************************************************************
     */
    inline ILogHandler::MessageInstance::MessageInstance (Message& m, Time::TimePointSeconds startedAt)
        : fMessage{m}
        , fStartedAt{startedAt}
    {
    }

}
