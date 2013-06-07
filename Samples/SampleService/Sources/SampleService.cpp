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
using   Containers::Sequence;



/// *** TODO ****
/// add option to log to LOGGER instead of stderr!!!
//




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
    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    shared_ptr<Main::IServiceIntegrationRep>    serviceIntegrationRep;
    {
        bool    run2IdleMode = false;
        // redo using sequence and lamnda code
        for (String i : args) {
            if (Execution::MatchesCommandLineArgument (i, L"run2Idle")) {
                run2IdleMode = true;
            }
        }
        if (run2IdleMode) {
            serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::RunTilIdleService ());
        }
        else {
            serviceIntegrationRep = Main::mkDefaultServiceIntegrationRep ();
        }
    }
    Main    m (shared_ptr<AppRep_> (new AppRep_ ()), serviceIntegrationRep);
    try {
        m.Run (args);
    }
    catch (const std::exception& e) {
        cerr << "FAILED: REGRESSION TEST (std::exception): '" << e.what () << endl;
        cout << "Failed" << endl;
        DbgTrace ("FAILED: REGRESSION TEST (std::exception): '%s", e.what ());
        return EXIT_FAILURE;
    }
    catch (const Execution::StringException& e) {
        cerr << "FAILED: REGRESSION TEST (Execution::StringException): '" << Characters::WideStringToNarrowSDKString (e.As<wstring> ()) << endl;
        cout << "Failed" << endl;
        DbgTrace (L"FAILED: REGRESSION TEST (std::exception): '%s", e.As<wstring> ().c_str ());
        return EXIT_FAILURE;
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
