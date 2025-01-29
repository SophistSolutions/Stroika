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
    struct CurrentIdentityAuthInterceptor::Rep_ : Interceptor::_IRep {
        function<optional<ID_TYPE> (Frameworks::WebServer::Request&)> fCallback_;

        Rep_ (function<optional<ID_TYPE> (Frameworks::WebServer::Request&)> cb)
            : fCallback_{cb}
        {
        }
        virtual void HandleFault (Message& m, [[maybe_unused]] const exception_ptr& e) const noexcept override
        {
            CurrentIdentityManager<ID_OBJ>::clear ();
        }
        virtual void HandleMessage (Message& m) const override
        {
            if (optional<ID_TYPE> oId = fCallback_(m->rwRequest()) {
                CurrentIdentityManager<ID_OBJ>::Set (*oId);
            }
        }
        virtual void CompleteNormally (Message& m) const override
        {
            CurrentIdentityManager<ID_OBJ>::clear ();
        }
    };
    template <IIdentityManagerCompatibleID ID_TYPE>
    CurrentIdentityAuthInterceptor::CurrentIdentityAuthInterceptor (function<optional<ID_TYPE> (Frameworks::WebServer::Request&)> cb)
        : inherited{make_shared<Rep_> (cb)}
    {
    }

}
