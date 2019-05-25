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

#if 0
namespace {
    /*
     ********************************************************************************
     *************************** MyNeighborDiscoverer_ ******************************
     ********************************************************************************
     */

    struct MyNeighborDiscoverer_ {
        MyNeighborDiscoverer_ ()
            : fMyThread_ (
                  Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New (DiscoveryChecker_, Thread::eAutoStart, L"MyNeighborDiscoverer"))
        {
        }

    private:
        static void DiscoveryChecker_ ()
        {
            static constexpr Activity kDiscovering_NetNeighbors_{L"discovering this network neighbors"sv};
            while (true) {
                try {
                    DeclareActivity da{&kDiscovering_NetNeighbors_};
                    for (ArpRec_ i : ArpDashA_ ()) {
                        // soon store/pay attention to macaddr as better indicator of unique device id than ip addr

                        // ignore multicast addresses as they are not real devices(???always???)
                        if (i.ia.IsMulticastAddress ()) {
                            //DbgTrace (L"ignoring arped multicast address %s", Characters::ToString (i.ia).c_str ());
                            continue;
                        }
#if qPlatform_Windows
                        if (i.fHardwareAddress == L"ff-ff-ff-ff-ff-ff") {
                            //DbgTrace (L"ignoring arped fake(broadcast) address %s", Characters::ToString (i.ia).c_str ());
                            continue;
                        }
#endif

                        // merge in data
                        auto           l  = sDiscoveredDevices_.rwget ();
                        DiscoveryInfo_ di = [&] () {
                            DiscoveryInfo_ tmp{};
                            tmp.ipAddresses += i.ia;
                            if (optional<DiscoveryInfo_> o = FindMatchingDevice_ (l, tmp)) {
                                return *o;
                            }
                            else {
                                // Generate GUID - based on ipaddrs
                                tmp.fGUID = LookupPersistentDeviceID_ (tmp);
                                return tmp;
                            }
                        }();

                        di.PatchDerivedFields ();
                        l->Add (di.fGUID, di);
                    }
                }
                catch (const Thread::InterruptException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
                    Execution::Logger::Get ().LogIfNew (Execution::Logger::Priority::eError, 5min, L"%s", Characters::ToString (current_exception ()).c_str ());
                }
                Execution::Sleep (1min); // unsure of right interval - maybe able to epoll or something so no actual polling needed
            }
        }
        Thread::CleanupPtr fMyThread_;
    };

    unique_ptr<MyNeighborDiscoverer_> sNeighborDiscoverer_;
}
#endif