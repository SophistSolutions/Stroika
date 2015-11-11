/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_AIX
#include    <libperfstat.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <Iphlpapi.h>
#endif

#include    "../../../Foundation/Characters/FloatConversion.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include    "../../../Foundation/Streams/InputStream.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/TextReader.h"

#include    "Network.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::Network;


using   Characters::Character;
using   Characters::String_Constant;
using   Containers::Mapping;
using   Containers::Sequence;
using   Containers::Set;
using   IO::FileSystem::FileInputStream;
using   Time::DurationSecondsType;




// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif




#ifndef qSupportProcNet_
#define qSupportProcNet_       qPlatform_Linux
#endif




#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif





#if     qUseWMICollectionSupport_
namespace {
    const   String_Constant     kBytesReceivedPerSecond_            { L"Bytes Received/sec" };
    const   String_Constant     kBytesSentPerSecond_                { L"Bytes Sent/sec" };
    const   String_Constant     kPacketsReceivedPerSecond_          { L"Packets Received/sec" };
    const   String_Constant     kPacketsSentPerSecond_              { L"Packets Sent/sec" };
    const   String_Constant     kTCPSegmentsPerSecond_              { L"Segments/sec" };
    const   String_Constant     kSegmentsRetransmittedPerSecond_    { L"Segments Retransmitted/sec" };
}
#endif



#if     qPlatform_Windows
#pragma comment (lib, "iphlpapi.lib")
#endif





/*
 ********************************************************************************
 *********************** Instruments::Network::IOStatistics *********************
 ********************************************************************************
 */
IOStatistics&   IOStatistics::operator+= (const IOStatistics& rhs)
{
    fTotalBytesSent.AccumulateIf (rhs.fTotalBytesSent);
    fTotalBytesReceived.AccumulateIf (rhs.fTotalBytesReceived);
    fBytesPerSecondSent.AccumulateIf (rhs.fBytesPerSecondSent);
    fBytesPerSecondReceived.AccumulateIf (rhs.fBytesPerSecondReceived);
    fTotalTCPSegments.AccumulateIf (rhs.fTotalTCPSegments);
    fTCPSegmentsPerSecond.AccumulateIf (rhs.fTCPSegmentsPerSecond);
    fTotalTCPRetransmittedSegments.AccumulateIf (rhs.fTotalTCPRetransmittedSegments);
    fTCPRetransmittedSegmentsPerSecond.AccumulateIf (rhs.fTCPRetransmittedSegmentsPerSecond);
    fTotalPacketsSent.AccumulateIf (rhs.fTotalPacketsSent);
    fTotalPacketsReceived.AccumulateIf (rhs.fTotalPacketsReceived);
    fPacketsPerSecondSent.AccumulateIf (rhs.fPacketsPerSecondSent);
    fPacketsPerSecondReceived.AccumulateIf (rhs.fPacketsPerSecondReceived);
    fTotalErrors.AccumulateIf (rhs.fTotalErrors);
    fTotalPacketsDropped.AccumulateIf (rhs.fTotalPacketsDropped);
    return *this;
}









namespace {
    struct  CapturerWithContext_COMMON_ {
        Options                     fOptions_;
        DurationSecondsType         fMinimumAveragingInterval_  {};
        DurationSecondsType         fPostponeCaptureUntil_      { 0 };
        DurationSecondsType         fLastCapturedAt             {};
        CapturerWithContext_COMMON_ () = delete;
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DurationSecondsType    GetLastCaptureAt () const { return fLastCapturedAt; }
        void    NoteCompletedCapture_ ()
        {
            auto now = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt = now;
        }
    };
}







#if     qPlatform_AIX
namespace {
    struct  CapturerWithContext_AIX_ : CapturerWithContext_COMMON_ {
        struct  PerfStats_ {
            uint64_t  fTotalBytesReceived;
            uint64_t  fTotalBytesSent;
            uint64_t  fTotalPacketsReceived;
            uint64_t  fTotalPacketsSent;
        };
        Mapping<String, PerfStats_>       fLast;
        struct OverallTCPStats_ {
            uint64_t    fTotalSegments;
            uint64_t    fTotalTCPRetransmits;
        };
        Optional<OverallTCPStats_>      fLastOverallTCPStats_;
        CapturerWithContext_AIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // hack for side-effect of  updating fLastSum etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_AIX_ (const CapturerWithContext_AIX_&) = default;   // copy by value fine - no need to re-wait...
        Instruments::Network::Info    capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Network::Info    capture_ ()
        {
            using   Instruments::Network::InterfaceInfo;
            using   Instruments::Network::Info;

            Mapping<String, PerfFetchResults_>   thisRunStats    =   perfstat_fetch_ ();

            Collection<InterfaceInfo>   interfaceResults;

            Time::DurationSecondsType   elapsed = Time::GetTickCount () - GetLastCaptureAt ();

            IOStatistics                accumSummary;
            for (KeyValuePair<String, PerfFetchResults_> i : thisRunStats) {
                try {
                    Memory::Optional<IO::Network::Interface>    oIFace = IO::Network::GetInterfaceById (i.fKey);
                    if (oIFace) {
                        InterfaceInfo   iiInfo;
                        iiInfo.fInterface = *oIFace;

                        IOStatistics    stats;
                        stats.fTotalBytesReceived = i.fValue.fTotalBytesReceived;
                        stats.fTotalBytesSent = i.fValue.fTotalBytesSent;
                        stats.fTotalPacketsReceived = i.fValue.fTotalPacketsReceived;
                        stats.fTotalPacketsSent = i.fValue.fTotalPacketsSent;

                        if (Optional<PerfStats_> prev = fLast.Lookup (i.fKey)) {
                            stats.fBytesPerSecondReceived = static_cast<double> (i.fValue.fTotalBytesReceived - prev->fTotalBytesReceived) / elapsed;
                            stats.fBytesPerSecondSent = static_cast<double> (i.fValue.fTotalBytesSent - prev->fTotalBytesSent) / elapsed;
                            stats.fPacketsPerSecondReceived = static_cast<double> (i.fValue.fTotalPacketsReceived - prev->fTotalPacketsReceived) / elapsed;
                            stats.fPacketsPerSecondSent = static_cast<double> (i.fValue.fTotalPacketsSent - prev->fTotalPacketsSent) / elapsed;
                        }

                        // fTransmitSpeedBaud and fReceiveLinkSpeedBaud already gathered from interface, but the values appear unlikely, so try
                        // these -LGP 2015-09-28
                        iiInfo.fInterface.fTransmitSpeedBaud = i.fValue.fBAUDRate;
                        iiInfo.fInterface.fReceiveLinkSpeedBaud = i.fValue.fBAUDRate;
                        stats.fTotalErrors = i.fValue.fTotalErrors;
                        iiInfo.fIOStatistics = stats;
                        interfaceResults.Add (iiInfo);
                        accumSummary += stats;
                    }
                    else {
                        DbgTrace (L"Ignore missing network interface %s", i.fKey.c_str ());
                    }
                }
                catch (...) {
                    DbgTrace (L"Ignore exception looking up network interface %s", i.fKey.c_str ());
                }
            }

            fLast.clear ();
            thisRunStats.Apply ([this] (const KeyValuePair<String, PerfFetchResults_>& i) { fLast.Add (i.fKey, i.fValue); });

            OverallTCPStats_    tcpStats    =   GetOverallTCPStats_ ();
            accumSummary.fTotalTCPSegments = tcpStats.fTotalSegments;
            accumSummary.fTotalTCPRetransmittedSegments = tcpStats.fTotalTCPRetransmits;
            if (fLastOverallTCPStats_) {
                accumSummary.fTCPSegmentsPerSecond = (tcpStats.fTotalSegments - fLastOverallTCPStats_->fTotalSegments) / elapsed;
                accumSummary.fTCPRetransmittedSegmentsPerSecond = (tcpStats.fTotalTCPRetransmits - fLastOverallTCPStats_->fTotalTCPRetransmits) / elapsed;
            }
            fLastOverallTCPStats_  = tcpStats;

            NoteCompletedCapture_ ();
            return Info { interfaceResults, accumSummary };
        }
        struct PerfFetchResults_ :  PerfStats_ {
            uint64_t    fBAUDRate;
            uint64_t    fTotalErrors;
            PerfFetchResults_ (uint64_t tbr, uint64_t tbs, uint64_t tpr, uint64_t tps, uint64_t baud, uint64_t totalErrors)
                : PerfStats_ { tbr, tbs, tpr, tps }
            , fBAUDRate { baud }
            , fTotalErrors { totalErrors } {
            }
        };
        Mapping<String, PerfFetchResults_>   perfstat_fetch_ ()
        {
            Mapping<String, PerfFetchResults_>   result;
            int interfaceCount =  perfstat_netinterface (NULL, NULL,  sizeof (perfstat_netinterface_t), 0);
            if (interfaceCount > 0) {
                Memory::SmallStackBuffer<perfstat_netinterface_t>   statBuf (interfaceCount);
                perfstat_id_t firstinterface = { 0 };
                int ret = perfstat_netinterface (&firstinterface, statBuf.begin (), sizeof(perfstat_netinterface_t), interfaceCount);
                Assert (ret <= interfaceCount);
                for (int i = 0; i < ret; ++i) {
                    result.Add (
                        String::FromNarrowSDKString (statBuf[i].name),
                    PerfFetchResults_ {
                        statBuf[i].ibytes,
                        statBuf[i].obytes,
                        statBuf[i].ipackets,
                        statBuf[i].opackets,
                        statBuf[i].bitrate,
                        statBuf[i].ierrors + statBuf[i].oerrors,
                    }
                    );
                }
            }
            return result;
        }
        OverallTCPStats_    GetOverallTCPStats_ ()
        {
            OverallTCPStats_ result {};
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Network::....GetOverallTCPStats_");
#endif
            using   Execution::ProcessRunner;
            {
                ProcessRunner   pr (String_Constant { L"/usr/bin/netstat -s" });
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                bool    sawTCP = false;
                Optional<uint64_t>  dataPackets;
                Optional<uint64_t>  retramsitPackets;
                for (String line : Streams::TextReader (useStdOut).ReadLines ()) {
                    line = line.Trim ();
                    if (not sawTCP) {
                        sawTCP = (line == L"tcp:");
                        continue;
                    }
                    unsigned long long  val     {};
                    wchar_t*            endptr  {};
                    val = ::wcstoull (line.c_str (), &endptr, 10);
                    AssertNotNull (endptr);
                    String  restOfLine = endptr;
                    if (restOfLine.Contains (L"data packets")) {
                        if (restOfLine.Contains (L"retransmitted")) {
                            retramsitPackets = val;
                        }
                        else {
                            dataPackets = val;
                        }
                    }
                }
                result.fTotalSegments = dataPackets.Value ();
                result.fTotalTCPRetransmits = retramsitPackets.Value ();
            }
            return result;
        }
    };
}
#endif






#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ : CapturerWithContext_COMMON_ {

        struct Last {
            uint64_t  fTotalBytesReceived;
            uint64_t  fTotalBytesSent;
            uint64_t  fTotalPacketsReceived;
            uint64_t  fTotalPacketsSent;
            DurationSecondsType  fAt;
        };
        Mapping<String, Last>       fLast;
        struct LastSum {
            uint64_t  fTotalTCPSegments;
            uint64_t  fTotalTCPRetransmittedSegments;
            DurationSecondsType  fAt;
        };
        Optional<LastSum>           fLastSum;
        CapturerWithContext_POSIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // hack for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_&) = default;   // copy by value fine - no need to re-wait...
        Instruments::Network::Info    capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Network::Info    capture_ ()
        {
            using   Instruments::Network::InterfaceInfo;
            using   Instruments::Network::Info;

            Collection<InterfaceInfo>   interfaceResults;
            IOStatistics                accumSummary;
#if     qSupportProcNet_
            Read_proc_net_dev_ (&interfaceResults, &accumSummary);
            Read_proc_net_snmp_ (&accumSummary);
#endif

            DurationSecondsType now = Time::GetTickCount ();
            if (fLastSum and accumSummary.fTotalTCPSegments) {
                Time::DurationSecondsType timespan  { now - fLastSum->fAt };
                accumSummary.fTCPSegmentsPerSecond = (accumSummary.fTotalTCPSegments.Value () - fLastSum->fTotalTCPSegments) / timespan;
            }
            if (fLastSum and accumSummary.fTotalTCPRetransmittedSegments) {
                Time::DurationSecondsType timespan  { now - fLastSum->fAt };
                accumSummary.fTCPRetransmittedSegmentsPerSecond = (accumSummary.fTotalTCPRetransmittedSegments.Value () - fLastSum->fTotalTCPRetransmittedSegments) / timespan;
            }
            if (accumSummary.fTotalTCPSegments and accumSummary.fTotalTCPRetransmittedSegments) {
                fLastSum = LastSum { *accumSummary.fTotalTCPSegments, *accumSummary.fTotalTCPRetransmittedSegments, now };
            }
            NoteCompletedCapture_ ();
            return Info { interfaceResults, accumSummary };
        }
#if     qSupportProcNet_
        void    Read_proc_net_dev_ (Collection<Instruments::Network::InterfaceInfo>* interfaceResults, IOStatistics* accumSummary)
        {
            using   Instruments::Network::InterfaceInfo;
            RequireNotNull (interfaceResults);
            RequireNotNull (accumSummary);
            DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
            static  const   String_Constant kProcFileName_ { L"/proc/net/dev" };
            //static    const String_Constant kProcFileName_ { L"c:\\Sandbox\\VMSharedFolder\\proc-net-dev" };
            // Note - /procfs files always unseekable
            unsigned int    nLine   = 0;
            unsigned int    n2Skip  = 2;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::Network::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                nLine++;
                if (n2Skip > 0) {
                    --n2Skip;
                    continue;
                }
                if (line.size () >= 17) {
                    constexpr   int kOffset2XMit_ = 8;
                    InterfaceInfo   ii;
                    if (auto info = IO::Network::GetInterfaceById (line[0])) {
                        ii.fInterface = *info;
                    }
                    else {
                        ii.fInterface.fInternalInterfaceID = line[0];
                        ii.fInterface.fFriendlyName = line[0];
                    }
                    ii.fIOStatistics.fTotalBytesReceived = Characters::String2Int<uint64_t> (line[1]);
                    ii.fIOStatistics.fTotalBytesSent = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 1]);
                    ii.fIOStatistics.fTotalPacketsReceived = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 2]);
                    ii.fIOStatistics.fTotalPacketsSent = Characters::String2Int<uint64_t> (line[2]);
                    ii.fIOStatistics.fTotalErrors = Characters::String2Int<uint64_t> (line[3]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 3]);
                    ii.fIOStatistics.fTotalPacketsDropped = Characters::String2Int<uint64_t> (line[4]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 4]);

                    DurationSecondsType now = Time::GetTickCount ();
                    if (auto o = fLast.Lookup (ii.fInterface.fInternalInterfaceID)) {
                        double scanTime = now - o->fAt;
                        if (scanTime > 0) {
                            ii.fIOStatistics.fBytesPerSecondReceived = (*ii.fIOStatistics.fTotalBytesReceived - o->fTotalBytesReceived) / scanTime;
                            ii.fIOStatistics.fBytesPerSecondSent = (*ii.fIOStatistics.fTotalBytesSent - o->fTotalBytesSent) / scanTime;
                            ii.fIOStatistics.fPacketsPerSecondReceived = (*ii.fIOStatistics.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                            ii.fIOStatistics.fPacketsPerSecondSent = (*ii.fIOStatistics.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                        }
                    }
                    (*accumSummary) += ii.fIOStatistics;
                    interfaceResults->Add (ii);
                    fLast.Add (ii.fInterface.fInternalInterfaceID, Last { *ii.fIOStatistics.fTotalBytesReceived, *ii.fIOStatistics.fTotalBytesSent, *ii.fIOStatistics.fTotalPacketsReceived, *ii.fIOStatistics.fTotalPacketsSent, now });
                }
                else {
                    DbgTrace (L"Line %d bad in file %s", nLine, kProcFileName_.c_str ());
                }
            }
        }
        void    Read_proc_net_netstat_ (IOStatistics* accumSummary)
        {
            AssertNotReached ();    // dont use this for now
            RequireNotNull (accumSummary);
            DataExchange::CharacterDelimitedLines::Reader reader {{  ' ', '\t' }};
            static  const   String_Constant kProcFileName_ { L"/proc/net/netstat" };
            // Note - /procfs files always unseekable
            bool    firstTime = true;
            Mapping<String, size_t> labelMap;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::Network::Info Read_proc_net_netstat_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                if (line.size () >= 2 and line[0].Trim () == L"TcpExt:") {
                    if (firstTime) {
                        size_t  idx = 0;
                        for (String i : line) {
                            labelMap.Add (i.Trim (), idx++);
                        }
                        firstTime = false;
                    }
                    else {
                        // @todo must sum several fields - NYI
                        if (auto oTCPSynRetransIdx = labelMap.Lookup (L"TCPSynRetrans")) {
                            if (*oTCPSynRetransIdx < line.length ()) {
                                uint64_t TCPSynRetrans =  Characters::String2Int<uint64_t> (line[*oTCPSynRetransIdx]);
                                accumSummary->fTotalTCPRetransmittedSegments = TCPSynRetrans;
                            }
                        }
                    }
                }
            }
        }
        void    Read_proc_net_snmp_ (IOStatistics* accumSummary)
        {
            RequireNotNull (accumSummary);
            DataExchange::CharacterDelimitedLines::Reader reader {{  ' ', '\t' }};
            static  const   String_Constant kProcFileName_ { L"/proc/net/snmp" };
            // Note - /procfs files always unseekable
            bool    firstTime = true;
            Mapping<String, size_t> labelMap;
            Optional<uint64_t>  totalTCPSegments;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::Network::Info Read_proc_net_snmp_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                if (line.size () >= 2 and line[0].Trim () == L"Tcp:") {
                    if (firstTime) {
                        size_t  idx = 0;
                        for (String i : line) {
                            labelMap.Add (i.Trim (), idx++);
                        }
                        firstTime = false;
                    }
                    else {
                        static  const   String_Constant     kInSegs_        { L"InSegs" };
                        static  const   String_Constant     kOutSegs_       { L"OutSegs" };
                        static  const   String_Constant     kRetransSegs_   { L"RetransSegs" };
                        if (auto idx = labelMap.Lookup (kInSegs_)) {
                            if (*idx < line.length ()) {
                                totalTCPSegments.AccumulateIf (Characters::String2Int<uint64_t> (line[*idx]));
                            }
                        }
                        if (auto idx = labelMap.Lookup (kOutSegs_)) {
                            if (*idx < line.length ()) {
                                totalTCPSegments.AccumulateIf (Characters::String2Int<uint64_t> (line[*idx]));
                            }
                        }
                        if (auto idx = labelMap.Lookup (kRetransSegs_)) {
                            if (*idx < line.length ()) {
                                accumSummary->fTotalTCPRetransmittedSegments = Characters::String2Int<uint64_t> (line[*idx]);
                            }
                        }
                    }
                }
            }
            totalTCPSegments.CopyToIf (&accumSummary->fTotalTCPSegments);
        }
#endif
    };
}
#endif






#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if     qUseWMICollectionSupport_
        WMICollector        fNetworkWMICollector_ { String_Constant { L"Network Interface" }, {},  { kBytesReceivedPerSecond_, kBytesSentPerSecond_, kPacketsReceivedPerSecond_, kPacketsSentPerSecond_ } };
        WMICollector        fTCPv4WMICollector_ { String_Constant { L"TCPv4" }, {},  { kTCPSegmentsPerSecond_, kSegmentsRetransmittedPerSecond_ } };
        WMICollector        fTCPv6WMICollector_ { String_Constant { L"TCPv6" }, {},  { kTCPSegmentsPerSecond_, kSegmentsRetransmittedPerSecond_ } };
        Set<String>         fAvailableInstances_;
#endif
        CapturerWithContext_Windows_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
#if     qUseWMICollectionSupport_
            fAvailableInstances_ = fNetworkWMICollector_.GetAvailableInstaces ();
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            {
                Debug::TraceContextBumper ctx ("ALL WMI Network Avialable instances");
                for (String i : fAvailableInstances_) {
                    DbgTrace (L"wmiInstanceName='%s'", i.c_str ());
                }
            }
#endif
            capture_ ();    // for the side-effect of filling in fNetworkWMICollector_ with interfaces and doing initial capture so WMI can compute averages
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if     qUseWMICollectionSupport_
            , fNetworkWMICollector_ (from.fNetworkWMICollector_)
            , fTCPv4WMICollector_ (from.fTCPv4WMICollector_)
            , fTCPv6WMICollector_ (from.fTCPv6WMICollector_)
            , fAvailableInstances_ (from.fAvailableInstances_)
#endif
        {
#if   qUseWMICollectionSupport_
            capture_ ();    // for the side-effect of filling in fNetworkWMICollector_ with interfaces and doing initial capture so WMI can compute averages
#endif
        }
        Instruments::Network::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Network::Info capture_ ()
        {
            using   IO::Network::Interface;
            using   Instruments::Network::InterfaceInfo;
            using   Instruments::Network::Info;

            Info                        result;
            Collection<InterfaceInfo>   interfaceResults;
            IOStatistics                accumSummary;
            Iterable<Interface>         networkInterfacs {  IO::Network::GetInterfaces () };
#if     qUseWMICollectionSupport_
            fNetworkWMICollector_.Collect ();
            fTCPv4WMICollector_.Collect ();
            fTCPv6WMICollector_.Collect ();
#endif
            {
                for (IO::Network::Interface networkInterface : networkInterfacs) {
                    InterfaceInfo   ii;
#if 0
                    ii.fInternalInterfaceID = networkInterface.fInternalInterfaceID;
                    ii.fDisplayName = networkInterface.fFriendlyName;
                    ii.fInterfaceType = networkInterface.fType;
                    ii.fInterfaceStatus = networkInterface.fStatus;
#endif
                    ii.fInterface = networkInterface;
#if     qUseWMICollectionSupport_
                    Read_WMI_ (networkInterface, &ii);
#endif
                    accumSummary += ii.fIOStatistics;
                    interfaceResults.Add (ii);
                }
            }
            result.fInterfaces = interfaceResults;
            {
                MIB_IPSTATS stats {};       // maybe more useful stats we could pull out of this?
                memset (&stats, 0, sizeof (stats));
                Execution::Platform::Windows::ThrowIfNot_NO_ERROR (::GetIpStatistics (&stats));
                accumSummary.fTotalPacketsReceived = stats.dwInReceives;
                accumSummary.fTotalPacketsSent = stats.dwOutRequests;
            }
            result.fSummaryIOStatistics = accumSummary;
            NoteCompletedCapture_ ();
            return result;
        }
#if     qUseWMICollectionSupport_
        void    Read_WMI_ (const IO::Network::Interface& iFace, Instruments::Network::InterfaceInfo* updateResult)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Network::....Read_WMI_");
#endif
            /*
             *  @todo - this mapping of descriptions to WMI instance names is an INCREDIBLE KLUDGE. Not sure how to do this properly.
             *          a research question.
             *          --LGP 2015-04-16
             */
            String wmiInstanceName = iFace.fDescription.Value ().ReplaceAll (L"(", L"[").ReplaceAll (L")", L"]").ReplaceAll (L"#", L"_");

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"iFace.fDescription='%s'", iFace.fDescription.Value ().c_str ());
            DbgTrace (L"wmiInstanceName='%s'", wmiInstanceName.c_str ());
#endif

            /*
             *  @todo   this fAvailableInstances_.Contains code all over the place is a horible kludge prevent WMI crashes. Not sure
             *          what the best solution is. COULD just pre-add all interfaces. BUt that might monitor/collect too much?
             *
             *          And it might miss if new interfaces are added dynamically.
             *          --LGP 2015-04-16
             */
            if (fAvailableInstances_.Contains (wmiInstanceName)) {
                fNetworkWMICollector_.AddInstancesIf (wmiInstanceName);
                fTCPv4WMICollector_.AddInstancesIf (wmiInstanceName);
                fTCPv6WMICollector_.AddInstancesIf (wmiInstanceName);
            }

            if (fAvailableInstances_.Contains (wmiInstanceName)) {
                fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kBytesReceivedPerSecond_).CopyToIf (&updateResult->fIOStatistics.fBytesPerSecondReceived);
                fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kBytesSentPerSecond_).CopyToIf (&updateResult->fIOStatistics.fBytesPerSecondSent);
                fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kPacketsReceivedPerSecond_).CopyToIf (&updateResult->fIOStatistics.fPacketsPerSecondReceived);
                fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kPacketsSentPerSecond_).CopyToIf (&updateResult->fIOStatistics.fPacketsPerSecondSent);

                updateResult->fIOStatistics.fTCPSegmentsPerSecond.AccumulateIf (fTCPv4WMICollector_.PeekCurrentValue (wmiInstanceName, kTCPSegmentsPerSecond_));
                updateResult->fIOStatistics.fTCPSegmentsPerSecond.AccumulateIf (fTCPv6WMICollector_.PeekCurrentValue (wmiInstanceName, kTCPSegmentsPerSecond_));

                updateResult->fIOStatistics.fTCPRetransmittedSegmentsPerSecond.AccumulateIf (fTCPv4WMICollector_.PeekCurrentValue (wmiInstanceName, kSegmentsRetransmittedPerSecond_));
                updateResult->fIOStatistics.fTCPRetransmittedSegmentsPerSecond.AccumulateIf (fTCPv6WMICollector_.PeekCurrentValue (wmiInstanceName, kSegmentsRetransmittedPerSecond_));
            }
        }
#endif
    };
}
#endif







namespace {
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_AIX
            , CapturerWithContext_AIX_
#elif   qPlatform_POSIX
            , CapturerWithContext_POSIX_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_AIX
        using inherited = CapturerWithContext_AIX_;
#elif   qPlatform_POSIX
        using inherited = CapturerWithContext_POSIX_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Instruments::Network::Info    capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Network::capture_");
#endif
            return inherited::capture ();
        }
    };
}




namespace {
    const   MeasurementType kNetworkInterfacesMeasurement_ = MeasurementType (String_Constant (L"Network-Interfaces"));
}




/*
 ********************************************************************************
 ***************** Instruments::Network::GetObjectVariantMapper *****************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Network::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<String>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Interface::Type> (Interface::Stroika_Enum_Names(Type)));
        mapper.AddCommonType<Optional<Interface::Type>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Interface::Status> (Interface::Stroika_Enum_Names(Status)));
        mapper.AddCommonType<Set<Interface::Status>> ();
        mapper.AddCommonType<Optional<Set<Interface::Status>>> ();
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        using   Interface = InterfaceInfo::Interface;
        mapper.AddClass<Interface> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fInternalInterfaceID), String_Constant (L"Interface-Internal-ID") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fFriendlyName), String_Constant (L"Friendly-Name") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fDescription), String_Constant (L"Description"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fType), String_Constant (L"Interface-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fStatus), String_Constant (L"Interface-Status"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fTransmitSpeedBaud), String_Constant (L"Transmit-Speed-Baud"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fReceiveLinkSpeedBaud), String_Constant (L"Receive-Link-Speed-Baud"), StructureFieldInfo::NullFieldHandling::eOmit },
            // TODO ADD:
            //Containers::Set<InternetAddress>            fBindings;
        });
        mapper.AddClass<IOStatistics> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalBytesSent), String_Constant (L"Total-Bytes-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalBytesReceived), String_Constant (L"Total-Bytes-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fBytesPerSecondSent), String_Constant (L"Bytes-Per-Second-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fBytesPerSecondReceived), String_Constant (L"Bytes-Per-Second-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalTCPSegments), String_Constant (L"Total-TCP-Segments"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTCPSegmentsPerSecond), String_Constant (L"TCP-Segments-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalTCPRetransmittedSegments), String_Constant (L"Total-TCP-Retransmitted-Segments"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTCPRetransmittedSegmentsPerSecond), String_Constant (L"TCP-Retransmitted-Segments-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalPacketsSent), String_Constant (L"Total-Packets-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalPacketsReceived), String_Constant (L"Total-Packets-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fPacketsPerSecondSent), String_Constant (L"Packets-Per-Second-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fPacketsPerSecondReceived), String_Constant (L"Packets-Per-Second-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalErrors), String_Constant (L"Total-Errors"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalPacketsDropped), String_Constant (L"Total-Packets-Dropped"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<InterfaceInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo, fInterface), String_Constant (L"Interface") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo, fIOStatistics), String_Constant (L"IO-Statistics") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<Collection<InterfaceInfo>> ();
        mapper.AddCommonType<Optional<Collection<InterfaceInfo>>> ();
        mapper.AddCommonType<Optional<IOStatistics>> ();
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fInterfaces), String_Constant (L"Interfaces"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fSummaryIOStatistics), String_Constant (L"Summary-IO-Statistics"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        return mapper;
    } ();
    return sMapper_;
}






namespace {
    class   MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;
    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet  Capture () override
        {
            MeasurementSet  results;
            Measurement     m { kNetworkInterfacesMeasurement_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info  Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType    before = fCaptureContext.fLastCapturedAt;
            Info rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.fLastCapturedAt);
            }
            return rawMeasurement;
        }
        virtual unique_ptr<ICapturer>   Clone () const override
        {
#if     qCompilerAndStdLib_make_unique_Buggy
            return unique_ptr<ICapturer> (new MyCapturer_ (fCaptureContext));
#else
            return make_unique<MyCapturer_> (fCaptureContext);
#endif
        }
    };
}




/*
 ********************************************************************************
 ****************** Instruments::Network::GetInstrument *************************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::Network::GetInstrument (Options options)
{
    return  Instrument (
                InstrumentNameType (String_Constant { L"Network" }),
#if     qCompilerAndStdLib_make_unique_Buggy
                Instrument::SharedByValueCaptureRepType (unique_ptr<MyCapturer_> (new MyCapturer_ (CapturerWithContext_ { options }))),
#else
                Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_ { options })),
#endif
    { kNetworkInterfacesMeasurement_ },
    GetObjectVariantMapper ()
            );
}




/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template    <>
Instruments::Network::Info   SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

