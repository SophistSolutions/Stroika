/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************* Execution::SetCPUAffinityQuietly ***************************
     ********************************************************************************
     */
    inline bool SetCPUAffinityQuietly (const LogicalCPUCoreSet& cores) noexcept
    {
        // The if constexpr matters: where affinity is unsupported this compiles to 'return false', so a
        // caller on macOS pays nothing - not even a try/catch frame - for asking.
        if constexpr (kCPUAffinitySupported) {
            try {
                SetCPUAffinity (cores);
                return true;
            }
            catch (...) {
                return false;
            }
        }
        else {
            return false;
        }
    }

}
