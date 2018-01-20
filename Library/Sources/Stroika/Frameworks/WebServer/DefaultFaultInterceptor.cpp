/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
                std::rethrow_exception (e);
            }
            catch (const IO::Network::HTTP::Exception& ee) {
                response->SetStatus (ee.GetStatus (), ee.GetReason ());
                response->writeln (Characters::ToString (ee).c_str ());
                response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_CT ());
            }
            catch (...) {
                response->SetStatus (IO::Network::HTTP::StatusCodes::kInternalError);
                response->writeln (Characters::ToString (e).c_str ());
                response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_CT ());
            }
        }
        catch (...) {
            DbgTrace (L"Oops! - not good, but nothing todo but burry it."); // not assert failure cuz could be out of memory
        }
    }
    virtual void HandleMessage (Message* m) override
    {
    }
};

struct DefaultFaultInterceptor::Rep_Explicit_ : Interceptor::_IRep {
#if qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy
    function<void(Message*, const exception_ptr&)> fHandleFault_;
    Rep_Explicit_ (const function<void(Message*, const exception_ptr&)>& handleFault)
        : fHandleFault_ (handleFault)
    {
    }
#else
    function<void(Message*, const exception_ptr&) noexcept> fHandleFault_;
    Rep_Explicit_ (const function<void(Message*, const exception_ptr&) noexcept>& handleFault)
        : fHandleFault_ (handleFault)
    {
    }
#endif
    virtual void HandleFault (Message* m, const exception_ptr& e) noexcept override
    {
        RequireNotNull (m);
        fHandleFault_ (m, e);
    }
    virtual void HandleMessage (Message* m) override
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
#if qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy
DefaultFaultInterceptor::DefaultFaultInterceptor (const function<void(Message*, const exception_ptr&)>& handleFault)
#else
DefaultFaultInterceptor::DefaultFaultInterceptor (const function<void(Message*, const exception_ptr&) noexcept>& handleFault)
#endif
    : inherited (make_shared<Rep_Explicit_> (handleFault))
{
}
