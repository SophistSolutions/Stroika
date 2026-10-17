/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_DeviceDescription_h_
#define _Stroika_Frameworks_UPnP_DeviceDescription_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/Options.h"

/*
 * TODO:
 *      @todo   Add image support
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
        /**
         *  The description's <URLBase>, if it has one: what its relative URLs - each service's, the icons', fPresentationURL -
         *  are relative to. Without one, they are relative to the URL the description was read from. So a control point
         *  resolves one as fURLBase.value_or (descriptionURL).Combine (url).
         *
         *  An older (UPnP 1.0) device may send one, and a control point must then apply it; UPnP 1.1 forbids a device to send
         *  one (section 2.3), so a device built on Stroika leaves it unset. Read, an empty one - or one that is not a URL - is
         *  taken for none.
         */
        optional<URI>    fURLBase;
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
         *  @todo made several fields optional - so Icon default constructible - but according to 
         *        https://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf these sb required (so not optional).
         *        @todo FIX
         */
        struct Icon {
            InternetMediaType fMimeType{};
            uint16_t          fHorizontalPixels{16};
            uint16_t          fVerticalPixels{16};
            uint16_t          fColorDepth{8};
            URI               fURL{}; // url to the icon image file

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };
        optional<Collection<Icon>> fIcons;

        /**
         *  One of the device's services: what it is, and where its description, its actions and its events are - each URL
         *  relative to the device description's own (UPnP Device Architecture 1.1, section 2.3; an older device's may be
         *  absolute, or relative to its fURLBase), so a control point resolves it with fURLBase.value_or (descriptionURL).Combine ().
         */
        struct Service {
            String fServiceType; // e.g. urn:schemas-upnp-org:service:serviceType:v
            String fServiceID;   // e.g. urn:upnp-org:serviceId:serviceID
            URI    fSCPDURL;     // URL to service description
            URI    fControlURL;  // URL for control: its actions, as SOAP requests
            URI    fEventSubURL; // URL for eventing (subscribing to its state's changes); empty if it has no evented state

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
        static const Foundation::Execution::LazyInitialized<Foundation::DataExchange::ObjectVariantMapper> kMapper;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        static Foundation::DataExchange::ObjectVariantMapper mkMapper_ ();
    };
    const inline Foundation::Execution::LazyInitialized<Foundation::DataExchange::ObjectVariantMapper> DeviceDescription::kMapper{DeviceDescription::mkMapper_};

    /**
     *  \brief The device's description XML: what its LOCATION serves - as UPnP's XML (text/xml), UTF-8
     *
     *  \note Needs no XML library: the text is written directly (Variant::XML::Writer), so options' provider plays no part.
     */
    Memory::BLOB Serialize (const DeviceDescription& dd, const Options& options = {});

    /**
     *  \brief A device description read - what Serialize writes, and any other's: what it does not know - a vendor's own
     *         elements, an embedded <deviceList> - is skipped, and a URL or icon size that is not one is read as an empty
     *         URL or 0. Throws if it is not XML, its document element is not <root>, or the build has no XML parser
     *         (qStroika_Foundation_DataExchange_XML_SupportParsing).
     */
    DeviceDescription DeSerialize (const Memory::BLOB& b, const Options& options = {});

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    /**
     *  \brief UPnP's own device description schema, compiled for a provider - what to give Options::fValidateReadAgainst
     *
     *  @see Options::fValidateReadAgainst, which is how a description being read is checked against it, and which says
     *       why nothing does so by default.
     *
     *  Compiled afresh each call, and NOT cached: a Schema::Ptr may not outlive the provider that made it, and nothing
     *  held in a static can promise that. A caller reading more than the occasional description holds one in its own
     *  Options, where the lifetime is plain. Compiling is not free, so do not call this per description.
     *
     *  Nothing inside UPnP calls this, which is what lets a program that never validates not carry the schema's ~11K
     *  (it lives in a translation unit of its own; Stroika is a static library).
     *
     *  \note Give it the provider the matching Options names, where that is not the default one: a schema and a parser
     *        from different providers do work together, but slowly (@see XML::SAXParse).
     *
     *  \note The schema is https://upnp.org/schemas/device-1-0.xsd as it stood 2026-10-13.
     */
    Foundation::DataExchange::XML::Schema::Ptr DeviceDescriptionSchema (
        const Foundation::DataExchange::XML::Providers::ISchemaProvider& p = *Foundation::DataExchange::XML::Providers::kDefaultProvider ());
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DeviceDescription.inl"

#endif /*_Stroika_Frameworks_UPnP_DeviceDescription_h_*/
