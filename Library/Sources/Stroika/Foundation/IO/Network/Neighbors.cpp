/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Characters/String2Int.h"
#include "../../DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/ProcessRunner.h"
#include "../../IO/FileSystem/FileInputStream.h"
#include "../../Streams/MemoryStream.h"
#include "../../Streams/TextReader.h"

#include "Neighbors.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using Neighbor = NeighborsMonitor::Neighbor;

// @todo
//- https://www.midnightfreddie.com/how-to-arp-a-in-ipv6.html
//-http ://man7.org/linux/man-pages/man8/ip-neighbour.8.html
//  -arp - a
//  - ip neigh show
//  - ip -6 neigh show
//  - cat /proc/net/arp
//  - use that to fill in new devices / info for discovery
//
// @todo perhaps add ability - background thread - to monitor, and always report up to date list?
//
namespace {
    Collection<Neighbor> ArpDashA_ (bool includePurgedEntries)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}ArpDashA_", L"includePurgedEntries=%s", Characters::ToString (includePurgedEntries).c_str ())};
#endif
        Collection<Neighbor> result;
        using std::byte;
#if qPlatform_POSIX
        ProcessRunner pr (L"arp -an"); // -a means 'BSD-style output' and -n means numeric (dont do reverse dns)
#elif qPlatform_Windows
        ProcessRunner pr (includePurgedEntries ? L"arp -av" : L"arp -a"); // -a means 'BSD-style output', -v verbose(show invalid items)
#endif
        Streams::MemoryStream<byte>::Ptr useStdOut = Streams::MemoryStream<byte>::New ();
        pr.SetStdOut (useStdOut);
        pr.Run ();
        String out;
#if qPlatform_Windows
        String curInterface;
#endif
        Streams::TextReader::Ptr stdOut = Streams::TextReader::New (useStdOut);
        for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
#if qPlatform_POSIX
            Sequence<String> s = i.Tokenize ();
            if (s.length () >= 4) {
                // raspberrypi.34ChurchStreet.sophists.com (192.168.244.32) at b8:27:eb:cc:c7:80 [ether] on enp0s31f6
                // ? (192.168.244.173) at b8:3e:59:88:71:06 [ether] on enp0s31f6
                //
                // ? (192.168.244.70) at 50:dc:e7:7c:76:a0 [ether] on enp0s31f6
                // ? (192.168.244.152) at <incomplete> on enp0s31f6
                // ? (192.168.244.210) at <incomplete> on enp0s31f6
                // ? (192.168.244.137) at <incomplete> on enp0s31f6
                // ? (192.168.244.126) at 00:1a:62:04:0a:b1 [ether] on enp0s31f6
                //
                // According to https://unix.stackexchange.com/questions/192313/how-do-you-clear-the-arp-cache-on-linux
                // 'incomplete' entries mean about to be removed from the ARP table (or mostly removed).
                //
                if (s[1].StartsWith (L"(") and s[1].EndsWith (L")")) {
                    if (not includePurgedEntries and s[3].Contains (L"incomplete")) {
                        continue;
                    }
                    String interfaceID;
                    size_t l = s.length ();
                    if (l >= 6 and s[l - 2] == L"on") {
                        interfaceID = s[l - 1];
                    }
                    result += Neighbor{InternetAddress{s[1].SubString (1, -1)}, s[3], interfaceID};
                }
            }
#elif qPlatform_Windows
            if (i.StartsWith (L"Interface:")) {
                Sequence<String> s = i.Tokenize ();
                if (s.length () >= 2) {
                    curInterface = s[1];
                }
            }
            if (i.StartsWith (L" ")) {
                Sequence<String> s = i.Tokenize ();
                if (s.length () >= 3 and (s[2] == L"static" or s[2] == L"dynamic")) {
                    result += Neighbor{InternetAddress{s[0]}, s[1], curInterface};
                }
            }
#endif
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("returning: %s", Characters::ToString (result).c_str ());
#endif
        return result;
    }
}

#if qPlatform_Linux
namespace {
    Collection<Neighbor> ProcNetArp_ (bool includePurgedEntries)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}ProcNetArp_", L"includePurgedEntries=%s", Characters::ToString (includePurgedEntries).c_str ())};
#endif
        Collection<Neighbor> result;
        using Characters::String2Int;
        using IO::FileSystem::FileInputStream;
        using Streams::TextReader;
        DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
        const String                                           kProcFileName_{L"/proc/net/arp"sv};
        /*
            IP address       HW type     Flags       HW address            Mask     Device
            192.168.244.194  0x1         0x0         00:00:00:00:00:00     *        enp0s31f6
            192.168.244.160  0x1         0x0         00:00:00:00:00:00     *        enp0s31f6
            192.168.244.5    0x1         0x2         04:a1:51:cd:fc:4c     *        enp0s31f6
            192.168.244.235  0x1         0x2         64:1c:ae:2a:95:7d     *        enp0s31f6
        */
        bool readFirstLine = false;
        // Note - /procfs files always unseekable
        for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"in ProcNetArp_ capture_ line=%s", Characters::ToString (line).c_str ());
#endif
            if (not readFirstLine) {
                readFirstLine = true;
                continue;
            }
            if (String2Int (line[2]) == 0) {
                continue; // I think this means disabled item
            }
            result += Neighbor{InternetAddress{line[0]}, line[3], line[5]};
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("returning: %s", Characters::ToString (result).c_str ());
#endif
        return result;
    }
}
#endif

/*
 ********************************************************************************
 ********************** NeighborsMonitor::Neighbor ******************************
 ********************************************************************************
 */
String NeighborsMonitor::Neighbor::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"fInternetAddress:" + Characters::ToString (fInternetAddress) + L",";
    sb += L"fHardwareAddress:" + Characters::ToString (fHardwareAddress) + L",";
    sb += L"fInterfaceID:" + Characters::ToString (fInterfaceID) + L",";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************************** NeighborsMonitor ******************************
 ********************************************************************************
 */
class NeighborsMonitor::Rep_ {
public:
    Rep_ (const Options& o)
        : fOptions_
    {
        o
    }
#if qPlatform_Linux
    , fUseStrategy_
    {
        Options::Strategy::eProcNetArp
    }
#else
    , fUseStrategy_
    {
        Options::Strategy::eArpProgram
    }
#endif
    {
        if (o.fStategies.has_value () and o.fStategies->Contains (Options::Strategy::eArpProgram)) {
            fUseStrategy_ = Options::Strategy::eArpProgram;
        }
#if qPlatform_Linux
        if (o.fStategies.has_value () and o.fStategies->Contains (Options::Strategy::eProcNetArp)) {
            fUseStrategy_ = Options::Strategy::eProcNetArp;
        }
#endif
    }
    Collection<NeighborsMonitor::Neighbor> GetNeighbors () const
    {
        switch (fUseStrategy_) {
            case Options::Strategy::eArpProgram:
                return ArpDashA_ (fOptions_.fIncludePurgedEntries.value_or (false));
#if qPlatform_Linux
            case Options::Strategy::eProcNetArp:
                return ProcNetArp_ (fOptions_.fIncludePurgedEntries.value_or (false));
#endif
            default:
                Assert (false);
                return Collection<NeighborsMonitor::Neighbor>{};
        }
    }
    Options           fOptions_;
    Options::Strategy fUseStrategy_;
};

NeighborsMonitor::NeighborsMonitor (const Options& options)
    : fRep_ (make_shared<Rep_> (options))
{
}

Collection<NeighborsMonitor::Neighbor> NeighborsMonitor::GetNeighbors () const
{
    return fRep_->GetNeighbors ();
}
