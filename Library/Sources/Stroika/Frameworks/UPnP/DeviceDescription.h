/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DeviceDescription_h_
#define _Stroika_Frameworks_UPnP_DeviceDescription_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/Containers/Collection.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"
#include    "../../Foundation/IO/Network/URL.h"
#include    "../../Foundation/Memory/BLOB.h"

#include    "Device.h"



/*
 * TODO:
 *      @todo   Add image support
 *      @todo   Add serviceList support
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Collection;
            using   DataExchange::InternetMediaType;
            using   IO::Network::URL;


            /**
             * high level device description - from ssdp. This is the BASIC device info
             * whcih appears to the XML file (poitned to by Device location field).
             */
            struct  DeviceDescription {
                String              fPresentationURL;
                String              fDeviceType;            //  http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf - <deviceType> - Page 44
                String              fManufactureName;
                String              fFriendlyName;
                String              fManufacturingURL;
                String              fModelDescription;
                String              fModelName;
                String              fModelNumber;
                String              fModelURL;
                String              fSerialNumber;
                String              fUPC;

                struct  Icon;
                Collection<Icon>      fIcons;

                struct  Service;
                Collection<Service>   fServices;

                DeviceDescription ();
            };


            /**
             */
            struct  DeviceDescription::Icon {
                Icon ();

                InternetMediaType   fMimeType;
                uint16_t            fHorizontalPixels;
                uint16_t            fVerticalPixels;
                uint16_t            fColorDepth;
                URL                 fURL;       // url to the icon image file
            };


            /**
            */
            struct  DeviceDescription::Service {
                Service ();

                String      fServiceType;           // eg urn:schemas - upnp - org : service : serviceType : v
                String      fServiceID;             // eg urn : upnp - org : serviceId : serviceID
                URL         fSCPDURL;               // URL to service description
                URL         fControlURL;            // URL to service description
                URL         fEventSubURL;           // URL to service description
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
