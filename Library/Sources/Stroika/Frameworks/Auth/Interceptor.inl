/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Frameworks::Auth {

    /*
     ********************************************************************************
     ***************** Auth::CurrentIdentityAuthInterceptor *************************
     ********************************************************************************
     */
    template <IIdentityManagerCompatibleID ID_TYPE>
    struct CurrentIdentityAuthInterceptor<ID_TYPE>::Rep_ : Interceptor::_IRep {
        function<optional<ID_TYPE> (WebServer::Request&)> fCallback_;

        Rep_ (function<optional<ID_TYPE> (WebServer::Request&)> cb)
            : fCallback_{cb}
        {
        }
        virtual void HandleFault ([[maybe_unused]] WebServer::Message& m, [[maybe_unused]] const exception_ptr& e) const noexcept override
        {
            CurrentIdentityManager<ID_TYPE>::clear ();
        }
        virtual void HandleMessage (WebServer::Message& m) const override
        {
            if (optional<ID_TYPE> oId = fCallback_ (m.rwRequest ())) {
                CurrentIdentityManager<ID_TYPE>::Set (*oId);
            }
        }
        virtual void CompleteNormally ([[maybe_unused]] WebServer::Message& m) const override
        {
            CurrentIdentityManager<ID_TYPE>::clear ();
        }
        virtual Characters::String ToString () const override
        {
            return "CurrentIdentityAuthInterceptor"sv;
        }
    };
    template <IIdentityManagerCompatibleID ID_TYPE>
    CurrentIdentityAuthInterceptor<ID_TYPE>::CurrentIdentityAuthInterceptor (function<optional<ID_TYPE> (WebServer::Request&)> cb)
        : inherited{make_shared<Rep_> (cb)}
    {
    }

}
