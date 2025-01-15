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
    inline CurrentIdentityManager<ID_OBJ>::Establish::Establish (const ID_OBJ& id)
    {
        Require (!sCurrent_);
        sCurrent_ = id;
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::~Establish ()
    {
        sCurrent_ = ID_OBJ{};
        Ensure (!sCurrent_);
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline ID_OBJ CurrentIdentityManager<ID_OBJ>::Get ()
    {
        return sCurrent_;
    }

}
