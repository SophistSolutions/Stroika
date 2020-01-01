/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Interceptor.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ***************************** Interceptor::MyRep_ ******************************
 ********************************************************************************
 */
class Interceptor::MyRep_ : public Interceptor::_IRep {
    function<void (Message*)>                         fHandleMessage_;
    function<void (Message*, const exception_ptr& e)> fHandleFault_;

public:
    MyRep_ (const function<void (Message*)>& handleMessage, const function<void (Message*, const exception_ptr&)>& handleFault)
        : fHandleMessage_ (handleMessage)
        , fHandleFault_ (handleFault)
    {
    }
    virtual void HandleFault (Message* m, const exception_ptr& e) noexcept override
    {
        fHandleFault_ (m, e);
    }
    virtual void HandleMessage (Message* m) override
    {
        fHandleMessage_ (m);
    }
};

/*
 ********************************************************************************
 ***************************** WebServer::Interceptor ***************************
 ********************************************************************************
 */
Interceptor::Interceptor (const function<void (Message*)>& handleMessage, const function<void (Message*, const exception_ptr&)>& handleFault)
    : Interceptor (make_shared<MyRep_> (handleMessage, handleFault))
{
}

/*
 ********************************************************************************
 *********************** WebServer::Interceptor::_IRep **************************
 ********************************************************************************
 */
void Interceptor::_IRep::CompleteNormally (Message* /*m*/)
{
}
