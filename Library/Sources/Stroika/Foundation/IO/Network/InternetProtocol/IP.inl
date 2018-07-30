/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_IP_inl_
#define _Stroika_Foundation_IO_Network_InternetProtocol_IP_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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

#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_IP_inl_*/
