/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/IO/Network/NetworkInterfaces.h"

#include    "Stroika/Frameworks/WebServer/ConnectionManager.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::WebServer;

using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;




int main (int argc, const char* argv[])
{
    try {
        ConnectionManager   cm;
        //d.fLocation = String (L"http://") + IO::Network::GetPrimaryInternetAddress ().As<String> ();
        Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
