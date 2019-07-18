/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"

#include "DefaultFaultInterceptor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ************************* WebServer::Router::Rep_ ******************************
 ********************************************************************************
 */
struct DefaultFaultInterceptor::Rep_ : Interceptor::_IRep {
    Rep_ () = default;
    virtual void HandleFault (Message* m, const exception_ptr& e) noexcept override
    {
        RequireNotNull (m);
        Response* response = m->PeekResponse ();
        try {
            try {
                rethrow_exception (e);
            }
            catch (const IO::Network::HTTP::Exception& ee) {
                response->SetStatus (ee.GetStatus (), ee.GetReason ());
                response->writeln (Characters::ToString (ee).c_str ());
                response->SetContentType (DataExchange::InternetMediaTypes::kText_PLAIN);
            }
            catch (...) {
                response->SetStatus (IO::Network::HTTP::StatusCodes::kInternalError);
                response->writeln (Characters::ToString (e).c_str ());
                response->SetContentType (DataExchange::InternetMediaTypes::kText_PLAIN);
            }
        }
        catch (...) {
            DbgTrace (L"Oops! - not good, but nothing todo but burry it."); // not assert failure cuz could be out of memory
        }
    }
    virtual void HandleMessage ([[maybe_unused]] Message* m) override
    {
    }
};

struct DefaultFaultInterceptor::Rep_Explicit_ : Interceptor::_IRep {
    function<void (Message*, const exception_ptr&)> fHandleFault_;
    Rep_Explicit_ (const function<void (Message*, const exception_ptr&)>& handleFault)
        : fHandleFault_ (handleFault)
    {
    }
    virtual void HandleFault (Message* m, const exception_ptr& e) noexcept override
    {
        RequireNotNull (m);
        fHandleFault_ (m, e);
    }
    virtual void HandleMessage ([[maybe_unused]] Message* m) override
    {
    }
};

/*
 ********************************************************************************
 ****************** WebServer::DefaultFaultInterceptor **************************
 ********************************************************************************
 */
DefaultFaultInterceptor::DefaultFaultInterceptor ()
    : inherited (make_shared<Rep_> ())
{
}
DefaultFaultInterceptor::DefaultFaultInterceptor (const function<void (Message*, const exception_ptr&)>& handleFault)
    : inherited (make_shared<Rep_Explicit_> (handleFault))
{
}
