/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/SystemPerformance/Measurement.h"
#include    "Stroika/Frameworks/SystemPerformance/MeasurementTypes.h"

using   namespace std;

using   namespace Stroika::Foundation;

using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;





int main (int argc, const char* argv[])
{
    bool    listen  =   false;
    Optional<String>    searchFor;

    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end(); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"l")) {
            listen = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"s")) {
            ++argi;
            if (argi != args.end ()) {
                searchFor = *argi;
            }
            else {
                cerr << "Expected arg to -s" << endl;
                return EXIT_FAILURE;
            }
        }
    }
    if (not listen and searchFor.IsMissing ()) {
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR]" << endl;
        return EXIT_FAILURE;
    }

    try {
        if (listen or searchFor.IsPresent ()) {
            Execution::WaitableEvent ().Wait ();    // wait forever - til user hits ctrl-c
        }
        else {
            cerr << "Specify -l to listen or -s STRING to search" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
