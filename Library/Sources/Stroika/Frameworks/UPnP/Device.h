/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Device_h_
#define _Stroika_Frameworks_UPnP_Device_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"


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
                String fUSN;
                String fLocation;
                String fServer;
                String fST;         // usually ST header (or NT for notify)
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
