/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"

CompileTimeFlagChecker_HEADER (Stroika::Foundation::IO::Network::Transfer, qHasFeature_WinHTTP, qHasFeature_WinHTTP);

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_WinHTTP
    namespace [[deprecated ("Since Stroika v3.0d4 use WinHTTP::Connection")]] Connection_WinHTTP {
        using namespace Stroika::Foundation::IO::Network::Transfer::WinHTTP::Connection;
    };
#endif

}
