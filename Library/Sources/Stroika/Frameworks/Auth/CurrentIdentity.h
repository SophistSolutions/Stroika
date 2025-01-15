/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Auth_CurrentIdentity_h_
#define _Stroika_Frameworks_Auth_CurrentIdentity_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"

/**
 *  \file
*  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Frameworks::Auth {

    using namespace Stroika::Foundation;


    // @todo IMPORTANT TODO - MUST BE INTEGRATED INTO WEBSERVER ROUTER automatically, so interceptor sets, not each route handler!!!


    // struct AuthenticatedIdentity {
    //     String fEMail;  // for now - lets assume that's our identity - what we extract from JWT
    // };

    /**
     *  \brief manage a 'thread_local' 'current-id' - typically for use in threaded applications where IDs might come from
     *         outside, like web-services
     * 
     *  \par Example Usage:
     *      using ID_OBJ = optional<String>;    // just the raw auth-header
     * 
     *      [] (Request& rq, Response& rs) {
     *          CurrentIdentityManager<ID_OBJ>::Establish curID {rq.headers().authorization ()};
     *          fWSImpl.method(decoded_args_from_req);  // internally peeks at CurrentIdentityManager<ID_OBJ>::Get ()
     *          ...
     *      }
     */
    template <typename T>
    concept IIdenityManagerCompatibleID =
        Common::Boolean_testable<T> and constructible_from<T>; // default_constructible, convertible_to<bool>, @todo!!! static_assert (ID_OBJ{} == false);

    // want this to be close to convertible_to<bool> - but must refine the concept - not right
    template <IIdenityManagerCompatibleID ID_OBJ>
    struct CurrentIdentityManager {

        /**
          * \brief sets the current ID to argument value -
          *     \req CurrentIdentityManager<ID_OBJ>::Get () == false
          */
        struct Establish {
            /**
             *     \req CurrentIdentityManager<ID_OBJ>::Get () == false
             */
            Establish (const ID_OBJ& id);

            /**
             *     \ens CurrentIdentityManager<ID_OBJ>::Get () == false
             */
            ~Establish ();
        };

        /**
         *  \brief if no identity set with Establish, and maybe even if it has been set, Get() == false
         */
        static ID_OBJ Get ();

    private:
        static inline thread_local ID_OBJ sCurrent_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CurrentIdentity.inl"

#endif /*_Stroika_Frameworks_Auth_CurrentIdentity_h_*/
