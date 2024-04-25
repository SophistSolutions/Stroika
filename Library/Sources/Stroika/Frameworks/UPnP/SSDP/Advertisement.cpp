/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Streams/TextWriter.h"

#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

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
        sb << "Alive : "sv << fAlive << ", "sv;
    }
    sb << "USN : "sv << fUSN << ", "sv;
    sb << "Location : "sv << fLocation << ", "sv;
    sb << "Server : "sv << fServer << ", "sv;
    sb << "Target : "sv << fTarget << ", "sv;
    sb << "Raw-Headers : "sv << fRawHeaders;
    sb << "}"sv;
    return sb;
}

ObjectVariantMapper Advertisement::kMapperGetter_ ()
{
    ObjectVariantMapper mapper;
    mapper.AddCommonType<Set<String>> ();
    mapper.AddCommonType<optional<Set<String>>> ();
    mapper.AddClass<Advertisement> ({
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
    using namespace Characters::Literals;
    Require (not headLine.Contains ("\n"));
    Require (not headLine.Contains ("\r"));
    Require (headLine.StartsWith ("NOTIFY") or (headLine == "HTTP/1.1 200 OK"));
    Streams::MemoryStream::Ptr<byte> out     = Streams::MemoryStream::New<byte> ();
    Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDontInclude);

    //// SUPER ROUGH FIRST DRAFT
    textOut.Write ("{}\r\n"_f(headLine));
    textOut.Write ("Host: {}:{}\r\n"_f(SSDP::V4::kSocketAddress.GetInternetAddress (), SSDP::V4::kSocketAddress.GetPort ()));
    textOut.Write ("Cache-Control: max-age=60\r\n"sv); // @todo fix
    textOut.Write ("Location: {}\r\n"_f(ad.fLocation));
    if (ad.fAlive.has_value ()) {
        if (*ad.fAlive) {
            textOut.Write ("NTS: ssdp:alive\r\n"sv);
        }
        else {
            textOut.Write ("NTS: ssdp:byebye\r\n"sv);
        }
    }
    if (not ad.fServer.empty ()) {
        textOut.Write ("Server: {}\r\n"_f(ad.fServer));
    }
    if (searchOrNotify == SearchOrNotify::SearchResponse) {
        textOut.Write ("ST: {}\r\n"_f(ad.fTarget));
    }
    else {
        textOut.Write ("NT: {}\r\n"_f(ad.fTarget));
    }
    textOut.Write ("USN: {}\r\n"_f(ad.fUSN));

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

#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
    TextReader::Ptr in = TextReader::New (ExternallyOwnedSpanInputStream::New<byte> (span<const byte>{b}));
#else
    TextReader::Ptr in = TextReader::New (ExternallyOwnedSpanInputStream::New<byte> (span{b}));
#endif

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
