/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/Format.h"
#include    "../../../Foundation/Streams/BasicBinaryOutputStream.h"
#include    "../../../Foundation/Streams/TextOutputStreamBinaryAdapter.h"

#include    "Common.h"

#include    "Advertisement.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;





Memory::BLOB        SSDP::Serialize (const String& headLine, const Advertisement& ad)
{
    Require (not headLine.Contains (L"\n"));
    Require (not headLine.Contains (L"\r"));
    Require (headLine.StartsWith (L"NOTIFY") or (headLine == L"HTTP/1.1 200 OK"));
    Streams::BasicBinaryOutputStream    out;
    Streams::TextOutputStreamBinaryAdapter  textOut (out, Streams::TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM);

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
    textOut.Write (Characters::Format (L"NT: %s\r\n", ad.fST.c_str ()));
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
}
