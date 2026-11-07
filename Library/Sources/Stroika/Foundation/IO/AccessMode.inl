/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO {

    /*
     ********************************************************************************
     ********************************* IO::AccessMode *******************************
     ********************************************************************************
     */
    constexpr AccessMode operator& (AccessMode l, AccessMode r)
    {
        return AccessMode (static_cast<int> (l) & static_cast<int> (r));
    }
    constexpr AccessMode operator| (AccessMode l, AccessMode r)
    {
        return AccessMode (static_cast<int> (l) | static_cast<int> (r));
    }

}

namespace Stroika::Foundation::IO {
    Common::StroikaNaming EnumNamingRule (AccessMode);
}
