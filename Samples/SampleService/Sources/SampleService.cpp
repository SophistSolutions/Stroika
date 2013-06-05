/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/Service/Main.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::Service;

using   Characters::String;
using   Memory::Optional;


namespace {
    struct   AppRep_ : Main::IApplicationRep {
        AppRep_ () {
        }
        virtual ~AppRep_ () {
        }

    public:
        virtual void                MainLoop () override {
        }
        virtual Main::ServiceDescription  GetServiceDescription () const override {
            Main::ServiceDescription    t;
            return t;
        }
    };
}


int main (int argc, const char* argv[])
{
    bool    listen  =   false;
    Optional<String>    searchFor;

    vector<String>  args    =   Execution::ParseCommandLine (argc, argv);
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
    if (not listen and searchFor.empty ()) {
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR]" << endl;
        return EXIT_FAILURE;
    }

    // must fix to pass right backend
    Main    m (shared_ptr<AppRep_> (new AppRep_ ()), shared_ptr<Main::IServiceIntegrationRep> (new Main::RunTilIdleService ()));
#if 0
    try {
        Listener l;
        if (listen) {
            DoListening_ (&l);
        }
        Search  s;
        if (not searchFor.empty ()) {
            DoSearching_ (&s, *searchFor);
        }

        if (listen or not searchFor.empty ()) {
            Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c
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
#endif

    return EXIT_SUCCESS;
}
