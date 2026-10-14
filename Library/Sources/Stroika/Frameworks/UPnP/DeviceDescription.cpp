/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#if qStroika_Foundation_DataExchange_XML_SupportDOM
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#endif

#include "DeviceDescription.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

namespace {
    // the namespace every element of a device description is in (UPnP Device Architecture 1.1, section 2.3)
    const IO::Network::URI kDeviceNamespace_{"urn:schemas-upnp-org:device-1-0"sv};
}

/*
 ********************************************************************************
 ************************* DeviceDescription::Icon ******************************
 ********************************************************************************
 */
String DeviceDescription::Icon::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "mimeType: "sv << fMimeType;
    sb << ", horizontalPixels: "sv << fHorizontalPixels;
    sb << ", nerticalPixels: "sv << fVerticalPixels;
    sb << ", colorDepth : "sv << fColorDepth;
    sb << ", URL: "sv << fURL;
    sb << "}"sv;
    return sb;
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
    sb << "Service-Type: "sv << fServiceType;
    sb << ", Service-ID: "sv << fServiceID;
    sb << ", SCPD-URL: "sv << fSCPDURL;
    sb << ", Control-URL: "sv << fControlURL;
    sb << ", Event-Sub-URL: "sv << fEventSubURL;
    sb << "}"sv;
    return sb;
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
        sb << "Presentation-URL: "sv << fPresentationURL;
    }
    if (fURLBase) {
        sb << ", URL-Base: "sv << *fURLBase;
    }
    sb << ", Device-Type: "sv << fDeviceType;
    sb << ", Manufacture-Name: "sv << fManufactureName;
    sb << ", Friendly-Name: "sv << fFriendlyName;
    if (fManufacturingURL) {
        sb << ", Manufacturing-URL: "sv << fManufacturingURL;
    }
    if (fModelDescription) {
        sb << ", Model-Description: "sv << fModelDescription;
    }
    sb << ", Model-Name: "sv << fModelName;
    if (fModelNumber) {
        sb << ", Model-Number: "sv << fModelNumber;
    }
    if (fModelURL) {
        sb << ", Model-URL: "sv << *fModelURL;
    }
    if (fSerialNumber) {
        sb << ", Serial-Number: "sv << fSerialNumber;
    }
    sb << ", UDN: "sv << fUDN;
    if (fUPC) {
        sb << ", UPC: "sv << fUPC;
    }
    if (fIcons) {
        sb << ", Icons: "sv << fIcons;
    }
    if (fServices) {
        sb << ", Services: "sv << fServices;
    }
    sb << "}"sv;
    return sb;
}

ObjectVariantMapper DeviceDescription::mkMapper_ ()
{
    ObjectVariantMapper mapper;

    mapper.AddClass<Icon> ({
        {"mimetype"sv, &Icon::fMimeType},
        {"width"sv, &Icon::fHorizontalPixels},
        {"height"sv, &Icon::fVerticalPixels},
        {"depth"sv, &Icon::fColorDepth},
        {"url"sv, &Icon::fURL},
    });
    mapper.AddCommonType<Collection<Icon>> ();
    mapper.AddCommonType<optional<Collection<Icon>>> ();

    mapper.AddClass<Service> ({
        {"serviceType"sv, &Service::fServiceType},
        {"serviceId"sv, &Service::fServiceID},
        {"SCPDURL"sv, &Service::fSCPDURL},
        {"controlURL"sv, &Service::fControlURL},
        {"eventSubURL"sv, &Service::fEventSubURL},
    });
    mapper.AddCommonType<Collection<Service>> ();
    mapper.AddCommonType<optional<Collection<Service>>> ();

    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<URI>> ();

    mapper.AddClass<DeviceDescription> ({
        {"URLBase"sv, &DeviceDescription::fURLBase},
        {"presentationURL"sv, &DeviceDescription::fPresentationURL},
        {"deviceType"sv, &DeviceDescription::fDeviceType},
        {"manufacturer"sv, &DeviceDescription::fManufactureName},
        {"friendlyName"sv, &DeviceDescription::fFriendlyName},
        {"manufacturerURL"sv, &DeviceDescription::fManufacturingURL},
        {"modelDescription"sv, &DeviceDescription::fModelDescription},
        {"modelName"sv, &DeviceDescription::fModelName},
        {"modelNumber"sv, &DeviceDescription::fModelNumber},
        {"modelURL"sv, &DeviceDescription::fModelURL},
        {"serialNumber"sv, &DeviceDescription::fSerialNumber},
        {"UDN"sv, &DeviceDescription::fUDN},
        {"UPC"sv, &DeviceDescription::fUPC},
        {"iconList"sv, &DeviceDescription::fIcons},
        {"serviceList"sv, &DeviceDescription::fServices},
    });
    return mapper;
};

/*
 ********************************************************************************
 ********************************* UPnP::Serialize ******************************
 ********************************************************************************
 */
Memory::BLOB UPnP::Serialize ([[maybe_unused]] const DeviceDescription& dd, [[maybe_unused]] const Options& options)
{
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    using namespace DataExchange::XML;
    namespace Element = DataExchange::XML::DOM::Element;
    auto urlOf        = [] (const optional<URI>& u) -> optional<String> { return u == nullopt ? optional<String>{} : u->As<String> (); };
    // each element, in the order UPnP Device Architecture 1.1, section 2.3, gives them. A name with no namespace of its own is
    // in the document's, declared once on <root> and inherited - which is what UPnP's schema says of every one of these
    DOM::Document::Ptr doc  = DOM::Document::New (options.GetXMLProvider (), NameWithNamespace{kDeviceNamespace_, "root"sv});
    Element::Ptr       root = doc.GetRootElement ();
    Element::Ptr       spec = root.Append ("specVersion"sv); // 1.0, not 1.1, on purpose - @see this framework's ReadMe.md
    spec.Append ("major"sv, "1"sv);
    spec.Append ("minor"sv, "0"sv);
    root.AppendIf ("URLBase"sv, urlOf (dd.fURLBase));
    Element::Ptr device = root.Append ("device"sv);
    device.Append ("deviceType"sv, dd.fDeviceType);
    device.Append ("friendlyName"sv, dd.fFriendlyName);
    device.Append ("manufacturer"sv, dd.fManufactureName);
    device.AppendIf ("manufacturerURL"sv, urlOf (dd.fManufacturingURL));
    device.AppendIf ("modelDescription"sv, dd.fModelDescription);
    device.Append ("modelName"sv, dd.fModelName);
    device.AppendIf ("modelNumber"sv, dd.fModelNumber);
    device.AppendIf ("modelURL"sv, urlOf (dd.fModelURL));
    device.AppendIf ("serialNumber"sv, dd.fSerialNumber);
    device.Append ("UDN"sv, dd.fUDN);
    device.AppendIf ("UPC"sv, dd.fUPC);
    if (dd.fIcons) {
        Element::Ptr iconList = device.Append ("iconList"sv);
        for (const DeviceDescription::Icon& i : *dd.fIcons) {
            Element::Ptr icon = iconList.Append ("icon"sv);
            icon.Append ("mimetype"sv, i.fMimeType.As<String> ());
            icon.Append ("width"sv, Characters::ToString (i.fHorizontalPixels));
            icon.Append ("height"sv, Characters::ToString (i.fVerticalPixels));
            icon.Append ("depth"sv, Characters::ToString (i.fColorDepth));
            icon.Append ("url"sv, i.fURL.As<String> ());
        }
    }
    if (dd.fServices) {
        Element::Ptr serviceList = device.Append ("serviceList"sv);
        for (const DeviceDescription::Service& i : *dd.fServices) {
            Element::Ptr service = serviceList.Append ("service"sv);
            service.Append ("serviceType"sv, i.fServiceType);
            service.Append ("serviceId"sv, i.fServiceID);
            service.Append ("SCPDURL"sv, i.fSCPDURL.As<String> ());
            service.Append ("controlURL"sv, i.fControlURL.As<String> ());
            service.Append ("eventSubURL"sv, i.fEventSubURL.As<String> ());
        }
    }
    // a <deviceList> of embedded devices would go here, in this order - but DeviceDescription has no field to write one from
    device.AppendIf ("presentationURL"sv, urlOf (dd.fPresentationURL));
    Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
    doc.Write (out, DOM::SerializationOptions{.fPrettyPrint = true});
    return out.As<Memory::BLOB> ();
#else
    Execution::Throw (Execution::Exception<runtime_error>{"A device description cannot be written: this build has no XML DOM"sv});
#endif
}

/*
 ********************************************************************************
 ******************************* UPnP::DeSerialize ******************************
 ********************************************************************************
 */
DeviceDescription UPnP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, [[maybe_unused]] const Options& options)
{
    using namespace Stroika::Foundation::DataExchange;
    using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
    using namespace Stroika::Foundation::DataExchange::XML;

    // read from <root>, not just <device>: <URLBase>, where there is one, is <device>'s sibling
    struct Root_ {
        optional<String>  fURLBase; // not optional<URI>: that reader returns an empty or bad one as an EMPTY URI, not nullopt
        DeviceDescription fDevice;
    };
    static const ObjectReader::Registry kTypesRegistry_ = [] () {
        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonType<optional<String>> ();
        registry.AddCommonType<uint16_t> ();
        registry.AddCommonType<URI> ();
        registry.AddCommonType<optional<URI>> ();
        registry.AddCommonReader_Simple<InternetMediaType> ([] (const String& s) { return InternetMediaType{s}; });
        registry.AddCommonReader_Class<DeviceDescription::Icon> ({
            {Name{"mimetype"sv}, &DeviceDescription::Icon::fMimeType},
            {Name{"width"sv}, &DeviceDescription::Icon::fHorizontalPixels},
            {Name{"height"sv}, &DeviceDescription::Icon::fVerticalPixels},
            {Name{"depth"sv}, &DeviceDescription::Icon::fColorDepth},
            {Name{"url"sv}, &DeviceDescription::Icon::fURL},
        });
        registry.AddCommonReader_Class<DeviceDescription::Service> ({
            {Name{"serviceType"sv}, &DeviceDescription::Service::fServiceType},
            {Name{"serviceId"sv}, &DeviceDescription::Service::fServiceID},
            {Name{"SCPDURL"sv}, &DeviceDescription::Service::fSCPDURL},
            {Name{"controlURL"sv}, &DeviceDescription::Service::fControlURL},
            {Name{"eventSubURL"sv}, &DeviceDescription::Service::fEventSubURL},
        });
        registry.AddCommonType<Collection<DeviceDescription::Icon>> (Name{"icon"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Icon>>> ();
        registry.AddCommonType<Collection<DeviceDescription::Service>> (Name{"service"});
        registry.AddCommonType<optional<Collection<DeviceDescription::Service>>> ();
        registry.AddCommonReader_Class<DeviceDescription> ({
            {Name{"presentationURL"sv}, &DeviceDescription::fPresentationURL},
            {Name{"deviceType"sv}, &DeviceDescription::fDeviceType},
            {Name{"manufacturer"sv}, &DeviceDescription::fManufactureName},
            {Name{"friendlyName"sv}, &DeviceDescription::fFriendlyName},
            {Name{"manufacturerURL"sv}, &DeviceDescription::fManufacturingURL},
            {Name{"modelDescription"sv}, &DeviceDescription::fModelDescription},
            {Name{"modelName"sv}, &DeviceDescription::fModelName},
            {Name{"modelNumber"sv}, &DeviceDescription::fModelNumber},
            {Name{"modelURL"sv}, &DeviceDescription::fModelURL},
            {Name{"serialNumber"sv}, &DeviceDescription::fSerialNumber},
            {Name{"UDN"sv}, &DeviceDescription::fUDN},
            {Name{"UPC"sv}, &DeviceDescription::fUPC},
            {Name{"iconList"sv}, &DeviceDescription::fIcons},
            {Name{"serviceList"sv}, &DeviceDescription::fServices},
        });
        registry.AddCommonReader_Class<Root_> ({
            {Name{"URLBase"sv}, &Root_::fURLBase},
            {Name{"device"sv}, &Root_::fDevice},
        });
        return registry;
    }();

    Root_ root;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"xml data: %s", Streams::BinaryToText::Reader::New (b).ReadAll ().c_str ());
#endif
#if qStroika_Foundation_DataExchange_XML_SupportParsing
    {
        ObjectReader::IConsumerDelegateToContext ctx{
            kTypesRegistry_, MakeSharedPtr<ObjectReader::ReadDownToReader> (kTypesRegistry_.MakeContextReader (&root), Name{"root"sv})};
        XML::SAXParse (options.GetXMLProvider (), b, &ctx, options.fValidateReadAgainst);
    }
#else
    WeakAssertNotImplemented (); // may want to allow to continue, as this may not be critical functionality, but you probably want XML parser when running this code...
#endif
    DeviceDescription deviceDescription = move (root.fDevice);
    // an empty <URLBase> means none (UPnP Device Architecture 1.0); one that is not a URL is taken for none too - ignored, as
    // every <URLBase> was before Stroika v3.0d25 - rather than failing the whole description
    if (root.fURLBase) {
        if (String u = root.fURLBase->Trim (); not u.empty ()) {
            try {
                deviceDescription.fURLBase = URI::Parse (u);
            }
            catch (...) {
                DbgTrace ("Ignoring <URLBase>{}</URLBase>, which is not a URL: {}"_f, u, current_exception ());
            }
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("deviceDescription: {}"_f, deviceDescription);
#endif
    return deviceDescription;
}
