/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DeviceDescription_h_
#define _Stroika_Frameworks_UPnP_DeviceDescription_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/Memory/BLOB.h"

#include    "Device.h"


/*
 * TODO:
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {


            using   namespace   Stroika::Foundation;
            using   Characters::String;


            /**
             * high level device description - from ssdp. This is the BASIC device info
             * whcih appears to the XML file (poitned to by Device location field).
             */
            struct  DeviceDescription {
                String  fUDN;
                String  fPresentationURL;
                String  fDeviceType;            //  http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf - <deviceType> - Page 44
                String  fManufactureName;
                String  fFriendlyName;
                String  fManufacturingURL;
                String  fModelDescription;
                String  fModelName;
                String  fModelNumber;
                String  fModelURL;
                String  fSerialNumber;
                String  fUPC;

                DeviceDescription ();
            };


            /*
            */
            Memory::BLOB        Serialize (const Device& d, const DeviceDescription& dd);


            /*
            */
            DeviceDescription   DeSerialize (const Memory::BLOB& b);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DeviceDescription.inl"

#endif  /*_Stroika_Frameworks_UPnP_DeviceDescription_h_*/
