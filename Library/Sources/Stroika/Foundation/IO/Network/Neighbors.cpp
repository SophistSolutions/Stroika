/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/ProcessRunner.h"
#include "../../Streams/MemoryStream.h"
#include "../../Streams/TextReader.h"

#include "Neighbors.h"

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
        String                   out;
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
                    if (not includePurgedEntries and s[3].Contains ("incomplete")) {
                        continue;
                    }
                    result += Neighbor{InternetAddress{s[1].SubString (1, -1)}, s[3]};
                }
            }
#elif qPlatform_Windows
            if (i.StartsWith (L" ")) {
                Sequence<String> s = i.Tokenize ();
                if (s.length () >= 3 and (s[2] == L"static" or s[2] == L"dynamic")) {
                    result += Neighbor{InternetAddress{s[0]}, s[1]};
                }
            }
#endif
        }
        return result;
    }
}

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
        : fOptions_{o}
    {
    }
    Collection<NeighborsMonitor::Neighbor> GetNeighbors () const
    {
        return ArpDashA_ (fOptions_.fIncludePurgedEntries.value_or (false));
    }
    Options fOptions_;
};

NeighborsMonitor::NeighborsMonitor (const Options& options)
    : fRep_ (make_shared<Rep_> (options))
{
}

Collection<NeighborsMonitor::Neighbor> NeighborsMonitor::GetNeighbors () const
{
    return fRep_->GetNeighbors ();
}
