/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/ToString.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** Common::UniqueID ********************************
     ********************************************************************************
     */
    template <typename TAG>
    constexpr UniqueID<TAG>::UniqueID (uint64_t id)
        : fID_{id}
    {
    }
    template <typename TAG>
    inline auto UniqueID<TAG>::New () -> UniqueID
    {
        return UniqueID{++Private_::sUniqueIDLast_};
    }
    template <typename TAG>
    inline Characters::String UniqueID<TAG>::ToString () const
    {
        return Characters::ToString (fID_);
    }

}
