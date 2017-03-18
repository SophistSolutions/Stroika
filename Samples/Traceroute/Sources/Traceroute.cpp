/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/NetworkMonitor/Ping.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Time;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMontior;

using Characters::String;
using Containers::Collection;
using Containers::Sequence;
using Memory::Optional;

namespace {
    mutex kStdOutMutex_; // If the listener impl uses multiple listen threads, prevent display from getting messed up
}

int main (int argc, const char* argv[])
{
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif

    Sequence<String> args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"l")) {
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"s")) {
            ++argi;
            if (argi != args.end ()) {
            }
            else {
                cerr << "Expected arg to -s" << endl;
                return EXIT_FAILURE;
            }
        }
    }

    try {
        Collection<InternetAddress> addrList = IO::Network::DNS::Default ().GetHostAddresses (L"www.sophists.com");
        if (addrList.empty ()) {
            Execution::Throw (Execution::StringException (L"whopse no addrs"));
        }
        InternetAddress addr = addrList.Nth (0);
        Duration        t    = NetworkMontior::Ping (addr);
        cout << "Ping to " << addr.ToString ().AsNarrowSDKString () << ": " << t.PrettyPrint ().AsNarrowSDKString () << endl;
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
