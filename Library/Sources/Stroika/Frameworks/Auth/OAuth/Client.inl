/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::Auth::OAuth {

    /*
     ********************************************************************************
     ************************************* Fetcher **********************************
     ********************************************************************************
     */
    inline Fetcher::Fetcher (const ProviderConfiguration& providerConfiguration)
        : Fetcher{providerConfiguration, Options{}}
    {
    }

}
