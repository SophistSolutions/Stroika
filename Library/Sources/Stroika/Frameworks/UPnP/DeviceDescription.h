/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DeviceDescription_h_
#define _Stroika_Frameworks_UPnP_DeviceDescription_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/IO/Network/URI.h"
#include "../../Foundation/Memory/BLOB.h"

#include "Device.h"

/*
 * TODO:
 *      @todo   Add image support
 *      @todo   Add serviceList support
 */

namespace Stroika::Frameworks::UPnP {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Collection;
    using DataExchange::InternetMediaType;
    using IO::Network::URI;

    /**
     * high level device description - from ssdp. This is the BASIC device info
     * whcih appears to the XML file (pointed to by Device location field).
     *
     *  This is based on http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf, page 43-46 apx
     */
    struct DeviceDescription {
        optional<URI>    fPresentationURL;
        String           fDeviceType; //  http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf - <deviceType> - Page 44
        String           fManufactureName;
        String           fFriendlyName;
        optional<URI>    fManufacturingURL;
        optional<String> fModelDescription;
        String           fModelName;
        optional<String> fModelNumber;
        optional<URI>    fModelURL;
        optional<String> fSerialNumber;
        /**
         *  Unique Device Name. Universally-unique identifier for the device, whether root or embedded.
         *  MUST be the same over time for a specific device instance (i.e., MUST survive reboots).MUST match the
         *  field value of the NT header field in device discovery messages.MUST match the prefix of the USN
         *  header field in all discovery messages.
         */
        String fUDN;

        /*
         * OPTIONAL. Universal Product Code. 12-digit, all-numeric code that identifies the consumer package.
         * Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
         */
        optional<String> fUPC;

        /**
         */
        struct Icon {
            Icon () = default;

            InternetMediaType fMimeType;
            uint16_t          fHorizontalPixels{16};
            uint16_t          fVerticalPixels{16};
            uint16_t          fColorDepth{8};
            URI               fURL; // url to the icon image file

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };
        optional<Collection<Icon>> fIcons;

        /**
         */
        struct Service {
            String fServiceType; // e.g. urn:schemas-upnp-org:service:serviceType:v
            String fServiceID;   // e.g. urn:upnp-org:serviceId:serviceID
            URI    fSCPDURL;     // URL to service description
            URI    fControlURL;  // URL to service description
            URI    fEventSubURL; // URL to service description

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };
        optional<Collection<Service>> fServices;

        DeviceDescription () = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /*
    */
    Memory::BLOB Serialize (const DeviceDescription& dd);

    /*
    */
    DeviceDescription DeSerialize (const Memory::BLOB& b);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DeviceDescription.inl"

#endif /*_Stroika_Frameworks_UPnP_DeviceDescription_h_*/
