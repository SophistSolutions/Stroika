/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Auth_Interceptor_h_
#define _Stroika_Frameworks_Auth_Interceptor_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"

#include "Stroika/Frameworks/Auth/CurrentIdentity.h"
#include "Stroika/Frameworks/WebServer/Interceptor.h"
#include "Stroika/Frameworks/WebServer/Request.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::Auth {

    using namespace Stroika::Foundation;

    using Frameworks::WebServer::Request;
    using Stroika::Frameworks::WebServer::Interceptor;

    /**
     *  Add Interceptor to ConnectionManager.
     * 
     *  Must be templated on TYPE of object to stick into CurrentIdentity.
     */

    /**
     *  \brief Interceptor added to WebServer::ConnectionManager to translate Authorization: headers (parse them) into thread_local ID variable accessible by webservice methods
     *
     *  Caller defines ID_TYPE - the type of record stored to track a user ID; type: typically like this
     *  \par Example Usage
     *      \code
     *          struct MyID_ { 
     *              String fBearerToken;
     *               MoreUserInfo GetInfoDerivedFromBearerOrThrow401 (); });
     *          };
     *      \enccode
     * 
     *  \note  The Interceptor doesn't generate 401s, populates a thread-local data structure CurrentIdentityAuthInterceptor with the auth-token
     * 
     *  \par Example Usage
     *      \code
     *          ...ConnectionManager fConnectionMgr_{...}
     *          CTOR BODY
     *          {
     *              auto convertAuthHeaderToIDObject  = [] (Request& request) -> optional<MyID_> {
     *                  if (auto authHeader = request.headers ().authorization (); authHeader and authHeader->StartsWith ("Bearer "sv)) {
     *                      return  WebServiceIdentity{.fBearerToken = authHeader->SubString (7).Trim ()};
     *                  }
     *                  return nullopt;
     *              };
     *              fConnectionMgr_.AddInterceptor (CurrentIdentityAuthInterceptor{convertAuthHeaderToIDObject}, ConnectionManager::ePrependsToEarly);
     *          }
     * 
     *          // THEN - inside any web-service method, the caller may check
     *          if (optional<MyID_> oid = CurrentIdentityManager<optional<MyID_>>::Get ()) {
     *              String email = oid->GetInfoDerivedFromBearerOrThrow401 ().fEmail;
     *          }
     *      \endcode
     */
    template <IIdentityManagerCompatibleID ID_TYPE>
    class CurrentIdentityAuthInterceptor : public Interceptor {
    private:
        using inherited = Interceptor;

    public:
        /**
         */
        CurrentIdentityAuthInterceptor (function<ID_TYPE (Request&)> cb);

    private:
        struct Rep_;
    };
    template <invocable<Request&> FN>
    CurrentIdentityAuthInterceptor (FN) -> CurrentIdentityAuthInterceptor<invoke_result_t<FN, Request&>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Interceptor.inl"

#endif /*_Stroika_Frameworks_Auth_Interceptor_h_*/
