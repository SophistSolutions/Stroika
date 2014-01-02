/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Device_h_
#define _Stroika_Frameworks_UPnP_Device_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/Memory/Optional.h"



/*
 * TODO:
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {

            using   namespace   Stroika::Foundation;
            using   Characters::String;


            /**
             * High level device description - from ssdp. This is the BASIC device info.
             *
             *  @see DeviceDescription for more details on the device.
             */
            class   Device {
            public:
                String  fDeviceID;      // this is the UUID (e.g. 315CAAE0-668D-47C7-A178-24C9EE756627)
                String  fLocation;
                String  fServer;
            };


            /**
             */
            String  MungePrimaryMacAddrIntoBaseDeviceID (String baseDeviceID);


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Device.inl"

#endif  /*_Stroika_Frameworks_UPnP_Device_h_*/
