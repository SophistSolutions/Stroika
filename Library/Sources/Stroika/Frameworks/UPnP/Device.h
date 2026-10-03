/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Device_h_
#define _Stroika_Frameworks_UPnP_Device_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/GUID.h"
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
        /**
         *  The device's UUID - a UPnP device MUST use a 128-bit UUID, written 8-4-4-4-12 hex digits (UPnP Device Architecture
         *  1.1, section 1.1.4). Its UDN is "uuid:" and this.
         */
        Common::GUID fDeviceID;

        /**
         *  Where the device's description is - the LOCATION it advertises.
         */
        URI fLocation;

        String fServer;

        /**
         *  Mapper to facilitate serialization
         */
        static const Foundation::DataExchange::ObjectVariantMapper kMapper;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Device.inl"

#endif /*_Stroika_Frameworks_UPnP_Device_h_*/
