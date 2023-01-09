/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <sstream>

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "../../Foundation/DataExchange/XML/SAXReader.h"
#include "../../Foundation/DataExchange/XML/WriterUtils.h"
#include "../../Foundation/Streams/TextReader.h"
#include "../../Foundation/Streams/iostream/InputStreamFromStdIStream.h"

#include "DeviceDescription.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

/*
 ********************************************************************************
 ************************* DeviceDescription::Icon ******************************
 ********************************************************************************
 */
String DeviceDescription::Icon::ToString () const
{
    Characters::StringBuilder sb;
    sb += "{";
    sb += "MimeType: " + Characters::ToString (fMimeType) + ", ";
    sb += "HorizontalPixels: " + Characters::ToString (fHorizontalPixels) + ", ";
    sb += "VerticalPixels: " + Characters::ToString (fVerticalPixels) + ", ";
    sb += "ColorDepth : " + Characters::ToString (fColorDepth) + ", ";
    sb += "URL: " + Characters::ToString (fURL) + ", ";
    sb += "}";
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
    sb += "{";
    sb += "Service-ID: " + Characters::ToString (fServiceID) + ", ";
    sb += "SCPD-URL: " + Characters::ToString (fSCPDURL) + ", ";
    sb += "Control-URL: " + Characters::ToString (fControlURL) + ", ";
    sb += "Event-Sub-URL: " + Characters::ToString (fEventSubURL) + ", ";
    sb += "}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************************* DeviceDescription ******************************
 ********************************************************************************
 */
String DeviceDescription::ToString () const
{
    Characters::StringBuilder sb;
    sb += "{";
    if (fPresentationURL) {
        sb += "Presentation-URL: " + Characters::ToString (fPresentationURL) + ", ";
    }
    sb += "Device-Type: " + Characters::ToString (fDeviceType) + ", ";
    sb += "Manufacture-Name: " + Characters::ToString (fManufactureName) + ", ";
    sb += "Friendly-Name: " + Characters::ToString (fFriendlyName) + ", ";
    if (fManufacturingURL) {
        sb += "Manufacturing-URL: " + Characters::ToString (fManufacturingURL) + ", ";
    }
    if (fModelDescription) {
        sb += "Model-Description: " + Characters::ToString (fModelDescription) + ", ";
    }
    sb += "Model-Name: " + Characters::ToString (fModelName) + ", ";
    if (fModelNumber) {
        sb += "Model-Number: " + Characters::ToString (fModelNumber) + ", ";
    }
    if (fModelURL) {
        sb += "Model-URL: " + Characters::ToString (*fModelURL) + ", ";
    }
    if (fSerialNumber) {
        sb += "Serial-Number: " + Characters::ToString (fSerialNumber) + ", ";
    }
    sb += "UDN: " + Characters::ToString (fUDN) + ", ";
    if (fUPC) {
        sb += "UPC: " + Characters::ToString (fUPC) + ", ";
    }
    if (fIcons) {
        sb += "Icons: " + Characters::ToString (fIcons) + ", ";
    }
    if (fServices) {
        sb += "Services: " + Characters::ToString (fServices) + ", ";
    }
    sb += "}";
    return sb.str ();
}

ObjectVariantMapper DeviceDescription::mkMapper_ ()
{
    ObjectVariantMapper mapper;

    mapper.AddClass<Icon> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {L"Alive", StructFieldMetaInfo{&Icon::fMimeType}},
        {L"USN", StructFieldMetaInfo{&Icon::fHorizontalPixels}},
        {L"Server", StructFieldMetaInfo{&Icon::fVerticalPixels}},
        {L"Target", StructFieldMetaInfo{&Icon::fColorDepth}},
        {L"RawHeaders", StructFieldMetaInfo{&Icon::fURL}},
    });
    mapper.AddCommonType<Collection<Icon>> ();
    mapper.AddCommonType<optional<Collection<Icon>>> ();

    mapper.AddClass<Service> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {L"Alive", StructFieldMetaInfo{&Service::fServiceType}},
        {L"USN", StructFieldMetaInfo{&Service::fServiceID}},
        {L"Server", StructFieldMetaInfo{&Service::fSCPDURL}},
        {L"Target", StructFieldMetaInfo{&Service::fControlURL}},
        {L"RawHeaders", StructFieldMetaInfo{&Service::fEventSubURL}},
    });
    mapper.AddCommonType<Collection<Service>> ();
    mapper.AddCommonType<optional<Collection<Service>>> ();

    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<URI>> ();

    mapper.AddClass<DeviceDescription> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {L"PresentationURL", StructFieldMetaInfo{&DeviceDescription::fPresentationURL}},
        {L"DeviceType", StructFieldMetaInfo{&DeviceDescription::fDeviceType}},
        {L"ManufactureName", StructFieldMetaInfo{&DeviceDescription::fManufactureName}},
        {L"FriendlyName", StructFieldMetaInfo{&DeviceDescription::fFriendlyName}},
        {L"ManufacturingURL", StructFieldMetaInfo{&DeviceDescription::fManufacturingURL}},
        {L"ModelDescription", StructFieldMetaInfo{&DeviceDescription::fModelDescription}},
        {L"ModelName", StructFieldMetaInfo{&DeviceDescription::fModelName}},
        {L"ModelNumber", StructFieldMetaInfo{&DeviceDescription::fModelNumber}},
        {L"ModelURL", StructFieldMetaInfo{&DeviceDescription::fModelURL}},
        {L"SerialNumber", StructFieldMetaInfo{&DeviceDescription::fSerialNumber}},
        {L"UDN", StructFieldMetaInfo{&DeviceDescription::fUDN}},
        {L"UPC", StructFieldMetaInfo{&DeviceDescription::fUPC}},
        {L"Icons", StructFieldMetaInfo{&DeviceDescription::fIcons}},
        {L"Services", StructFieldMetaInfo{&DeviceDescription::fServices}},
    });
    return mapper;
};

/*
 ********************************************************************************
 ********************************* UPnP::Serialize ******************************
 ********************************************************************************
 */
Memory::BLOB UPnP::Serialize (const DeviceDescription& dd)
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
    if (dd.fManufacturingURL) {
        tmp << "                <manufacturerURL>" << QuoteForXML (dd.fManufacturingURL->As<String> ()) << "</manufacturerURL>" << endl;
    }
    if (dd.fModelDescription) {
        tmp << "            <modelDescription>" << QuoteForXML (dd.fModelDescription) << "</modelDescription>" << endl;
    }
    tmp << "            <modelName>" << QuoteForXML (dd.fModelName) << "</modelName>" << endl;
    if (dd.fModelNumber) {
        tmp << "            <modelNumber>" << QuoteForXML (dd.fModelNumber) << "</modelNumber>" << endl;
    }
    if (dd.fModelURL) {
        tmp << "                <modelURL>" << QuoteForXML (dd.fModelURL->As<String> ()) << "</modelURL>" << endl;
    }
    if (dd.fSerialNumber) {
        tmp << "            <serialNumber>" << QuoteForXML (dd.fSerialNumber) << "</serialNumber>" << endl;
    }
    tmp << "            <UDN>" << QuoteForXML (dd.fUDN) << "</UDN>" << endl;
    if (dd.fUPC) {
        tmp << "                <UPC>" << QuoteForXML (*dd.fUPC) << "</UPC>" << endl;
    }
    if (dd.fIcons) {
        tmp << "                <iconList>" << endl;
        for (const DeviceDescription::Icon& i : *dd.fIcons) {
            tmp << "                    <icon>" << endl;
            tmp << "                            <mimetype>" << QuoteForXML (i.fMimeType.As<String> ()) << "</mimetype>" << endl;
            tmp << "                            <width>" << i.fHorizontalPixels << "</width>" << endl;
            tmp << "                            <height>" << i.fVerticalPixels << "</height>" << endl;
            tmp << "                            <depth>" << i.fColorDepth << "</depth>" << endl;
            tmp << "                            <url>" << QuoteForXML (i.fURL.As<String> ()) << "</url>" << endl;
            tmp << "                    </icon>" << endl;
        }
        tmp << "                </iconList>" << endl;
    }
    if (dd.fServices) {
        tmp << "                <serviceList>" << endl;
        for (const DeviceDescription::Service& i : *dd.fServices) {
            tmp << "                    <service>" << endl;
            tmp << "                            <serviceType>" << QuoteForXML (i.fServiceType) << "</serviceType>" << endl;
            tmp << "                            <serviceId>" << QuoteForXML (i.fServiceID) << "</serviceId>" << endl;
            tmp << "                            <SCPDURL>" << QuoteForXML (i.fSCPDURL.As<String> ()) << "</SCPDURL>" << endl;
            tmp << "                            <controlURL>" << QuoteForXML (i.fControlURL.As<String> ()) << "</controlURL>" << endl;
            tmp << "                            <eventSubURL>" << QuoteForXML (i.fEventSubURL.As<String> ()) << "</eventSubURL>" << endl;
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

    if (dd.fPresentationURL.has_value ()) {
        tmp << "                <presentationURL>" << QuoteForXML (dd.fPresentationURL->As<String> ()) << "</presentationURL>" << endl;
    }
    tmp << "    </device>" << endl;
    tmp << "</root>" << endl;
    return Streams::iostream::InputStreamFromStdIStream<byte>::New (tmp).ReadAll ();
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

    static const ObjectReader::Registry kTypesRegistry_ = [] () {
        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonType<optional<String>> ();
        registry.AddCommonType<uint16_t> ();
        registry.AddCommonType<URI> ();
        registry.AddCommonType<optional<URI>> ();
        registry.AddCommonReader_Simple<InternetMediaType> ([] (const String& s) { return InternetMediaType{s}; });
        registry.AddCommonReader_Class<DeviceDescription::Icon> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"mimetype"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fMimeType}},
            {Name{L"width"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fHorizontalPixels}},
            {Name{L"height"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fVerticalPixels}},
            {Name{L"depth"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fColorDepth}},
            {Name{L"url"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fURL}},
        });
        registry.AddCommonReader_Class<DeviceDescription::Service> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"serviceType"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fServiceType}},
            {Name{L"serviceId"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fServiceID}},
            {Name{L"SCPDURL"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fSCPDURL}},
            {Name{L"controlURL"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fControlURL}},
        });
        registry.AddCommonType<Collection<DeviceDescription::Icon>> (Name{L"icon"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Icon>>> ();
        registry.AddCommonType<Collection<DeviceDescription::Service>> (Name{L"service"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Service>>> ();
        registry.AddCommonReader_Class<DeviceDescription> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"presentationURL"sv}, StructFieldMetaInfo{&DeviceDescription::fPresentationURL}},
            {Name{L"deviceType"sv}, StructFieldMetaInfo{&DeviceDescription::fDeviceType}},
            {Name{L"manufacturer"sv}, StructFieldMetaInfo{&DeviceDescription::fManufactureName}},
            {Name{L"friendlyName"sv}, StructFieldMetaInfo{&DeviceDescription::fFriendlyName}},
            {Name{L"manufacturerURL"sv}, StructFieldMetaInfo{&DeviceDescription::fManufacturingURL}},
            {Name{L"modelDescription"sv}, StructFieldMetaInfo{&DeviceDescription::fModelDescription}},
            {Name{L"modelName"sv}, StructFieldMetaInfo{&DeviceDescription::fModelName}},
            {Name{L"modelNumber"sv}, StructFieldMetaInfo{&DeviceDescription::fModelNumber}},
            {Name{L"modelURL"sv}, StructFieldMetaInfo{&DeviceDescription::fModelURL}},
            {Name{L"serialNum"sv}, StructFieldMetaInfo{&DeviceDescription::fSerialNumber}},
            {Name{L"UDN"sv}, StructFieldMetaInfo{&DeviceDescription::fUDN}},
            {Name{L"UPC"sv}, StructFieldMetaInfo{&DeviceDescription::fUPC}},
            {Name{L"iconList"sv}, StructFieldMetaInfo{&DeviceDescription::fIcons}},
            {Name{L"serviceList"sv}, StructFieldMetaInfo{&DeviceDescription::fServices}},
        });
        return registry;
    }();

    DeviceDescription deviceDescription;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"xml data: %s", Streams::TextReader::New (b).ReadAll ().c_str ());
#endif
    {
        ObjectReader::IConsumerDelegateToContext ctx{kTypesRegistry_, make_shared<ObjectReader::ReadDownToReader> (kTypesRegistry_.MakeContextReader (&deviceDescription), Name{L"device"})};
        XML::SAXParse (b, ctx);
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"deviceDescription: %s", Characters::ToString (deviceDescription).c_str ());
#endif
    return deviceDescription;
}
