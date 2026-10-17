/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Binding.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Writer.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

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

    using VVMap_ = Containers::Mapping<String, VariantValue>;

    // DeviceDescription::kMapper - its fields, by UPnP's names - with two of its conversions changed for a description's XML
    const ObjectVariantMapper& Mapper_ ()
    {
        static const ObjectVariantMapper kMapper_ = [] () {
            ObjectVariantMapper mapper = DeviceDescription::kMapper ();
            // a URL that is not one reads as an empty URL - as UPnP's reader always has - rather than failing the whole
            // description, as kMapper's would: devices in the field are loose
            mapper.Add<URI> ([] (const ObjectVariantMapper&, const URI* u) -> VariantValue { return VariantValue{u->As<String> ()}; },
                             [] (const ObjectVariantMapper&, const VariantValue& v, URI* u) {
                                 IgnoreExceptionsForCall (*u = URI::Parse (v.As<String> ()));
                             });
            // an icon's sizes as text, as the Binding declares them - so one that is not a number reads as 0, as UPnP's reader
            // always has, rather than failing the whole description (declared a number, the Reader would throw)
            mapper.Add<uint16_t> (
                [] (const ObjectVariantMapper&, const uint16_t* n) -> VariantValue { return VariantValue{Characters::ToString (*n)}; },
                [] (const ObjectVariantMapper&, const VariantValue& v, uint16_t* n) {
                    *n = Characters::String2Int<uint16_t> (v.As<String> ()); // 0 where it is not a number
                });
            return mapper;
        }();
        return kMapper_;
    }

    // the description's shape (UPnP Device Architecture 1.1, section 2.3): <root>'s specVersion, URLBase and <device>, their
    // elements' names and order, UPnP's namespace, each list and its items - and what no DeviceDescription field holds, a
    // vendor's own elements or an embedded <deviceList> say, skipped
    //
    // @todo derive this from Mapper_ (https://github.com/SophistSolutions/Stroika/issues/1217), or from UPnP's own schema
    //       (DeviceDescriptionSchema), rather than spelling it out: what only XML says - item names, the namespace,
    //       skipping - would be all that is left
    const Variant::XML::Binding& Binding_ ()
    {
        using Variant::XML::Binding;
        static const Binding kBinding_ = [] () {
            constexpr auto kSkip = Binding::UndeclaredMembers::eSkip;
            auto           text  = [] () { return Binding{.fType = VariantValue::eString}; };
            auto           list  = [] (const Binding& item) {
                return Binding{.fType              = VariantValue::eArray,
                               .fUndeclaredMembers = Binding::UndeclaredMembers::eSkip,
                               .fItems             = MakeSharedPtr<Binding> (item)};
            };
            Binding icon{.fName = "icon"sv,
                         .fType = VariantValue::eMap,
                         .fMembers = {{"mimetype"sv, text ()}, {"width"sv, text ()}, {"height"sv, text ()}, {"depth"sv, text ()}, {"url"sv, text ()}},
                         .fUndeclaredMembers = kSkip};
            Binding service{
                .fName = "service"sv,
                .fType = VariantValue::eMap,
                .fMembers = {{"serviceType"sv, text ()}, {"serviceId"sv, text ()}, {"SCPDURL"sv, text ()}, {"controlURL"sv, text ()}, {"eventSubURL"sv, text ()}},
                .fUndeclaredMembers = kSkip};
            // a <deviceList> of embedded devices comes after serviceList - skipped, as DeviceDescription has no field for one
            Binding device{.fType              = VariantValue::eMap,
                           .fMembers           = {{"deviceType"sv, text ()},
                                                  {"friendlyName"sv, text ()},
                                                  {"manufacturer"sv, text ()},
                                                  {"manufacturerURL"sv, text ()},
                                                  {"modelDescription"sv, text ()},
                                                  {"modelName"sv, text ()},
                                                  {"modelNumber"sv, text ()},
                                                  {"modelURL"sv, text ()},
                                                  {"serialNumber"sv, text ()},
                                                  {"UDN"sv, text ()},
                                                  {"UPC"sv, text ()},
                                                  {"iconList"sv, list (icon)},
                                                  {"serviceList"sv, list (service)},
                                                  {"presentationURL"sv, text ()}},
                           .fUndeclaredMembers = kSkip};
            Binding specVersion{.fType = VariantValue::eMap, .fMembers = {{"major"sv, text ()}, {"minor"sv, text ()}}, .fUndeclaredMembers = kSkip};
            Binding root{.fName              = XML::NameWithNamespace{kDeviceNamespace_, "root"sv},
                         .fType              = VariantValue::eMap,
                         .fMembers           = {{"specVersion"sv, specVersion}, {"URLBase"sv, text ()}, {"device"sv, device}},
                         .fUndeclaredMembers = kSkip};
            Ensure (root.IsComplete ()); // so what Serialize writes, DeSerialize reads back exactly
            return root;
        }();
        return kBinding_;
    }
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
Memory::BLOB UPnP::Serialize (const DeviceDescription& dd, [[maybe_unused]] const Options& options)
{
    // DeviceDescription is flat, where the document is not: <URLBase> is <device>'s sibling, in <root>
    VVMap_ device = Mapper_ ().FromObject (dd).As<VVMap_> ();
    // 1.0, not 1.1, on purpose - @see this framework's ReadMe.md
    VVMap_ root{{"specVersion"sv, VariantValue{VVMap_{{"major"sv, VariantValue{"1"sv}}, {"minor"sv, VariantValue{"0"sv}}}}}};
    if (optional<VariantValue> urlBase = device.Lookup ("URLBase"sv)) {
        root.Add ("URLBase"sv, *urlBase);
        device.Remove ("URLBase"sv);
    }
    root.Add ("device"sv, VariantValue{device});
    // the Writer writes the text itself, with no XML provider - so nothing in options applies
    static const Variant::XML::Writer kWriter_{{.fBinding = Binding_ ()}};
    return kWriter_.WriteAsBLOB (VariantValue{root});
}

/*
 ********************************************************************************
 ******************************* UPnP::DeSerialize ******************************
 ********************************************************************************
 */
DeviceDescription UPnP::DeSerialize (const Memory::BLOB& b, const Options& options)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("xml data: {}"_f, Streams::BinaryToText::Reader::New (b).ReadAll ());
#endif
    const Variant::XML::Reader reader{{.fBinding = Binding_ (), .fXMLOptions = options.fXMLOptions, .fValidateAgainst = options.fValidateReadAgainst}};
    VVMap_ root   = reader.Read (b.As<Streams::InputStream::Ptr<byte>> ()).As<VVMap_> ();
    VVMap_ device = root.LookupValue ("device"sv).As<VVMap_> ();
    // an empty <URLBase> means none (UPnP Device Architecture 1.0); one that is not a URL is taken for none too - ignored, as
    // every <URLBase> was before Stroika v3.0d25 - rather than failing the whole description
    if (optional<VariantValue> urlBase = root.Lookup ("URLBase"sv)) {
        if (String u = urlBase->As<String> ().Trim (); not u.empty ()) {
            try {
                (void)URI::Parse (u);
                device.Add ("URLBase"sv, VariantValue{u});
            }
            catch (...) {
                DbgTrace ("Ignoring <URLBase>{}</URLBase>, which is not a URL: {}"_f, u, current_exception ());
            }
        }
    }
    DeviceDescription deviceDescription = Mapper_ ().ToObject<DeviceDescription> (VariantValue{device});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("deviceDescription: {}"_f, deviceDescription);
#endif
    return deviceDescription;
}
