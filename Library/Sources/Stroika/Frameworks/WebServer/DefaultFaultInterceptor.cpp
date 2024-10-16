/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"

#include "DefaultFaultInterceptor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
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
    virtual void HandleFault (Message* m, const exception_ptr& e) const noexcept override
    {
        RequireNotNull (m);
        Response& response = m->rwResponse ();
        try {
            try {
                rethrow_exception (e);
            }
            catch (const IO::Network::HTTP::Exception& ee) {
                if (response.responseStatusSent) {
                    DbgTrace ("response failed after sending the status: {}"_f, current_exception ()); // else horse has left the barn
                    response.Abort ();
                }
                else {
                    response.statusAndOverrideReason = make_tuple (ee.GetStatus (), ee.GetReason ());
                    response.contentType             = DataExchange::InternetMediaTypes::kText_PLAIN;
                    response.writeln (Characters::ToString (ee).c_str ());
                }
            }
            catch (...) {
                if (response.responseStatusSent) {
                    DbgTrace ("response failed after sending the status: {}"_f, current_exception ()); // else horse has left the barn
                    response.Abort ();
                }
                else {
                    response.status      = IO::Network::HTTP::StatusCodes::kInternalError;
                    response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                    response.writeln (Characters::ToString (e).c_str ());
                }
            }
        }
        catch (...) {
            DbgTrace (L"Oops! - not good, but nothing todo but burry it: {}"_f, current_exception ()); // else horse has left the barn
            response.Abort ();
        }
    }
    virtual void HandleMessage ([[maybe_unused]] Message* m) const override
    {
    }
};

struct DefaultFaultInterceptor::Rep_Explicit_ : Interceptor::_IRep {
    function<void (Message*, const exception_ptr&)> fHandleFault_;
    Rep_Explicit_ (const function<void (Message*, const exception_ptr&)>& handleFault)
        : fHandleFault_{handleFault}
    {
    }
    virtual void HandleFault (Message* m, const exception_ptr& e) const noexcept override
    {
        RequireNotNull (m);
        fHandleFault_ (m, e);
    }
    virtual void HandleMessage ([[maybe_unused]] Message* m) const override
    {
    }
};

/*
 ********************************************************************************
 ****************** WebServer::DefaultFaultInterceptor **************************
 ********************************************************************************
 */
DefaultFaultInterceptor::DefaultFaultInterceptor ()
    : inherited{make_shared<Rep_> ()}
{
}
DefaultFaultInterceptor::DefaultFaultInterceptor (const function<void (Message*, const exception_ptr&)>& handleFault)
    : inherited{make_shared<Rep_Explicit_> (handleFault)}
{
}
