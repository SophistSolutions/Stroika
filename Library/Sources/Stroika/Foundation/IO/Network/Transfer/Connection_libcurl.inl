/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::IO::Network::Transfer, qHasFeature_LibCurl, qHasFeature_LibCurl);

#if qHasFeature_LibCurl
namespace Stroika::Foundation::IO::Network::Transfer::LibCurl {

    inline void ThrowIfError (CURLcode status)
    {
        if (status != CURLE_OK) [[unlikely]] {
            Execution::Throw (Execution::SystemErrorException<>{status, error_category ()});
        }
    }

}
#endif

#if qHasFeature_LibCurl
namespace Stroika::Foundation::IO::Network::Transfer {
    [[deprecated ("Since Stroika v3.0d4")]] inline const std::error_category& LibCurl_error_category () noexcept
    {
        return LibCurl::error_category ();
    }

    /**
     * throw Exception () type iff the status indicates a real exception code. This MAY not throw an exception of type LibCurlException,
     * but MAY map to any other exception type
     */
    [[deprecated ("Since Stroika v3.0d4")]] inline void ThrowIfError (CURLcode status)
    {
        LibCurl::ThrowIfError (status);
    }

    namespace [[deprecated ("Since Stroika v3.0d4 use LibCurl::Connection")]] Connection_LibCurl {
        using namespace Stroika::Foundation::IO::Network::Transfer::LibCurl::Connection;

    };
}
#endif
