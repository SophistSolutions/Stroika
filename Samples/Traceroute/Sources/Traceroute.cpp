/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/IO/Network/DNS.h"

#include "Stroika/Frameworks/NetworkMonitor/Ping.h"
#include "Stroika/Frameworks/NetworkMonitor/Traceroute.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;
using namespace Stroika::Foundation::Time;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMonitor;

using Containers::Collection;
using Containers::Sequence;

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, Characters::ToString (vector<const char*>{argv, argv + argc}))};
    String targetAddress;
    enum class MajorOp {
        ePing,
        eTraceroute,
    };
    MajorOp               majorOp     = MajorOp::eTraceroute;
    unsigned int          maxHops     = Ping::Options::kDefaultMaxHops;
    unsigned int          sampleCount = 3;
    static const Duration kInterSampleTime_{"PT.1S"}; // aka 1.5s
    size_t packetSize = Ping::Options::kDefaultPayloadSize + sizeof (ICMP::V4::PacketHeader); // historically, the app ping has measured this including ICMP packet header, but not ip packet header size
    const Execution::CommandLine::Option kPingO_{.fLongName = "ping"sv, .fHelpOptionText = "Ping the target address"sv};
    const Execution::CommandLine::Option kTraceRtO_{.fLongName = "traceroute"sv, .fHelpOptionText = "Trace the route to the target address"sv};
    const Execution::CommandLine::Option kPacketSizeO_{
        .fLongName = "packetSize"sv, .fSupportsArgument = true, .fHelpArgName = "N", .fHelpOptionText = "Number of bytes to send in each test packet"sv};
    const Execution::CommandLine::Option kMaxHopsO_{
        .fLongName = "maxHops"sv, .fSupportsArgument = true, .fHelpArgName = "N", .fHelpOptionText = "Max number of hops to get to the target address"sv};
    const Execution::CommandLine::Option kSampleCntO_{
        .fLongName         = "sampleCount"sv,
        .fSupportsArgument = true,
        .fHelpArgName      = "N"sv,
        .fHelpOptionText = "(default 3) - if sampleCount is 1, exception details shown, otherwise just summary of number of exceptions"sv};
    const Execution::CommandLine::Option kTargetAddress_{.fSupportsArgument = true, .fRequired = true, .fHelpArgName = "target-address"sv};

    using namespace Execution::StandardCommandLineOptions;

    const initializer_list<Execution::CommandLine::Option> kAllOptions_{kHelp,      kPingO_,      kTraceRtO_,     kPacketSizeO_,
                                                                        kMaxHopsO_, kSampleCntO_, kTargetAddress_};

    auto usage = [&] (const optional<String>& extraArg = {}) {
        if (extraArg) {
            cerr << extraArg->AsNarrowSDKString () << endl;
        }
        cerr << Execution::CommandLine::GenerateUsage ("traceroute", kAllOptions_).AsNarrowSDKString ();
    };

    Execution::CommandLine cmdLine{argc, argv};
    using namespace Execution::StandardCommandLineOptions;

    if (cmdLine.Has (kPingO_)) {
        majorOp = MajorOp::ePing;
    }
    if (cmdLine.Has (kTraceRtO_)) {
        majorOp = MajorOp::eTraceroute;
    }
    if (auto o = cmdLine.GetArgument (kPacketSizeO_)) {
        packetSize = Characters::String2Int<unsigned int> (*o);
    }
    if (auto o = cmdLine.GetArgument (kMaxHopsO_)) {
        maxHops = Characters::String2Int<unsigned int> (*o);
    }
    if (auto o = cmdLine.GetArgument (kSampleCntO_)) {
        sampleCount = Characters::String2Int<unsigned int> (*o);
    }
    if (cmdLine.Has (kHelp)) {
        usage ();
        return EXIT_SUCCESS;
    }
    targetAddress = cmdLine.GetArgument (kTargetAddress_).value_or (String{});

    try {
        cmdLine.Validate (kAllOptions_);
        Collection<InternetAddress> addrList = IO::Network::DNS::kThe.GetHostAddresses (targetAddress);
        if (addrList.empty ()) {
            Execution::Throw (Execution::Exception{"whoops no addrs"sv});
        }
        InternetAddress addr = addrList.Nth (0);

        switch (majorOp) {
            case MajorOp::ePing: {
                Ping::Options options{};
                options.fPacketPayloadSize = Ping::Options::kAllowedICMPPayloadSizeRange.Pin (packetSize - sizeof (ICMP::V4::PacketHeader));
                options.fMaxHops           = maxHops;
                //   options.fSampleInfo                   = Ping::Options::SampleInfo{kInterSampleTime_, sampleCount};
                NetworkMonitor::Ping::SampleResults t =
                    NetworkMonitor::Ping::Sample (addr, Ping::SampleOptions{kInterSampleTime_, sampleCount}, options);
                cout << "Ping to " << addr.ToString ().AsNarrowSDKString () << ": " << Characters::ToString (t).AsNarrowSDKString () << endl;
            } break;
            case MajorOp::eTraceroute: {
                Traceroute::Options options{};
                options.fPacketPayloadSize = Traceroute::Options::kAllowedICMPPayloadSizeRange.Pin (packetSize - sizeof (ICMP::V4::PacketHeader));
                options.fMaxHops = maxHops;

                cout << "Tracing Route to " << targetAddress.AsNarrowSDKString () << " ["
                     << Characters::ToString (addr).AsNarrowSDKString () << "] over a maximum of " << maxHops << " hops." << endl;
                cout << endl;

                // quickie - weak attempt at formatting the output
                cout << "Hop\tTime\t\tAddress" << endl;
                unsigned int hopIdx{1};
                auto         perHopCallback = [&] (Traceroute::Hop h) {
                    String hopName = [=] () -> String {
                        if (h.fAddress.empty ()) {
                            return "*"sv;
                        }
                        String addrStr = h.fAddress.As<String> ();
                        if (auto rdnsName = DNS::kThe.QuietReverseLookup (h.fAddress)) {
                            return *rdnsName + " ["_k + addrStr + "]"_k;
                        }
                        else {
                            return addrStr;
                        }
                    }();
                    String timeStr = h.fTime.empty () ? "timeout\t"_k : h.fTime.PrettyPrint ();
                    cout << hopIdx++ << "\t" << timeStr.AsNarrowSDKString () << "\t" << hopName.AsNarrowSDKString () << endl;
                };
                // can call without callback, and just get all the hops in a list, which is simpler. But using the callback allows you to see
                // per hop progress as its accumulated, which is more typical for traceroute UI
                Traceroute::Run (addr, perHopCallback, options);
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
