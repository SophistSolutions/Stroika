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
    template <convertible_to<bool> ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::Establish (const ID_OBJ& id)
    {
        Require (sCurrent_ == false);
        sCurrent_ = id;
    }
    template <convertible_to<bool> ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::~Establish ()
    {
        Require (sCurrent_ == false);
        sCurrent_ = id;
    }
    template <convertible_to<bool> ID_OBJ>
    inline ID_OBJ CurrentIdentityManager<ID_OBJ>::Get ()
    {
        return sCurrent_;
    }
    
}
