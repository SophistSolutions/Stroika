/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../HTTP/Exception.h"

namespace Stroika::Foundation::IO::Network::InternetProtocol::IP {

    inline bool SupportIPV4 (IPVersionSupport flag)
    {
        return flag == IPVersionSupport::eIPV4Only or flag == IPVersionSupport::eIPV4AndIPV6;
    }
    inline bool SupportIPV6 (IPVersionSupport flag)
    {
        return flag == IPVersionSupport::eIPV6Only or flag == IPVersionSupport::eIPV4AndIPV6;
    }

}
