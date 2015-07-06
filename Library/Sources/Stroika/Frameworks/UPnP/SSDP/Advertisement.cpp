/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/Format.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"
#include    "../../../Foundation/Streams/TextReader.h"
#include    "../../../Foundation/Streams/TextWriter.h"

#include    "Common.h"

#include    "Advertisement.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;


using   Memory::Byte;



Memory::BLOB        SSDP::Serialize (const String& headLine, SearchOrNotify searchOrNotify, const Advertisement& ad)
{
    Require (not headLine.Contains (L"\n"));
    Require (not headLine.Contains (L"\r"));
    Require (headLine.StartsWith (L"NOTIFY") or (headLine == L"HTTP/1.1 200 OK"));
    Streams::MemoryStream<Byte>    out;
    Streams::TextWriter  textOut (out, Streams::TextWriter::Format::eUTF8WithoutBOM);

    //// SUPER ROUGH FIRST DRAFT
    textOut.Write (Characters::Format (L"%s\r\n", headLine.c_str ()));
    textOut.Write (Characters::Format (L"Host: %s:%d\r\n", SSDP::V4::kSocketAddress.GetInternetAddress ().As<String> ().c_str (), SSDP::V4::kSocketAddress.GetPort ()));
    textOut.Write (Characters::Format (L"Cache-Control: max-age=60\r\n"));    // @todo fix
    if (not ad.fLocation.empty ()) {
        textOut.Write (Characters::Format (L"Location: %s\r\n", ad.fLocation.c_str ()));
    }
    if (ad.fAlive.IsPresent ()) {
        if (*ad.fAlive) {
            textOut.Write (Characters::Format (L"NTS: ssdp:alive\r\n"));
        }
        else {
            textOut.Write (Characters::Format (L"NTS: ssdp:bye\r\n"));
        }
    }
    if (not ad.fServer.empty ()) {
        textOut.Write (Characters::Format (L"Server: %s\r\n", ad.fServer.c_str ()));
    }
    if (searchOrNotify == SearchOrNotify::SearchResponse) {
        textOut.Write (Characters::Format (L"ST: %s\r\n", ad.fTarget.c_str ()));
    }
    else {
        textOut.Write (Characters::Format (L"NT: %s\r\n", ad.fTarget.c_str ()));
    }
    textOut.Write (Characters::Format (L"USN: %s\r\n", ad.fUSN.c_str ()));

    // Terminate list of headers
    textOut.Write (L"\r\n");

    // need flush API on

    return out.As<Memory::BLOB> ();
}


/*
*/
void    SSDP::DeSerialize (const Memory::BLOB& b, String* headLine, Advertisement* advertisement)
{
    RequireNotNull (headLine);
    RequireNotNull (advertisement);
    *advertisement = Advertisement ();

    TextReader in = TextReader (ExternallyOwnedMemoryBinaryInputStream (b.begin (), b.end ()));

    *headLine = in.ReadLine ().Trim ();
    while (true) {
        String line = in.ReadLine ().Trim ();
        if (line.empty ()) {
            break;
        }

        // Need to simplify this code (stroika string util)
        String  label;
        String  value;
        {
            size_t n = line.Find (':');
            if (n != Characters::String::kBadIndex) {
                label = line.SubString (0, n);
                value = line.SubString (n + 1).Trim ();
            }
        }
        if (not label.empty ()) {
            advertisement->fRawHeaders.Add (label, value);
        }
        if (label.Compare (L"Location", Characters::CompareOptions::eCaseInsensitive) == 0) {
            advertisement->fLocation = value;
        }
        else if (label.Compare (L"NT", Characters::CompareOptions::eCaseInsensitive) == 0) {
            advertisement->fTarget = value;
        }
        else if (label.Compare (L"USN", Characters::CompareOptions::eCaseInsensitive) == 0) {
            advertisement->fUSN = value;
        }
        else if (label.Compare (L"Server", Characters::CompareOptions::eCaseInsensitive) == 0) {
            advertisement->fServer = value;
        }
        else if (label.Compare (L"NTS", Characters::CompareOptions::eCaseInsensitive) == 0) {
            if (value.Compare (L"ssdp:alive", Characters::CompareOptions::eCaseInsensitive) == 0) {
                advertisement->fAlive = true;
            }
            else if (value.Compare (L"ssdp:bye", Characters::CompareOptions::eCaseInsensitive) == 0) {
                advertisement->fAlive = false;
            }
        }
        else {
            int hreab = 1;
        }
    }
}
