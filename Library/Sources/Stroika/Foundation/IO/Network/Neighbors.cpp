/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <filesystem>

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
using namespace Stroika::Foundation::Streams;

using Neighbor = NeighborsMonitor::Neighbor;

// @todo Consider supporting ip neigh show and/or ip -6 neigh show
//- https://www.midnightfreddie.com/how-to-arp-a-in-ipv6.html
//-http ://man7.org/linux/man-pages/man8/ip-neighbour.8.html
//
// @todo perhaps add ability - background thread - to monitor, and always report up to date list?
//
namespace {
    Collection<Neighbor> ArpDashA_ (bool includePurgedEntries, bool omitAllFFHardwareAddresses)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}ArpDashA_", L"includePurgedEntries=%s",
                                                                                     Characters::ToString (includePurgedEntries).c_str ())};
#endif
#if qPlatform_Windows
        SystemInterfacesMgr sysInterfacesMgr;
#endif
        Collection<Neighbor> result;
        using std::byte;
#if qPlatform_POSIX
        ProcessRunner pr{"arp -an"sv}; // -a means 'BSD-style output' and -n means numeric (dont do reverse dns)
#elif qPlatform_Windows
        ProcessRunner pr{includePurgedEntries ? "arp -av"sv : "arp -a"sv}; // -a means 'BSD-style output', -v verbose(show invalid items)
#endif
        Streams::MemoryStream::Ptr<byte> useStdOut = Streams::MemoryStream::New<byte> ();
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
                if (s[1].StartsWith ("("sv) and s[1].EndsWith (")"sv)) {
                    if (not includePurgedEntries and s[3].Contains ("incomplete"sv)) {
                        continue;
                    }
                    String interfaceID;
                    size_t l = s.length ();
                    if (l >= 6 and s[l - 2] == "on"sv) {
                        interfaceID = s[l - 1];
                    }
                    result += Neighbor{InternetAddress{s[1].SubString (1, -1)}, s[3], interfaceID};
                }
            }
#elif qPlatform_Windows
            if (i.StartsWith ("Interface:"sv)) {
                Sequence<String> s = i.Tokenize ();
                if (s.length () >= 2) {
                    curInterface = s[1];
                    if (auto iface = sysInterfacesMgr.GetContainingAddress (InternetAddress{curInterface})) {
                        curInterface = iface->fInternalInterfaceID;
                    }
                    else {
                        WeakAssert (false); // bad -
                        curInterface.clear ();
                    }
                }
            }
            if (i.StartsWith (" "sv) and not curInterface.empty ()) {
                Sequence<String> s = i.Tokenize ();
                if (s.length () >= 3 and (s[2] == "static"sv or s[2] == "dynamic"sv)) {
                    // Windows arp produces address of the form xy-ab-cd, while most other platforms produce xy:ab:cd, so standardize the
                    // format we produce; helpful for WTF for example, sharing data among servers from different platforms --LGP 2022-06-14
                    static const String kDash_  = "-"sv;
                    static const String kColon_ = ":"sv;
                    if (omitAllFFHardwareAddresses) {
                        static const String kFFFF_ = "ff-ff-ff-ff-ff-ff"sv;
                        if (s[1] == kFFFF_) {
                            //DbgTrace (L"ignoring arped fake(broadcast) address %s", s[1].c_str ());
                            continue;
                        }
                    }
                    result += Neighbor{InternetAddress{s[0]}, s[1].ReplaceAll (kDash_, kColon_), curInterface};
                }
            }
#endif
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returning: %s", Characters::ToString (result).c_str ());
#endif
        return result;
    }
}

#if qPlatform_Linux
namespace {
    Collection<Neighbor> ProcNetArp_ (bool includePurgedEntries)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}ProcNetArp_", L"includePurgedEntries=%s",
                                                                                     Characters::ToString (includePurgedEntries).c_str ())};
#endif
        Collection<Neighbor> result;
        using Characters::String2Int;
        DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
        static const filesystem::path                          kProcFileName_{"/proc/net/arp"sv};
        /*
            IP address       HW type     Flags       HW address            Mask     Device
            192.168.244.194  0x1         0x0         00:00:00:00:00:00     *        enp0s31f6
            192.168.244.160  0x1         0x0         00:00:00:00:00:00     *        enp0s31f6
            192.168.244.5    0x1         0x2         04:a1:51:cd:fc:4c     *        enp0s31f6
            192.168.244.235  0x1         0x2         64:1c:ae:2a:95:7d     *        enp0s31f6
        */
        bool readFirstLine = false;
        // Note - /procfs files always unseekable
        for (const Sequence<String>& line : reader.ReadMatrix (IO::FileSystem::FileInputStream::New (kProcFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"in ProcNetArp_ capture_ line=%s", Characters::ToString (line).c_str ());
#endif
            if (not readFirstLine) {
                readFirstLine = true;
                continue;
            }
            if (line[2] == "0x0"sv) {
                continue; // I think this means disabled item
            }
            result += Neighbor{InternetAddress{line[0]}, line[3], line[5]};
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returning: %s", Characters::ToString (result).c_str ());
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
    sb << "{"sv;
    sb << "InternetAddress:"sv << Characters::ToString (fInternetAddress) << ","sv;
    sb << "HardwareAddress:"sv << Characters::ToString (fHardwareAddress) << ","sv;
    sb << "InterfaceID:"sv << Characters::ToString (fInterfaceID);
    sb << "}"sv;
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
        Require (not o.fMonitor.has_value ()); // cuz NYI
    }
    Collection<NeighborsMonitor::Neighbor> GetNeighbors () const
    {
        std::exception_ptr e           = nullptr; // try best approach, and then try other fallbacks, but rethrow exception from best
        auto               tryStrategy = [&] (Options::Strategy s) -> optional<Collection<NeighborsMonitor::Neighbor>> {
            // if strategies not specified, try any we are handed
            if (not fOptions_.fStategies or fOptions_.fStategies->Contains (s)) {
                try {
                    return GetNeighbors_ (s);
                }
                catch (...) {
                    if (e == nullptr) {
                        e = current_exception ();
                    }
                }
            }
            return nullopt;
        };
        // try strategies in best to worst order (filtered by caller restriction on which we can try)
#if qPlatform_Linux
        if (auto o = tryStrategy (Options::Strategy::eProcNetArp)) {
            return *o;
        }
#endif
        if (auto o = tryStrategy (Options::Strategy::eArpProgram)) {
            return *o;
        }
        if (e != nullptr) {
            rethrow_exception (e);
        }
        Execution::Throw (Execution::Exception{"No matching / available neighbors strategy"sv});
    }
    Collection<NeighborsMonitor::Neighbor> GetNeighbors_ (Options::Strategy s) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"NeighborsMonitor::Rep_::GetNeighbors_", L"s=%s",
                                                                                     Characters::ToString ((int)s).c_str ())};
#endif
        switch (s) {
            case Options::Strategy::eArpProgram:
                return ArpDashA_ (fOptions_.fIncludePurgedEntries.value_or (false), fOptions_.fOmitAllFFHardwareAddresses.value_or (true));
#if qPlatform_Linux
            case Options::Strategy::eProcNetArp:
                // fOmitAllFFHardwareAddresses not needed here apparently
                return ProcNetArp_ (fOptions_.fIncludePurgedEntries.value_or (false));
#endif
            default:
                Assert (false);
                return Collection<NeighborsMonitor::Neighbor>{};
        }
    }
    Options fOptions_;
};

NeighborsMonitor::NeighborsMonitor (const Options& options)
    : fRep_{make_shared<Rep_> (options)}
{
}

Collection<NeighborsMonitor::Neighbor> NeighborsMonitor::GetNeighbors () const
{
    return fRep_->GetNeighbors ();
}
