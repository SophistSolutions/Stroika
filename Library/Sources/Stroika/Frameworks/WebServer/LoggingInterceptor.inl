/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    inline ILogHandler::MessageInstance::MessageInstance (Message* m, Time::TimePointSeconds startedAt)
        : fMessage{m}
        , fStartedAt{startedAt}
    {
    }

}
