/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_h_
#define _Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_h_ 1

#include "../StroikaPreComp.h"

namespace Stroika::Foundation::Common {

    /**
     */
    class EmptyObjectForConstructorSideEffect {
    public:
        /**
         *  \brief Sometimes you want to sequence some code around mem-initializers. SOMETIMES - if those mem-initializers take paraemters, you can
         *         squeeze this in with (fake-value-for-side-effect,real-value) arguments.
         *         But other times that doesn't work out, and this trick solves the same problem, perhaps more cleanly.
         *
         *  \par Example Usage
         *      \code
         *          struct foo { ...
         *              // Mark the activity 'constructing WSAPI webserver' as surrounding the construction of the ConnectionManager object
         *              static constexpr Activity                                      kContructing_WSAPI_WebServer_{L"constructing WSAPI webserver"sv};
         *              optional<DeclareActivity<Activity<>>>                          fEstablishActivity1_{&kContructing_WSAPI_WebServer_};
         *              ConnectionManager                                              fWSConnectionMgr_;
         *              [[NO_UNIQUE_ADDRESS_ATTR]] EmptyObjectForConstructorSideEffect fIgnore1_{[this]() { fEstablishActivity1_.reset (); }};
         *              ...
         *          };
         *      \endcode
         */
        template <typename CALL>
        EmptyObjectForConstructorSideEffect (const CALL& c);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EmptyObjectForConstructorSideEffect.inl"

#endif /*_Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_h_*/