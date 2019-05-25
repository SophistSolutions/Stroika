/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
//  - ip - 6 neigh show
//  - cat / proc / net / arp
//  - use that to fill in new devices / info for discovery
//
// @todo perhaps add ability - background thread - to monitor, and always report up to date list?
//
namespace {
    Collection<Neighbor> ArpDashA_ ()
    {
        Collection<Neighbor> result;
        using std::byte;
        ProcessRunner                    pr (L"arp -a");
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
                if (s[1].StartsWith (L"(") and s[1].EndsWith (L")")) {
                    result += ArpRec_{InternetAddress{s[1].SubString (1, -1)}, s[3]};
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
        return ArpDashA_ ();
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
