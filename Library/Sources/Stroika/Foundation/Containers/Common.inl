/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <algorithm>

#include "Stroika/Foundation/Math/Common.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************************* Start ************************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline typename CONTAINER::value_type* Start (CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0];
    }
    template <typename CONTAINER>
    inline const typename CONTAINER::value_type* Start (const CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0];
    }

    /*
     ********************************************************************************
     ************************************* End **************************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline typename CONTAINER::value_type* End (CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0] + cnt;
    }
    template <typename CONTAINER>
    inline const typename CONTAINER::value_type* End (const CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0] + cnt;
    }

    /*
     ********************************************************************************
     ************************* AddOrExtendOrReplaceMode *****************************
     ********************************************************************************
     */
    constexpr AddOrExtendOrReplaceMode::AddOrExtendOrReplaceMode (uint8_t v) noexcept
        : fValue_{v}
    {
        Require (v <= 3);
    }
    constexpr AddOrExtendOrReplaceMode::AddOrExtendOrReplaceMode (AddReplaceMode v) noexcept
        : AddOrExtendOrReplaceMode{static_cast<uint8_t> (v)}
    {
    }
    constexpr AddOrExtendOrReplaceMode::operator uint8_t () const noexcept
    {
        return fValue_;
    }
    inline constexpr AddOrExtendOrReplaceMode AddOrExtendOrReplaceMode::eAddIfMissing{AddReplaceMode::eAddIfMissing};
    inline constexpr AddOrExtendOrReplaceMode AddOrExtendOrReplaceMode::eAddReplaces{AddReplaceMode::eAddReplaces};
    inline constexpr AddOrExtendOrReplaceMode AddOrExtendOrReplaceMode::eAddExtras{2};
    inline constexpr AddOrExtendOrReplaceMode AddOrExtendOrReplaceMode::eDuplicatesRejected{3};

}
