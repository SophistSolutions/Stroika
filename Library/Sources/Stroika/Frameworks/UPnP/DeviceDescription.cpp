/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
    sb << "{"sv;
    sb << "MimeType: "sv << Characters::ToString (fMimeType) << ", "sv;
    sb << "HorizontalPixels: "sv << Characters::ToString (fHorizontalPixels) << ", "sv;
    sb << "VerticalPixels: "sv << Characters::ToString (fVerticalPixels) << ", "sv;
    sb << "ColorDepth : "sv << Characters::ToString (fColorDepth) << ", "sv;
    sb << "URL: "sv << Characters::ToString (fURL);
    sb << "}"sv;
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
    sb << "{"sv;
    sb << "Service-ID: "sv << Characters::ToString (fServiceID) << ", "sv;
    sb << "SCPD-URL: "sv << Characters::ToString (fSCPDURL) << ", "sv;
    sb << "Control-URL: "sv << Characters::ToString (fControlURL) << ", "sv;
    sb << "Event-Sub-URL: "sv << Characters::ToString (fEventSubURL);
    sb << "}"sv;
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
    sb << "{"sv;
    if (fPresentationURL) {
        sb << "Presentation-URL: "sv << Characters::ToString (fPresentationURL) << ", "sv;
    }
    sb << "Device-Type: "sv << Characters::ToString (fDeviceType) << ", "sv;
    sb << "Manufacture-Name: "sv << Characters::ToString (fManufactureName) << ", "sv;
    sb << "Friendly-Name: "sv << Characters::ToString (fFriendlyName) << ", "sv;
    if (fManufacturingURL) {
        sb << "Manufacturing-URL: "sv << Characters::ToString (fManufacturingURL) << ", "sv;
    }
    if (fModelDescription) {
        sb << "Model-Description: "sv << Characters::ToString (fModelDescription) << ", "sv;
    }
    sb << "Model-Name: "sv << Characters::ToString (fModelName) << ", "sv;
    if (fModelNumber) {
        sb << "Model-Number: "sv << Characters::ToString (fModelNumber) << ", "sv;
    }
    if (fModelURL) {
        sb << "Model-URL: "sv << Characters::ToString (*fModelURL) << ", "sv;
    }
    if (fSerialNumber) {
        sb << "Serial-Number: "sv << Characters::ToString (fSerialNumber) << ", "sv;
    }
    sb << "UDN: "sv << Characters::ToString (fUDN) << ", "sv;
    if (fUPC) {
        sb << "UPC: "sv << Characters::ToString (fUPC) << ", "sv;
    }
    if (fIcons) {
        sb << "Icons: "sv << Characters::ToString (fIcons) << ", "sv;
    }
    if (fServices) {
        sb << "Services: "sv << Characters::ToString (fServices);
    }
    sb << "}sv";
    return sb.str ();
}

ObjectVariantMapper DeviceDescription::mkMapper_ ()
{
    ObjectVariantMapper mapper;

    mapper.AddClass<Icon> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {"Alive"sv, StructFieldMetaInfo{&Icon::fMimeType}},
        {"USN"sv, StructFieldMetaInfo{&Icon::fHorizontalPixels}},
        {"Server"sv, StructFieldMetaInfo{&Icon::fVerticalPixels}},
        {"Target"sv, StructFieldMetaInfo{&Icon::fColorDepth}},
        {"RawHeaders"sv, StructFieldMetaInfo{&Icon::fURL}},
    });
    mapper.AddCommonType<Collection<Icon>> ();
    mapper.AddCommonType<optional<Collection<Icon>>> ();

    mapper.AddClass<Service> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {"Alive"sv, StructFieldMetaInfo{&Service::fServiceType}},
        {"USN"sv, StructFieldMetaInfo{&Service::fServiceID}},
        {"Server"sv, StructFieldMetaInfo{&Service::fSCPDURL}},
        {"Target"sv, StructFieldMetaInfo{&Service::fControlURL}},
        {"RawHeaders"sv, StructFieldMetaInfo{&Service::fEventSubURL}},
    });
    mapper.AddCommonType<Collection<Service>> ();
    mapper.AddCommonType<optional<Collection<Service>>> ();

    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<URI>> ();

    mapper.AddClass<DeviceDescription> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {"PresentationURL"sv, StructFieldMetaInfo{&DeviceDescription::fPresentationURL}},
        {"DeviceType"sv, StructFieldMetaInfo{&DeviceDescription::fDeviceType}},
        {"ManufactureName"sv, StructFieldMetaInfo{&DeviceDescription::fManufactureName}},
        {"FriendlyName"sv, StructFieldMetaInfo{&DeviceDescription::fFriendlyName}},
        {"ManufacturingURL"sv, StructFieldMetaInfo{&DeviceDescription::fManufacturingURL}},
        {"ModelDescription"sv, StructFieldMetaInfo{&DeviceDescription::fModelDescription}},
        {"ModelName"sv, StructFieldMetaInfo{&DeviceDescription::fModelName}},
        {"ModelNumber"sv, StructFieldMetaInfo{&DeviceDescription::fModelNumber}},
        {"ModelURL"sv, StructFieldMetaInfo{&DeviceDescription::fModelURL}},
        {"SerialNumber"sv, StructFieldMetaInfo{&DeviceDescription::fSerialNumber}},
        {"UDN"sv, StructFieldMetaInfo{&DeviceDescription::fUDN}},
        {"UPC"sv, StructFieldMetaInfo{&DeviceDescription::fUPC}},
        {"Icons"sv, StructFieldMetaInfo{&DeviceDescription::fIcons}},
        {"Services"sv, StructFieldMetaInfo{&DeviceDescription::fServices}},
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
            {Name{"mimetype"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fMimeType}},
            {Name{"width"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fHorizontalPixels}},
            {Name{"height"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fVerticalPixels}},
            {Name{"depth"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fColorDepth}},
            {Name{"url"sv}, StructFieldMetaInfo{&DeviceDescription::Icon::fURL}},
        });
        registry.AddCommonReader_Class<DeviceDescription::Service> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{"serviceType"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fServiceType}},
            {Name{"serviceId"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fServiceID}},
            {Name{"SCPDURL"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fSCPDURL}},
            {Name{"controlURL"sv}, StructFieldMetaInfo{&DeviceDescription::Service::fControlURL}},
        });
        registry.AddCommonType<Collection<DeviceDescription::Icon>> (Name{"icon"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Icon>>> ();
        registry.AddCommonType<Collection<DeviceDescription::Service>> (Name{"service"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Service>>> ();
        registry.AddCommonReader_Class<DeviceDescription> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{"presentationURL"sv}, StructFieldMetaInfo{&DeviceDescription::fPresentationURL}},
            {Name{"deviceType"sv}, StructFieldMetaInfo{&DeviceDescription::fDeviceType}},
            {Name{"manufacturer"sv}, StructFieldMetaInfo{&DeviceDescription::fManufactureName}},
            {Name{"friendlyName"sv}, StructFieldMetaInfo{&DeviceDescription::fFriendlyName}},
            {Name{"manufacturerURL"sv}, StructFieldMetaInfo{&DeviceDescription::fManufacturingURL}},
            {Name{"modelDescription"sv}, StructFieldMetaInfo{&DeviceDescription::fModelDescription}},
            {Name{"modelName"sv}, StructFieldMetaInfo{&DeviceDescription::fModelName}},
            {Name{"modelNumber"sv}, StructFieldMetaInfo{&DeviceDescription::fModelNumber}},
            {Name{"modelURL"sv}, StructFieldMetaInfo{&DeviceDescription::fModelURL}},
            {Name{"serialNum"sv}, StructFieldMetaInfo{&DeviceDescription::fSerialNumber}},
            {Name{"UDN"sv}, StructFieldMetaInfo{&DeviceDescription::fUDN}},
            {Name{"UPC"sv}, StructFieldMetaInfo{&DeviceDescription::fUPC}},
            {Name{"iconList"sv}, StructFieldMetaInfo{&DeviceDescription::fIcons}},
            {Name{"serviceList"sv}, StructFieldMetaInfo{&DeviceDescription::fServices}},
        });
        return registry;
    }();

    DeviceDescription deviceDescription;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"xml data: %s", Streams::TextReader::New (b).ReadAll ().c_str ());
#endif
    {
        ObjectReader::IConsumerDelegateToContext ctx{
            kTypesRegistry_, make_shared<ObjectReader::ReadDownToReader> (kTypesRegistry_.MakeContextReader (&deviceDescription), Name{"device"sv})};
        XML::SAXParse (b, ctx);
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"deviceDescription: %s", Characters::ToString (deviceDescription).c_str ());
#endif
    return deviceDescription;
}
