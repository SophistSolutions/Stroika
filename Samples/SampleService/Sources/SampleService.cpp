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
///         (we have TODO adding wrapper of service backend which writes to syslog. Use that).
//          OR - maybe - just always do logging. Thats probably better, and have a flag in service wrapper
//          about log level? No - maybe thats obvihs too. Mabe just upagrade service code to use logger!!!
//          I think that would always - or nearly always - be desirable. Maybe make configurable. But no need
//          as a user can always use their own servcie impl!
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

#if 0
usage --start, or --stop or --status or --restart
#endif

int main (int argc, const char* argv[])
{
    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    shared_ptr<Main::IServiceIntegrationRep>    serviceIntegrationRep   =   Main::mkDefaultServiceIntegrationRep ();
    if (args.ApplyUntilTrue ([] (const String & i) { return Execution::MatchesCommandLineArgument (i, L"run2Idle"); })) {
        // note - cannot use 'contains' because we need MatchesCommandLineArgument matching...
        serviceIntegrationRep = shared_ptr<Main::IServiceIntegrationRep> (new Main::RunTilIdleService ());
    }
    Main    m (shared_ptr<AppRep_> (new AppRep_ ()), serviceIntegrationRep);
#if 1
    if (Execution::MatchesCommandLineArgument (args, L"status")) {
        cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
        return EXIT_SUCCESS;
    }
    else if (Execution::MatchesCommandLineArgument (args, L"help")) {
        //ShowUsage_ ();
        return EXIT_SUCCESS;
    }
#else
    if (args.ApplyUntilTrue ([] (const String & i) { return Execution::MatchesCommandLineArgument (i, L"status"); })) {
        cout << m.GetServiceStatusMessage ().AsUTF8<string> ();
        return EXIT_SUCCESS;
    }
    else if (args.ApplyUntilTrue ([] (const String & i) { return Execution::MatchesCommandLineArgument (i, L"help"); })) {
        //ShowUsage_ ();
        return EXIT_SUCCESS;
    }
#endif
    try {
        m.Run (args);
    }
    catch (const std::exception& e) {
        cerr << "FAILED: (std::exception): '" << e.what () << endl;
        return EXIT_FAILURE;
    }
    catch (const Execution::StringException& e) {
        cerr << "FAILED: (Execution::StringException): '" << Characters::WideStringToNarrowSDKString (e.As<wstring> ()) << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
