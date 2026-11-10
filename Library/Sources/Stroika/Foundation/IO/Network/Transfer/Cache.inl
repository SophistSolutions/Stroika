/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::Transfer::Cache {

    /// <summary>
    //////////////////////////// DEPRECATED BELOW.../////////////////////////////
    /// </summary>

    using DefaultOptions [[deprecated ("Since Stroika v3.0d26 use Cache::Options")]] = Options;
    [[deprecated ("Since Stroika v3.0d26 use Cache::New")]] inline Ptr CreateDefault (const Options& options = {})
    {
        return New (options);
    }

}
