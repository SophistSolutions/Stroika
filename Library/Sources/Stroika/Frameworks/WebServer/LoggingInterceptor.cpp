/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"

#include "LoggingInterceptor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ******************************* WebServer::ILogHandler *************************
 ********************************************************************************
 */
shared_ptr<ILogHandler::MessageInstance> ILogHandler::Started (Message* m)
{
    return make_shared<MessageInstance> (m, Time::GetTickCount ());
}

/*
 ********************************************************************************
 *********************** WebServer::LoggingInterceptor **************************
 ********************************************************************************
 */
struct LoggingInterceptor::Rep_ : Interceptor::_IRep {
    Rep_ (const shared_ptr<ILogHandler>& logger)
        : fLogger_ (logger)
    {
    }
    virtual void HandleFault (Message* m, [[maybe_unused]] const exception_ptr& e) noexcept override
    {
        RequireNotNull (m);
        shared_ptr<ILogHandler::MessageInstance> logID;
        {
            auto rwLock = fOngoingMessages_.rwget ();
            Assert (rwLock->Lookup (m));
            logID = *rwLock->Lookup (m);
            rwLock->Remove (m);
        }
        fLogger_->Completed (logID);
    }
    virtual void HandleMessage (Message* m) override
    {
        shared_ptr<ILogHandler::MessageInstance> logID = fLogger_->Started (m);
        Assert (not fOngoingMessages_->Lookup (m).has_value ());
        fOngoingMessages_.rwget ().rwref ().Add (m, logID);
    }
    virtual void CompleteNormally (Message* m) override
    {
        shared_ptr<ILogHandler::MessageInstance> logID;
        {
            auto rwLock = fOngoingMessages_.rwget ();
            Assert (rwLock->Lookup (m));
            logID = *rwLock->Lookup (m);
            rwLock->Remove (m);
        }
        fLogger_->Completed (logID);
    }
    shared_ptr<ILogHandler>                                                              fLogger_;
    Execution::Synchronized<Mapping<Message*, shared_ptr<ILogHandler::MessageInstance>>> fOngoingMessages_;
};

LoggingInterceptor::LoggingInterceptor (const shared_ptr<ILogHandler>& logger)
    : inherited (make_shared<Rep_> (logger))
{
}
