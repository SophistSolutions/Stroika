/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Device_h_
#define _Stroika_Frameworks_UPnP_Device_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/*
 * TODO:
 */

namespace Stroika::Frameworks::UPnP {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Foundation::IO::Network::URI;

    /**
     * High level device description - from ssdp. This is the BASIC device info.
     *
     *  @see DeviceDescription for more details on the device.
     */
    class Device {
    public:
        String fDeviceID; // this is the UUID (e.g. 315CAAE0-668D-47C7-A178-24C9EE756627)
        URI    fLocation;
        String fServer;

        /**
         *  Mapper to facilitate serialization
         */
        static const Foundation::DataExchange::ObjectVariantMapper kMapper;
    };

    /**
     */
    String MungePrimaryMacAddrIntoBaseDeviceID (String baseDeviceID);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Device.inl"

#endif /*_Stroika_Frameworks_UPnP_Device_h_*/
