/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/Characters/Format.h"
#include "../../../Foundation/Characters/StringBuilder.h"
#include "../../../Foundation/Characters/ToString.h"
#include "../../../Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include "../../../Foundation/Streams/MemoryStream.h"
#include "../../../Foundation/Streams/TextReader.h"
#include "../../../Foundation/Streams/TextWriter.h"

#include "Common.h"

#include "Advertisement.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;

/*
 ********************************************************************************
 ****************************** SSDP::Advertisement *****************************
 ********************************************************************************
 */
String Advertisement::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    if (fAlive) {
        sb << "Alive : "sv << Characters::ToString (fAlive) << ", "sv;
    }
    sb << "USN : "sv << Characters::ToString (fUSN) << ", "sv;
    sb << "Location : "sv << Characters::ToString (fLocation) << ", "sv;
    sb << "Server : "sv << Characters::ToString (fServer) << ", "sv;
    sb << "Target : "sv << Characters::ToString (fTarget) << ", "sv;
    sb << "Raw-Headers : "sv << Characters::ToString (fRawHeaders);
    sb << "}"sv;
    return sb.str ();
}

ObjectVariantMapper Advertisement::kMapperGetter_ ()
{
    ObjectVariantMapper mapper;
    mapper.AddCommonType<Set<String>> ();
    mapper.AddCommonType<optional<Set<String>>> ();
    mapper.AddClass<Advertisement> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {"Alive"sv, StructFieldMetaInfo{&Advertisement::fAlive}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
        {"USN"sv, StructFieldMetaInfo{&Advertisement::fUSN}},
        {"Server"sv, StructFieldMetaInfo{&Advertisement::fServer}},
        {"Target"sv, StructFieldMetaInfo{&Advertisement::fTarget}},
        {"RawHeaders"sv, StructFieldMetaInfo{&Advertisement::fRawHeaders}},
    });
    return mapper;
};

/*
 ********************************************************************************
 ******************************* SSDP::Serialize ********************************
 ********************************************************************************
 */
Memory::BLOB SSDP::Serialize (const String& headLine, SearchOrNotify searchOrNotify, const Advertisement& ad)
{
    Require (not headLine.Contains ("\n"));
    Require (not headLine.Contains ("\r"));
    Require (headLine.StartsWith ("NOTIFY") or (headLine == "HTTP/1.1 200 OK"));
    Streams::MemoryStream::Ptr<byte> out     = Streams::MemoryStream::New<byte> ();
    Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDontInclude);

    //// SUPER ROUGH FIRST DRAFT
    textOut.Write (Characters::Format (L"%s\r\n", headLine.As<wstring> ().c_str ()));
    textOut.Write (Characters::Format (L"Host: %s:%d\r\n", SSDP::V4::kSocketAddress.GetInternetAddress ().As<String> ().As<wstring> ().c_str (),
                                       SSDP::V4::kSocketAddress.GetPort ()));
    textOut.Write (Characters::Format (L"Cache-Control: max-age=60\r\n")); // @todo fix
    textOut.Write (Characters::Format (L"Location: %s\r\n", ad.fLocation.As<String> ().As<wstring> ().c_str ()));
    if (ad.fAlive.has_value ()) {
        if (*ad.fAlive) {
            textOut.Write ("NTS: ssdp:alive\r\n"sv);
        }
        else {
            textOut.Write ("NTS: ssdp:byebye\r\n"sv);
        }
    }
    if (not ad.fServer.empty ()) {
        textOut.Write (Characters::Format (L"Server: %s\r\n", ad.fServer.As<wstring> ().c_str ()));
    }
    if (searchOrNotify == SearchOrNotify::SearchResponse) {
        textOut.Write (Characters::Format (L"ST: %s\r\n", ad.fTarget.As<wstring> ().c_str ()));
    }
    else {
        textOut.Write (Characters::Format (L"NT: %s\r\n", ad.fTarget.As<wstring> ().c_str ()));
    }
    textOut.Write (Characters::Format (L"USN: %s\r\n", ad.fUSN.As<wstring> ().c_str ()));

    // Terminate list of headers
    textOut.Write ("\r\n"sv);

    // need flush API on

    return out.As<Memory::BLOB> ();
}

/*
 ********************************************************************************
 **************************** SSDP::DeSerialize *********************************
 ********************************************************************************
 */
void SSDP::DeSerialize (const Memory::BLOB& b, String* headLine, Advertisement* advertisement)
{
    RequireNotNull (headLine);
    RequireNotNull (advertisement);
    *advertisement = Advertisement{};

    TextReader::Ptr in = TextReader::New (ExternallyOwnedMemoryInputStream::New<byte> (b.begin (), b.end ()));

    *headLine = in.ReadLine ().Trim ();
    while (true) {
        String line = in.ReadLine ().Trim ();
        if (line.empty ()) {
            break;
        }

        // Need to simplify this code (stroika string util)
        String label;
        String value;
        if (optional<size_t> n = line.Find (':')) {
            label = line.SubString (0, *n);
            value = line.SubString (*n + 1).Trim ();
        }
        if (not label.empty ()) {
            advertisement->fRawHeaders.Add (label, value);
        }
        constexpr auto kLabelComparer_ = String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive};
        if (kLabelComparer_ (label, "Location"sv) == 0) {
            advertisement->fLocation = IO::Network::URI{value};
        }
        else if (kLabelComparer_ (label, "NT"sv) == 0) {
            advertisement->fTarget = value;
        }
        else if (kLabelComparer_ (label, "USN"sv) == 0) {
            advertisement->fUSN = value;
        }
        else if (kLabelComparer_ (label, "Server"sv) == 0) {
            advertisement->fServer = value;
        }
        else if (kLabelComparer_ (label, "NTS"sv) == 0) {
            constexpr auto kValueComparer_ = String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive};
            if (kValueComparer_ (value, "ssdp:alive"sv) == 0) {
                advertisement->fAlive = true;
            }
            else if (kValueComparer_ (value, "ssdp:byebye"sv) == 0) {
                advertisement->fAlive = false;
            }
        }
    }
}
