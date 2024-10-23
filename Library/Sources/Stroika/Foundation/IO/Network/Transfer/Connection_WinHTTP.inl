/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"

CompileTimeFlagChecker_HEADER (Stroika::Foundation::IO::Network::Transfer, qStroika_HasComponent_WinHTTP, qStroika_HasComponent_WinHTTP);

namespace Stroika::Foundation::IO::Network::Transfer {

#if qStroika_HasComponent_WinHTTP
    namespace [[deprecated ("Since Stroika v3.0d4 use WinHTTP::Connection")]] Connection_WinHTTP {
        using namespace Stroika::Foundation::IO::Network::Transfer::WinHTTP::Connection;
    };
#endif

}
