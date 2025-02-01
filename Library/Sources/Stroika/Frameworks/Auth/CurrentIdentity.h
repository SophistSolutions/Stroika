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
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::Auth {

    using namespace Stroika::Foundation;

    /**
     * struct AuthenticatedIdentity {
     *     String fEMail;  // for now - lets assume that's our identity - what we extract from JWT
     * };
     * 
     *  OR
     * 
     * struct AuthenticatedIdentity {
     *     String fBearerToken;
     * 
     *      optional<MoreInfo> PeekAtDerivedInfo () const;
     * };
     * then use optional<AuthenticatedIdentity> as arg to IIdentityManagerCompatibleID/CurrentIdentityManager
     */
    template <typename T>
    concept IIdentityManagerCompatibleID = default_initializable<T> and requires (T t) {
        { static_cast<bool> (t) } -> Common::Boolean_testable;
        static_cast<bool> (T{}) == false;
    };
    static_assert (IIdentityManagerCompatibleID<optional<std::string>>);

    /**
     *  \brief static/thread_local storage of the some notion of identity, which can be used to 'pass data' to functions
     *         without explicit parameters.
     * 
     *  Intended use is in webserver capturing info from auth headers in interceptors, and storing for use in actual
     *  Route callbacks.
     * 
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
     * 
     *  \see also Stroika::Frameworks::Auth::CurrentIdentityAuthInterceptor - maybe better way to 'Establish' context auth values
     * 
     */
    template <IIdentityManagerCompatibleID ID_OBJ>
    struct CurrentIdentityManager {

        /**
         */
        using IDType = ID_OBJ;

        /**
          * \brief sets the current ID to argument value -
          *     \req CurrentIdentityManager<ID_OBJ>::Get () == false
          */
        struct Establish {
            /**
             *     \req CurrentIdentityManager<ID_OBJ>::Get () == false
             */
            Establish (const IDType& id);

            /**
             *     \ens CurrentIdentityManager<ID_OBJ>::Get () == false
             */
            ~Establish ();
        };

        /**
         *  \brief if no identity set with Establish, and maybe even if it has been set, Get() == false
         */
        static IDType Get ();

        /**
         *  \brief if no identity set with Establish, and maybe even if it has been set, Get() == false
         */
        static void Set (IDType id);

        /**
         */
        static void clear ();

    private:
        static inline thread_local IDType sCurrent_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CurrentIdentity.inl"

#endif /*_Stroika_Frameworks_Auth_CurrentIdentity_h_*/
