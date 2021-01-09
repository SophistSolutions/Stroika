/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
    sb += L"{";
    sb += L"MimeType: " + Characters::ToString (fMimeType) + L", ";
    sb += L"HorizontalPixels: " + Characters::ToString (fHorizontalPixels) + L", ";
    sb += L"VerticalPixels: " + Characters::ToString (fVerticalPixels) + L", ";
    sb += L"ColorDepth : " + Characters::ToString (fColorDepth) + L", ";
    sb += L"URL: " + Characters::ToString (fURL) + L", ";
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
    sb += L"Service-ID: " + Characters::ToString (fServiceID) + L", ";
    sb += L"SCPD-URL: " + Characters::ToString (fSCPDURL) + L", ";
    sb += L"Control-URL: " + Characters::ToString (fControlURL) + L", ";
    sb += L"Event-Sub-URL: " + Characters::ToString (fEventSubURL) + L", ";
    sb += L"}";
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
    sb += L"{";
    sb += L"Presentation-URL: " + Characters::ToString (fPresentationURL) + L", ";
    sb += L"Device-Type: " + Characters::ToString (fDeviceType) + L", ";
    sb += L"Manufacture-Name: " + Characters::ToString (fManufactureName) + L", ";
    sb += L"Friendly-Name: " + Characters::ToString (fFriendlyName) + L", ";
    if (fManufacturingURL) {
        sb += L"Manufacturing-URL: " + Characters::ToString (fManufacturingURL) + L", ";
    }
    if (fModelDescription) {
        sb += L"Model-Description: " + Characters::ToString (fModelDescription) + L", ";
    }
    sb += L"Model-Name: " + Characters::ToString (fModelName) + L", ";
    if (fModelNumber) {
        sb += L"Model-Number: " + Characters::ToString (fModelNumber) + L", ";
    }
    if (fModelURL) {
        sb += L"Model-URL: " + Characters::ToString (*fModelURL) + L", ";
    }
    if (fSerialNumber) {
        sb += L"Serial-Number: " + Characters::ToString (fSerialNumber) + L", ";
    }
    sb += L"UDN: " + Characters::ToString (fUDN) + L", ";
    if (fUPC) {
        sb += L"UPC: " + Characters::ToString (fUPC) + L", ";
    }
    sb += L"Icons: " + Characters::ToString (fIcons) + L", ";
    sb += L"Services: " + Characters::ToString (fServices) + L", ";
    sb += L"}";
    return sb.str ();
}

 ObjectVariantMapper DeviceDescription::mkMapper_ ()
{
    ObjectVariantMapper mapper;

DISABLE_COMPILER_MSC_WARNING_START (4573);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
    mapper.AddClass<Icon> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {L"Alive", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Icon, fMimeType)},
        {L"USN", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Icon, fHorizontalPixels)},
        {L"Server", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Icon, fVerticalPixels)},
        {L"Target", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Icon, fColorDepth)},
        {L"RawHeaders", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Icon, fURL)},
    });
    mapper.AddCommonType<Collection<Icon>> ();
    mapper.AddCommonType<optional<Collection<Icon>>> ();

    mapper.AddClass<Service> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {L"Alive", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Service, fServiceType)},
        {L"USN", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Service, fServiceID)},
        {L"Server", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Service, fSCPDURL)},
        {L"Target", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Service, fControlURL)},
        {L"RawHeaders", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Service, fEventSubURL)},
    });
    mapper.AddCommonType<Collection<Service>> ();
    mapper.AddCommonType<optional<Collection<Service>>> ();

    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<URI>> ();

    mapper.AddClass<DeviceDescription> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {L"PresentationURL", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fPresentationURL)},
        {L"DeviceType", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fDeviceType)},
        {L"ManufactureName", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fManufactureName)},
        {L"FriendlyName", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fFriendlyName)},
        {L"ManufacturingURL", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fManufacturingURL)},
        {L"ModelDescription", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelDescription)},
        {L"ModelName", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelName)},
        {L"ModelNumber", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelNumber)},
        {L"ModelURL", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelURL)},
        {L"SerialNumber", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fSerialNumber)},
        {L"UDN", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fUDN)},
        {L"UPC", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fUPC)},
        {L"Icons", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fIcons)},
        {L"Services", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fServices)},
    });
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
    DISABLE_COMPILER_MSC_WARNING_END (4573);
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
        for (DeviceDescription::Icon i : *dd.fIcons) {
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
        for (DeviceDescription::Service i : *dd.fServices) {
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
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        registry.AddCommonReader_Class<DeviceDescription::Icon> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"mimetype"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fMimeType)},
            {Name{L"width"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fHorizontalPixels)},
            {Name{L"height"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fVerticalPixels)},
            {Name{L"depth"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fColorDepth)},
            {Name{L"url"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Icon, fURL)},
        });
        registry.AddCommonReader_Class<DeviceDescription::Service> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"serviceType"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fServiceType)},
            {Name{L"serviceId"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fServiceID)},
            {Name{L"SCPDURL"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fSCPDURL)},
            {Name{L"controlURL"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription::Service, fControlURL)},
        });
        registry.AddCommonType<Collection<DeviceDescription::Icon>> (Name{L"icon"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Icon>>> ();
        registry.AddCommonType<Collection<DeviceDescription::Service>> (Name{L"service"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Service>>> ();
        registry.AddCommonReader_Class<DeviceDescription> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{L"presentationURL"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fPresentationURL)},
            {Name{L"deviceType"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fDeviceType)},
            {Name{L"manufacturer"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fManufactureName)},
            {Name{L"friendlyName"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fFriendlyName)},
            {Name{L"manufacturerURL"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fManufacturingURL)},
            {Name{L"modelDescription"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelDescription)},
            {Name{L"modelName"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelName)},
            {Name{L"modelNumber"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelNumber)},
            {Name{L"modelURL"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fModelURL)},
            {Name{L"serialNum"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fSerialNumber)},
            {Name{L"UDN"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fUDN)},
            {Name{L"UPC"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fUPC)},
            {Name{L"iconList"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fIcons)},
            {Name{L"serviceList"sv}, Stroika_Foundation_DataExchange_StructFieldMetaInfo (DeviceDescription, fServices)},
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return registry;
    }();

    DeviceDescription deviceDescription;
    {
        ObjectReader::IConsumerDelegateToContext ctx{kTypesRegistry_, make_shared<ObjectReader::ReadDownToReader> (kTypesRegistry_.MakeContextReader (&deviceDescription), Name{L"device"})};
        XML::SAXParse (b, ctx);
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"xml data: %s", Streams::TextReader::New (b).ReadAll ().c_str ());
    DbgTrace (L"deviceDescription: %s", Characters::ToString (deviceDescription).c_str ());
#endif
    return deviceDescription;
}
