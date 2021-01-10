/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddressRange_h_
#define _Stroika_Foundation_IO_Network_InternetAddressRange_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Traversal/DiscreteRange.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    /**
     *  \par Example Usage
     *      \code
     *          for (InternetAddress ia : CIDR{ L"192.168.243.0/24" }.GetRange ()) {
     *               print (ia);
     *          }
     *      \endcode
     */
    using InternetAddressRange [[deprecated ("Since Stroika 2.1b8, use DiscreteRange<InternetAddress>")]] = Traversal::DiscreteRange<InternetAddress>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_IO_Network_InternetAddressRange_h_*/
