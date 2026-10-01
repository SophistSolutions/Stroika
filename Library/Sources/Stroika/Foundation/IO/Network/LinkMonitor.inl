/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ********************************** LinkMonitor *********************************
     ********************************************************************************
     */

}

namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<IO::Network::LinkMonitor::LinkChange> DefaultNames<IO::Network::LinkMonitor::LinkChange>::k{{{
        {IO::Network::LinkMonitor::LinkChange::eAdded, L"Added"},
        {IO::Network::LinkMonitor::LinkChange::eRemoved, L"Removed"},
    }}};
}
