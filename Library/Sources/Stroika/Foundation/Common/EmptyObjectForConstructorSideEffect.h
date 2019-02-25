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
         *  \par Example Usage
         *      \code
         *          static const Activity<>                                        kContructing_WSAPI_WebServer_{L"constructing WSAPI webserver"};
         *          optional<DeclareActivity<Activity<>>>                          fEstablishActivity1_{&kContructing_WSAPI_WebServer_};
         *          ConnectionManager                                              fWSConnectionMgr_;
         *          [[NO_UNIQUE_ADDRESS_ATTR]] EmptyObjectForConstructorSideEffect fIgnore1_{[this]() { fEstablishActivity1_.reset (); }};
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