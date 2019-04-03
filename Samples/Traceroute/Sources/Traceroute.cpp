/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/IO/Network/DNS.h"

#include "Stroika/Frameworks/NetworkMonitor/Ping.h"
#include "Stroika/Frameworks/NetworkMonitor/Traceroute.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;
using namespace Stroika::Foundation::Time;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMonitor;

using Characters::String;
using Containers::Collection;
using Containers::Sequence;

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
    String                    targetAddress;
    enum class MajorOp {
        ePing,
        eTraceroute,
    };
    MajorOp               majorOp     = MajorOp::eTraceroute;
    unsigned int          maxHops     = Ping::Options::kDefaultMaxHops;
    unsigned int          sampleCount = 3;
    static const Duration kInterSampleTime_{"PT.1S"};
    size_t                packetSize = Ping::Options::kDefaultPayloadSize + sizeof (ICMP::V4::PacketHeader); // historically, the app ping has measured this including ICMP packet header, but not ip packet header size
    auto                  usage      = [] (const optional<String>& extraArg = {}) {
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
            Execution::Throw (Execution::Exception (L"whoops no addrs"sv));
        }
        InternetAddress addr = addrList.Nth (0);

        switch (majorOp) {
            case MajorOp::ePing: {
                Ping::Options options{};
                options.fPacketPayloadSize = Ping::Options::kAllowedICMPPayloadSizeRange.Pin (packetSize - sizeof (ICMP::V4::PacketHeader));
                options.fMaxHops           = maxHops;
                //   options.fSampleInfo                   = Ping::Options::SampleInfo{kInterSampleTime_, sampleCount};
                NetworkMonitor::Ping::SampleResults t = NetworkMonitor::Ping::Sample (addr, Ping::SampleOptions{kInterSampleTime_, sampleCount}, options);
                cout << "Ping to " << addr.ToString ().AsNarrowSDKString () << ": " << Characters::ToString (t).AsNarrowSDKString () << endl;
            } break;
            case MajorOp::eTraceroute: {
                Traceroute::Options options{};
                options.fPacketPayloadSize = Traceroute::Options::kAllowedICMPPayloadSizeRange.Pin (packetSize - sizeof (ICMP::V4::PacketHeader));
                options.fMaxHops           = maxHops;

                cout << "Tracing Route to " << targetAddress.AsNarrowSDKString () << " [" << Characters::ToString (addr).AsNarrowSDKString () << "] over a maximum of " << maxHops << " hops." << endl;
                cout << endl;

                // quickie - weak attempt at formatting the output
                cout << "Hop\tTime\t\tAddress" << endl;

                Sequence<Traceroute::Hop> hops = Traceroute::Run (addr, options);
                unsigned int              hopIdx{1};
                for (Traceroute::Hop h : hops) {
                    String hopName = [=] () {
                        String addrStr = h.fAddress.As<String> ();
                        if (auto rdnsName = DNS::Default ().QuietReverseLookup (h.fAddress)) {
                            return *rdnsName + L" [" + addrStr + L"]";
                        }
                        else {
                            return addrStr;
                        }
                    }();
                    cout << hopIdx++ << "\t" << h.fTime.PrettyPrint ().AsNarrowSDKString () << "\t" << hopName.AsNarrowSDKString () << endl;
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
