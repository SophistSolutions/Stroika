/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DeviceDescription_h_
#define _Stroika_Frameworks_UPnP_DeviceDescription_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"

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
     * which appears to the XML file (pointed to by Device location field).
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
            optional<InternetMediaType> fMimeType;
            optional<uint16_t>          fHorizontalPixels;
            optional<uint16_t>          fVerticalPixels;
            optional<uint16_t>          fColorDepth;
            optional<URI>               fURL; // url to the icon image file

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;

            static const Icon kDefault;
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
         *  Mapper to facilitate serialization
         */
        static const Foundation::Common::ConstantProperty<Foundation::DataExchange::ObjectVariantMapper> kMapper;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        static Foundation::DataExchange::ObjectVariantMapper mkMapper_ ();
    };
    #if qCompilerAndStdLib_defaultconstructibleFailsWithoutStaticAssert_Buggy
    static_assert (is_constructible_v<DeviceDescription::Icon>);
    #endif
    const inline Foundation::Common::ConstantProperty<Foundation::DataExchange::ObjectVariantMapper> DeviceDescription::kMapper{DeviceDescription::mkMapper_};

    /**
     */
    Memory::BLOB Serialize (const DeviceDescription& dd);

    /**
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
