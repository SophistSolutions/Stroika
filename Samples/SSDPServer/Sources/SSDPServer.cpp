/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Server/PeriodicNotifier.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::UPnP;
using   namespace Stroika::Frameworks::UPnP::SSDP;

using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;
using   Server::PeriodicNotifier;




int main (int argc, const char* argv[])
{
    bool    listen  =   false;
    Optional<String>    searchFor;

    try {
        Device  d;
        d.fLocation = L"http://PITIPERE";
        d.fServer = L"sample-stroika-ssdp-server";
        d.fUSN = L"uuid:TESTUID";
        d.fST = L"upnp:rootdevice";
        PeriodicNotifier l;
        l.Run (d, PeriodicNotifier::FrequencyInfo ());
        Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
