/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/NetworkMonitor/Ping.h"
#include "Stroika/Frameworks/NetworkMonitor/Traceroute.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;
using namespace Stroika::Foundation::Time;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMontior;

using Characters::String;
using Containers::Collection;
using Containers::Sequence;
using Memory::Optional;

int main (int argc, const char* argv[])
{
    String targetAddress;
    enum class MajorOp {
        ePing,
        eTraceroute,
    };
    MajorOp               majorOp     = MajorOp::eTraceroute;
    unsigned int          maxHops     = Ping::Options::kDefaultMaxHops;
    unsigned int          sampleCount = 3;
    static const Duration kInterSampleTime_{"PT.1S"};
    size_t                packetSize = Ping::Options::kDefaultPayloadSize + sizeof (ICMP::PacketHeader); // historically, the app ping has measured this including ICMP packet header, but not ip packet header size
    auto usage = [](const Optional<String>& extraArg = {}) {
        if (extraArg) {
            cerr << extraArg->AsNarrowSDKString () << endl;
        }
        cerr << "traceroute [--ping][--traceroute][--packetSize NNN][--maxHops NNN][--sampleCount N]  target-address" << endl;
        cerr << "   if sampleCount is 1, exception details shown, otherwise just summary of number of exceptions" << endl;
    };
    {
        Sequence<String> args = Execution::ParseCommandLine (argc, argv);
        for (auto argi = args.begin (); argi != args.end (); ++argi) {
            if (Execution::MatchesCommandLineArgument (*argi, L"ping")) {
                majorOp = MajorOp::ePing;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"traceroute")) {
                majorOp = MajorOp::eTraceroute;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"packetSize")) {
                ++argi;
                if (argi != args.end ()) {
                    packetSize = Characters::String2Int<unsigned int> (*argi);
                }
                else {
                    usage (L"Expected arg to -packetSize");
                    return EXIT_FAILURE;
                }
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"maxHops")) {
                ++argi;
                if (argi != args.end ()) {
                    maxHops = Characters::String2Int<unsigned int> (*argi);
                }
                else {
                    usage (L"Expected arg to -maxHops");
                    return EXIT_FAILURE;
                }
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"sampleCount")) {
                ++argi;
                if (argi != args.end ()) {
                    sampleCount = Characters::String2Int<unsigned int> (*argi);
                }
                else {
                    usage (L"Expected arg to -sampleCount");
                    return EXIT_FAILURE;
                }
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"help") or Execution::MatchesCommandLineArgument (*argi, L"h")) {
                usage ();
                return EXIT_SUCCESS;
            }
            else if (argi->StartsWith (L"-")) {
                usage (L"unrecognized option: " + *argi);
                return EXIT_FAILURE;
            }
            else {
                targetAddress = *argi;
            }
        }
    }

    try {
        Collection<InternetAddress> addrList = IO::Network::DNS::Default ().GetHostAddresses (targetAddress);
        if (addrList.empty ()) {
            Execution::Throw (Execution::StringException (L"whoops no addrs"));
        }
        InternetAddress addr = addrList.Nth (0);

        switch (majorOp) {
            case MajorOp::ePing: {
                Ping::Options options{};
                options.fPacketPayloadSize      = Ping::Options::kAllowedICMPPayloadSizeRange.Pin (packetSize - sizeof (ICMP::PacketHeader));
                options.fMaxHops                = maxHops;
                options.fSampleInfo             = Ping::Options::SampleInfo{kInterSampleTime_, sampleCount};
                NetworkMontior::Ping::Results t = NetworkMontior::Ping::Run (addr, options);
                cout << "Ping to " << addr.ToString ().AsNarrowSDKString () << ": " << Characters::ToString (t).AsNarrowSDKString () << endl;
            } break;
            case MajorOp::eTraceroute: {
                Sequence<Hop> hops = Traceroute (addr);
                cout << hops.size () << " hops" << endl;
                for (Hop h : hops) {
                    cerr << h.fAddress.As<String> ().AsNarrowSDKString () << " " << h.fTime.PrettyPrint ().AsNarrowSDKString () << endl;
                }
            } break;
        }
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
