/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/Service/Main.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::Service;

using   Characters::String;
using   Memory::Optional;



/// *** TODO ****
/// add option to log to LOGGER instead of stderr!!!
//
//  replace kUseRunTilIdleServiceHandler_with something to check cmdline



namespace {
    // handy to test, but real users will want to point this to the default implementation (set to false)
    // or pull the value from a command line argument
    constexpr   bool    kUseRunTilIdleServiceHandler_   =   true;
}




namespace {
    struct   AppRep_ : Main::IApplicationRep {
        AppRep_ () {
        }
        virtual ~AppRep_ () {
        }

    public:
        virtual void  MainLoop () override {
            Execution::Sleep (1 * 24 * 60 * 60);    // wait 1 day ... simple test....
        }
        virtual Main::ServiceDescription  GetServiceDescription () const override {
            Main::ServiceDescription    t;
            t.fPrettyName = L"Test Service";
            t.fRegistrationName = L"Test-Service";
            return t;
        }
    };
}



int main (int argc, const char* argv[])
{
    vector<String>  args    =   Execution::ParseCommandLine (argc, argv);
    shared_ptr<Main::IServiceIntegrationRep>    serviceIntegrationRep;
    if (kUseRunTilIdleServiceHandler_) {
        serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::RunTilIdleService ());
    }
    else {
        serviceIntegrationRep = Main::mkDefaultServiceIntegrationRep ();
    }
    Main    m (shared_ptr<AppRep_> (new AppRep_ ()), serviceIntegrationRep);
    try {
        m.Run (args);
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
