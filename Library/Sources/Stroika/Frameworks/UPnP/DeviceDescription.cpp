/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <sstream>

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "../../Foundation/DataExchange/XML/SAXReader.h"
#include "../../Foundation/DataExchange/XML/WriterUtils.h"
#include "../../Foundation/Streams/iostream/InputStreamFromStdIStream.h"

#include "DeviceDescription.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

/*
 ********************************************************************************
 *************************** DeviceDescription::Icon ****************************
 ********************************************************************************
 */
DeviceDescription::Icon::Icon ()
    : fMimeType ()
    , fHorizontalPixels (16)
    , fVerticalPixels (16)
    , fColorDepth (8)
    , fURL ()
{
}

/*
 ********************************************************************************
 ************************* DeviceDescription::Icon ******************************
 ********************************************************************************
 */
String DeviceDescription::Icon::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"fMimeType : '" + Characters::ToString (fMimeType) + L"', ";
    sb += L"fHorizontalPixels : '" + Characters::ToString (fHorizontalPixels) + L"', ";
    sb += L"fVerticalPixels : '" + Characters::ToString (fVerticalPixels) + L"', ";
    sb += L"fColorDepth : '" + Characters::ToString (fColorDepth) + L"', ";
    sb += L"fURL : '" + Characters::ToString (fURL) + L"', ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************ DeviceDescription::Service ****************************
 ********************************************************************************
 */
String DeviceDescription::Service::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"Service-ID : '" + Characters::ToString (fServiceID) + L"', ";
    sb += L"SCPD-URL : '" + Characters::ToString (fSCPDURL) + L"', ";
    sb += L"Control-URL : '" + Characters::ToString (fControlURL) + L"', ";
    sb += L"Event-Sub-URL : '" + Characters::ToString (fEventSubURL) + L"', ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************************* DeviceDescription ******************************
 ********************************************************************************
 */
DeviceDescription::DeviceDescription ()
    : fPresentationURL ()
    , fDeviceType ()
    , fManufactureName ()
    , fFriendlyName ()
    , fManufacturingURL ()
    , fModelDescription ()
    , fModelName ()
    , fModelNumber ()
    , fModelURL ()
    , fSerialNumber ()
    , fUPC ()
    , fIcons ()
    , fServices ()
{
}

String DeviceDescription::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"Presentation-URL : '" + Characters::ToString (fPresentationURL) + L"', ";
    sb += L"Device-Type : '" + Characters::ToString (fDeviceType) + L"', ";
    sb += L"Manufacture-Name : '" + Characters::ToString (fManufactureName) + L"', ";
    sb += L"Friendly-Name : '" + Characters::ToString (fFriendlyName) + L"', ";
    sb += L"Manufacturing-URL : '" + Characters::ToString (fManufacturingURL) + L"', ";
    sb += L"Model-Description : '" + Characters::ToString (fModelDescription) + L"', ";
    sb += L"Model-Name : '" + Characters::ToString (fModelName) + L"', ";
    sb += L"Model-Number : '" + Characters::ToString (fModelNumber) + L"', ";
    sb += L"Serial-Number : '" + Characters::ToString (fSerialNumber) + L"', ";
    sb += L"UPC : '" + Characters::ToString (fUPC) + L"', ";
    sb += L"Icons : '" + Characters::ToString (fIcons) + L"', ";
    sb += L"Services : '" + Characters::ToString (fServices) + L"', ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ********************************* UPnP::Serialize ******************************
 ********************************************************************************
 */
Memory::BLOB UPnP::Serialize (const Device& d, const DeviceDescription& dd)
{
    using namespace DataExchange::XML;

    /*
    // very very rough prelim draft
    *Example based on
    http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.0.pdf
    */
    stringstream tmp;
    tmp << "<?xml version=\"1.0\"?>" << endl;
    tmp << "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">" << endl;
    tmp << "    <specVersion>" << endl;
    tmp << "            <major>1</major>" << endl;
    tmp << "            <minor>0</minor>" << endl;
    tmp << "    </specVersion>" << endl;
    tmp << "    <device>" << endl;
    tmp << "            <deviceType>" << QuoteForXML (dd.fDeviceType) << "</deviceType>" << endl;
    tmp << "            <friendlyName>" << QuoteForXML (dd.fFriendlyName) << "</friendlyName>" << endl;
    tmp << "            <manufacturer>" << QuoteForXML (dd.fManufactureName) << "</manufacturer>" << endl;
    if (dd.fManufacturingURL.IsPresent ()) {
        tmp << "                <manufacturerURL>" << QuoteForXML (dd.fManufacturingURL->GetFullURL ()) << "</manufacturerURL>" << endl;
    }
    tmp << "            <modelDescription>" << QuoteForXML (dd.fModelDescription) << "</modelDescription>" << endl;
    tmp << "            <modelName>" << QuoteForXML (dd.fModelName) << "</modelName>" << endl;
    tmp << "            <modelNumber>" << QuoteForXML (dd.fModelNumber) << "</modelNumber>" << endl;
    if (dd.fModelURL.IsPresent ()) {
        tmp << "                <modelURL>" << QuoteForXML (dd.fModelURL->GetFullURL ()) << "</modelURL>" << endl;
    }
    tmp << "            <serialNumber>" << QuoteForXML (dd.fSerialNumber) << "</serialNumber>" << endl;
    tmp << "            <UDN>uuid:" << QuoteForXML (d.fDeviceID) << "</UDN>" << endl;
    if (not dd.fUPC.empty ()) {
        tmp << "                <UPC>" << QuoteForXML (dd.fUPC) << "</UPC>" << endl;
    }
    if (not dd.fIcons.empty ()) {
        tmp << "                <iconList>" << endl;
        for (DeviceDescription::Icon i : dd.fIcons) {
            tmp << "                    <icon>" << endl;
            tmp << "                            <mimetype>" << QuoteForXML (i.fMimeType.As<String> ()) << "</mimetype>" << endl;
            tmp << "                            <width>" << i.fHorizontalPixels << "</width>" << endl;
            tmp << "                            <height>" << i.fVerticalPixels << "</height>" << endl;
            tmp << "                            <depth>" << i.fColorDepth << "</depth>" << endl;
            tmp << "                            <url>" << QuoteForXML (i.fURL.GetFullURL ()) << "</url>" << endl;
            tmp << "                    </icon>" << endl;
        }
        tmp << "                </iconList>" << endl;
    }
    if (not dd.fServices.empty ()) {
        tmp << "                <serviceList>" << endl;
        for (DeviceDescription::Service i : dd.fServices) {
            tmp << "                    <service>" << endl;
            tmp << "                            <serviceType>" << QuoteForXML (i.fServiceType) << "</serviceType>" << endl;
            tmp << "                            <serviceId>" << QuoteForXML (i.fServiceID) << "</serviceId>" << endl;
            tmp << "                            <SCPDURL>" << QuoteForXML (i.fSCPDURL.GetFullURL ()) << "</SCPDURL>" << endl;
            tmp << "                            <controlURL>" << QuoteForXML (i.fControlURL.GetFullURL ()) << "</controlURL>" << endl;
            tmp << "                            <eventSubURL>" << QuoteForXML (i.fEventSubURL.GetFullURL ()) << "</eventSubURL>" << endl;
            tmp << "                    </service>" << endl;
        }
        tmp << "                </serviceList>" << endl;
    }

#if 0
    tmp << "    <deviceList>" << endl;
    // <!-- Description of embedded devices defined by a UPnP Forum working committee
    // (if any) go here -->
    // <!-- Description of embedded devices added by UPnP vendor (if any) go here -->
    tmp << "    </deviceList>" << endl;
#endif

    if (dd.fPresentationURL.IsPresent ()) {
        tmp << "                <presentationURL>" << QuoteForXML (dd.fPresentationURL->GetFullURL ()) << "</presentationURL>" << endl;
    }
    tmp << "    </device>" << endl;
    tmp << "</root>" << endl;
    return Streams::iostream::InputStreamFromStdIStream<Memory::Byte>::New (tmp).ReadAll ();
}

/*
 ********************************************************************************
 ******************************* UPnP::DeSerialize ******************************
 ********************************************************************************
 */
DeviceDescription UPnP::DeSerialize (const Memory::BLOB& b)
{
    using namespace Stroika::Foundation::DataExchange;
    using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
    using namespace Stroika::Foundation::DataExchange::XML;

    // @todo - Note - Based on UPnP::Serialize - so we should better source this data/info

    static const ObjectReader::Registry kTypesRegistry_ = []() {
        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonType<uint16_t> ();

// @todo  INCOMPLETE - URL, Optional<URL> InternetMediaType and two 'list' types incomplete
#if 0
        Optional<URL> fPresentationURL;
#endif
#if 0
        URL               fURL; // url to the icon image file
#endif
//  InternetMediaType fMimeType;
#if 0
        Collection<Icon> fIcons;
#endif
#if 0
        Collection<Service> fServices;
#endif
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        registry.AddClass<DeviceDescription::Icon> (initializer_list<ObjectReader::StructFieldInfo>{
            //       {Name{L"mimetype"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fMimeType)},
            {Name{L"width"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fHorizontalPixels)},
            {Name{L"height"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fVerticalPixels)},
            {Name{L"depth"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fColorDepth)},
            //      { Name{ L"url" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fURL) },
        });
        registry.AddClass<DeviceDescription::Service> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"serviceType"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fServiceType)},
            {Name{L"serviceId"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fServiceID)},
            //{ Name{ L"SCPDURL" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fSCPDURL) },
            //{ Name{ L"controlURL" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fControlURL) },
            //{ Name{ L"eventSubURL" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fEventSubURL) },
        });
        registry.AddClass<DeviceDescription> (initializer_list<ObjectReader::StructFieldInfo>{
            //          {Name{ L"presentationURL" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fPresentationURL)},
            {Name{L"deviceType"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fDeviceType)},
            {Name{L"manufacturer"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fManufactureName)},
            {Name{L"friendlyName"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fFriendlyName)},
            //          { Name{ L"manufacturerURL" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fManufacturingURL) },
            {Name{L"modelDescription"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelDescription)},
            {Name{L"modelName"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelName)},
            {Name{L"modelNumber"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelNumber)},
            //  { Name{ L"modelURL" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelURL) },
            {Name{L"serialNumber"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fSerialNumber)},
            {Name{L"UPC"}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fUPC)},
            //      { Name{ L"icons" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fIcons) },
            //      { Name{ L"serviceList" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fServices) },
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return registry;
    }();

    DeviceDescription deviceDescription;
    {
        ObjectReader::IConsumerDelegateToContext ctx{kTypesRegistry_, make_shared<ObjectReader::ReadDownToReader> (kTypesRegistry_.MakeContextReader (&deviceDescription), Name{L"device"})};
        XML::SAXParse (b, ctx);
    }
    return deviceDescription;
}
