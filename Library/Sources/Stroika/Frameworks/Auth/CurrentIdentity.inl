/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Frameworks::Auth {

    /*
     ********************************************************************************
     ************************* Auth::CurrentIdentityManager *************************
     ********************************************************************************
     */
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::Establish (const IDType& id)
    {
        Require (!sCurrent_);
        sCurrent_ = id;
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::~Establish ()
    {
        sCurrent_ = IDType{};
        Ensure (!sCurrent_);
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline auto CurrentIdentityManager<ID_OBJ>::Get () -> IDType
    {
        return sCurrent_;
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline void CurrentIdentityManager<ID_OBJ>::Set (IDType id)
    {
        sCurrent_ = id;
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline void CurrentIdentityManager<ID_OBJ>::clear ()
    {
        sCurrent_ = ID_OBJ{};
    }

}
