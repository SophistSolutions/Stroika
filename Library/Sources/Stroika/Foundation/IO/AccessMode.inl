/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<IO::AccessMode> DefaultNames<IO::AccessMode>::k{{{
        {IO::AccessMode::eNoAccess, L"No-Access"},
        {IO::AccessMode::eRead, L"Read"},
        {IO::AccessMode::eWrite, L"Write"},
        {IO::AccessMode::eReadWrite, L"Read-Write"},
    }}};
}
