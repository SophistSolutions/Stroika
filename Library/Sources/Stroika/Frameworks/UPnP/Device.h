/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Device_h_
#define _Stroika_Frameworks_UPnP_Device_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/Containers/Mapping.h"
#include    "../../Foundation/Memory/Optional.h"


/*
 * TODO:
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {

            using   namespace   Stroika::Foundation;
            using   Characters::String;



            // PROBABYLY add new file/mopdule for class ElaboratedDevice : public Device {};


            /**
             * high level device description - from ssdp. This is the BASIC device info
             */
            class   Device {
            public:
                String  fDeviceID;      // this is the UUID (e.g. 315CAAE0-668D-47C7-A178-24C9EE756627)
                String  fLocation;
                String  fServer;
//                String    fST;         // usually ST header (or NT for notify)
            };


            /*
            */
            String  MungePrimaryMacAddrIntoBaseDeviceID (String baseDeviceID);


            // @todo MOVE TO SEPARET IFLE
            struct  DeviceAnnouncement {
                Memory::Optional<bool>      fAlive; // else Bye notification, or empty if neither
                String                      fUSN;
                String                      fLocation;
                String                      fServer;
                String                      fST;         // usually ST header (or NT for notify)
                Containers::Mapping<String, String>     fRawHeaders;
            };


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
