/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
     *  Caller defines ID_TYPE - the type of record stored to track a user ID (e.g. struct MyID_ { String fEmail; })
     * 
     *  \note  this doesn't generate 401s - but 
     *  \par Example Usage
     *      \code
     *          struct MyID_ { String fEmail; })
     *          ...ConnectionManager fConnectionMgr_{...}
     *          CTOR BODY
     *          {
     *              auto convertRequest2IDToken = [] (Request& request) -> optional<MyID_> {
     *                  // don't throw on bad conversion, but return nullopt - or COULD - 
     *                  // depending on application logic - return an ID with a flag saying bad
     *                  if (optional<String> authHdr = request.headers ().authorization ()) {
     *                      // @todo parse as JWT maybe
     *                      return MyID_{.fEmail = *authHdr };
     *                  }
     *                  return nullopt;
     *              };
     *              fConnectionMgr_.AddInterceptor(CurrentIdentityAuthInterceptor{convertRequest2IDToken}, eEarly);
     *          }
     * 
     *          // THEN - inside any web-service method, the caller may check
     *          if (CurrentIdentityManager<MyID_>::Get ()) {
     *              String email = CurrentIdentityManager<MyID_>::Get ()->fEMail;
     *          }
     *      \endcode
     * 
     */
    template <IIdentityManagerCompatibleID ID_TYPE>
    class CurrentIdentityAuthInterceptor : public Interceptor {
    private:
        using inherited = Interceptor;

    public:
        /**
         */
        CurrentIdentityAuthInterceptor (function<optional<ID_TYPE> (Request&)> cb);

    private:
        struct Rep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Interceptor.inl"

#endif /*_Stroika_Frameworks_Auth_Interceptor_h_*/
