/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <WinSock2.h>

#include <Iphlpapi.h>
#endif

#include "../../../Foundation/Characters/FloatConversion.h"
#include "../../../Foundation/Characters/String2Int.h"
#include "../../../Foundation/Characters/String_Constant.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"
#include "../../../Foundation/Debug/Assertions.h"
#include "../../../Foundation/Debug/Trace.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "../../../Foundation/Execution/ProcessRunner.h"
#include "../../../Foundation/Execution/Sleep.h"
#include "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../../Foundation/Streams/InputStream.h"
#include "../../../Foundation/Streams/MemoryStream.h"
#include "../../../Foundation/Streams/TextReader.h"

#include "Network.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;
using namespace Stroika::Frameworks::SystemPerformance::Instruments::Network;

using Characters::Character;
using Characters::String_Constant;
using Containers::Mapping;
using Containers::Sequence;
using Containers::Set;
using IO::FileSystem::FileInputStream;
using Time::DurationSecondsType;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_ qPlatform_Windows
#endif

#ifndef qSupportProcNet_
#define qSupportProcNet_ qPlatform_Linux
#endif

#if qUseWMICollectionSupport_
#include "../Support/WMICollector.h"

using SystemPerformance::Support::WMICollector;
#endif

#if qUseWMICollectionSupport_
namespace {
    const String_Constant kBytesReceivedPerSecond_{L"Bytes Received/sec"};
    const String_Constant kBytesSentPerSecond_{L"Bytes Sent/sec"};
    const String_Constant kPacketsReceivedPerSecond_{L"Packets Received/sec"};
    const String_Constant kPacketsSentPerSecond_{L"Packets Sent/sec"};
    const String_Constant kTCPSegmentsPerSecond_{L"Segments/sec"};
    const String_Constant kSegmentsRetransmittedPerSecond_{L"Segments Retransmitted/sec"};
}
#endif

#if qPlatform_Windows
#pragma comment(lib, "iphlpapi.lib")
#endif

/*
 ********************************************************************************
 *********************** Instruments::Network::IOStatistics *********************
 ********************************************************************************
 */
IOStatistics& IOStatistics::operator+= (const IOStatistics& rhs)
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

/*
 ********************************************************************************
 ******************* Instruments::Network::IOStatistics *************************
 ********************************************************************************
 */
String Instruments::Network::IOStatistics::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
********************************************************************************
********************** Instruments::Network::Info ******************************
********************************************************************************
*/
String Instruments::Network::Info::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
********************************************************************************
********************** Instruments::Network::InterfaceInfo *********************
********************************************************************************
*/
String Instruments::Network::InterfaceInfo::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

namespace {
    struct CapturerWithContext_COMMON_ {
        Options             fOptions_;
        DurationSecondsType fMinimumAveragingInterval_{};
        DurationSecondsType fPostponeCaptureUntil_{0};
        DurationSecondsType fLastCapturedAt{};
        CapturerWithContext_COMMON_ () = delete;
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DurationSecondsType GetLastCaptureAt () const { return fLastCapturedAt; }
        void                NoteCompletedCapture_ ()
        {
            auto now               = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt        = now;
        }
    };
}

#if qPlatform_POSIX
namespace {
    struct CapturerWithContext_POSIX_ : CapturerWithContext_COMMON_ {

        struct Last {
            uint64_t            fTotalBytesReceived;
            uint64_t            fTotalBytesSent;
            uint64_t            fTotalPacketsReceived;
            uint64_t            fTotalPacketsSent;
            DurationSecondsType fAt;
        };
        Mapping<String, Last> fLast;
        struct LastSum {
            uint64_t            fTotalTCPSegments;
            uint64_t            fTotalTCPRetransmittedSegments;
            DurationSecondsType fAt;
        };
        Optional<LastSum> fLastSum;
        CapturerWithContext_POSIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // hack for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed."); // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_&) = default; // copy by value fine - no need to re-wait...
        Instruments::Network::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Network::Info capture_ ()
        {
            using Instruments::Network::Info;
            using Instruments::Network::InterfaceInfo;

            Collection<InterfaceInfo> interfaceResults;
            IOStatistics              accumSummary;
#if qSupportProcNet_
            Read_proc_net_dev_ (&interfaceResults, &accumSummary);
            Read_proc_net_snmp_ (&accumSummary);
#endif

            DurationSecondsType now = Time::GetTickCount ();
            if (fLastSum and accumSummary.fTotalTCPSegments) {
                Time::DurationSecondsType timespan{now - fLastSum->fAt};
                accumSummary.fTCPSegmentsPerSecond = (ValueOrDefault (accumSummary.fTotalTCPSegments) - fLastSum->fTotalTCPSegments) / timespan;
            }
            if (fLastSum and accumSummary.fTotalTCPRetransmittedSegments) {
                Time::DurationSecondsType timespan{now - fLastSum->fAt};
                accumSummary.fTCPRetransmittedSegmentsPerSecond = (ValueOrDefault (accumSummary.fTotalTCPRetransmittedSegments) - fLastSum->fTotalTCPRetransmittedSegments) / timespan;
            }
            if (accumSummary.fTotalTCPSegments and accumSummary.fTotalTCPRetransmittedSegments) {
                fLastSum = LastSum{*accumSummary.fTotalTCPSegments, *accumSummary.fTotalTCPRetransmittedSegments, now};
            }
            NoteCompletedCapture_ ();
            return Info{interfaceResults, accumSummary};
        }
#if qSupportProcNet_
        void Read_proc_net_dev_ (Collection<Instruments::Network::InterfaceInfo>* interfaceResults, IOStatistics* accumSummary)
        {
            using Instruments::Network::InterfaceInfo;
            RequireNotNull (interfaceResults);
            RequireNotNull (accumSummary);
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{':', ' ', '\t'}};
            static const String_Constant                           kProcFileName_{L"/proc/net/dev"};
            //static    const String_Constant kProcFileName_ { L"c:\\Sandbox\\VMSharedFolder\\proc-net-dev" };
            // Note - /procfs files always unseekable
            unsigned int nLine  = 0;
            unsigned int n2Skip = 2;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::Network::Info capture_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                nLine++;
                if (n2Skip > 0) {
                    --n2Skip;
                    continue;
                }
                if (line.size () >= 17) {
                    constexpr int kOffset2XMit_ = 8;
                    InterfaceInfo ii;
                    if (auto info = IO::Network::GetInterfaceById (line[0])) {
                        ii.fInterface = *info;
                    }
                    else {
                        ii.fInterface.fInternalInterfaceID = line[0];
                        ii.fInterface.fFriendlyName        = line[0];
                    }
                    ii.fIOStatistics.fTotalBytesReceived   = Characters::String2Int<uint64_t> (line[1]);
                    ii.fIOStatistics.fTotalBytesSent       = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 1]);
                    ii.fIOStatistics.fTotalPacketsReceived = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 2]);
                    ii.fIOStatistics.fTotalPacketsSent     = Characters::String2Int<uint64_t> (line[2]);
                    ii.fIOStatistics.fTotalErrors          = Characters::String2Int<uint64_t> (line[3]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 3]);
                    ii.fIOStatistics.fTotalPacketsDropped  = Characters::String2Int<uint64_t> (line[4]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 4]);

                    DurationSecondsType now = Time::GetTickCount ();
                    if (auto o = fLast.Lookup (ii.fInterface.fInternalInterfaceID)) {
                        double scanTime = now - o->fAt;
                        if (scanTime > 0) {
                            ii.fIOStatistics.fBytesPerSecondReceived   = (*ii.fIOStatistics.fTotalBytesReceived - o->fTotalBytesReceived) / scanTime;
                            ii.fIOStatistics.fBytesPerSecondSent       = (*ii.fIOStatistics.fTotalBytesSent - o->fTotalBytesSent) / scanTime;
                            ii.fIOStatistics.fPacketsPerSecondReceived = (*ii.fIOStatistics.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                            ii.fIOStatistics.fPacketsPerSecondSent     = (*ii.fIOStatistics.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                        }
                    }
                    (*accumSummary) += ii.fIOStatistics;
                    interfaceResults->Add (ii);
                    fLast.Add (ii.fInterface.fInternalInterfaceID, Last{*ii.fIOStatistics.fTotalBytesReceived, *ii.fIOStatistics.fTotalBytesSent, *ii.fIOStatistics.fTotalPacketsReceived, *ii.fIOStatistics.fTotalPacketsSent, now});
                }
                else {
                    DbgTrace (L"Line %d bad in file %s", nLine, kProcFileName_.c_str ());
                }
            }
        }
        void Read_proc_net_netstat_ (IOStatistics* accumSummary)
        {
            AssertNotReached (); // dont use this for now
            RequireNotNull (accumSummary);
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            static const String_Constant                           kProcFileName_{L"/proc/net/netstat"};
            // Note - /procfs files always unseekable
            bool                    firstTime = true;
            Mapping<String, size_t> labelMap;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::Network::Info Read_proc_net_netstat_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                if (line.size () >= 2 and line[0].Trim () == L"TcpExt:") {
                    if (firstTime) {
                        size_t idx = 0;
                        for (String i : line) {
                            labelMap.Add (i.Trim (), idx++);
                        }
                        firstTime = false;
                    }
                    else {
                        // @todo must sum several fields - NYI
                        if (auto oTCPSynRetransIdx = labelMap.Lookup (L"TCPSynRetrans")) {
                            if (*oTCPSynRetransIdx < line.length ()) {
                                uint64_t TCPSynRetrans                       = Characters::String2Int<uint64_t> (line[*oTCPSynRetransIdx]);
                                accumSummary->fTotalTCPRetransmittedSegments = TCPSynRetrans;
                            }
                        }
                    }
                }
            }
        }
        void Read_proc_net_snmp_ (IOStatistics* accumSummary)
        {
            RequireNotNull (accumSummary);
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            static const String_Constant                           kProcFileName_{L"/proc/net/snmp"};
            // Note - /procfs files always unseekable
            bool                    firstTime = true;
            Mapping<String, size_t> labelMap;
            Optional<uint64_t>      totalTCPSegments;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::Network::Info Read_proc_net_snmp_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                if (line.size () >= 2 and line[0].Trim () == L"Tcp:") {
                    if (firstTime) {
                        size_t idx = 0;
                        for (String i : line) {
                            labelMap.Add (i.Trim (), idx++);
                        }
                        firstTime = false;
                    }
                    else {
                        static const String_Constant kInSegs_{L"InSegs"};
                        static const String_Constant kOutSegs_{L"OutSegs"};
                        static const String_Constant kRetransSegs_{L"RetransSegs"};
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

#if qPlatform_Windows
namespace {
    struct CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if qUseWMICollectionSupport_
        WMICollector fNetworkWMICollector_{String_Constant{L"Network Interface"}, {}, {kBytesReceivedPerSecond_, kBytesSentPerSecond_, kPacketsReceivedPerSecond_, kPacketsSentPerSecond_}};
        WMICollector fTCPv4WMICollector_{String_Constant{L"TCPv4"}, {}, {kTCPSegmentsPerSecond_, kSegmentsRetransmittedPerSecond_}};
        WMICollector fTCPv6WMICollector_{String_Constant{L"TCPv6"}, {}, {kTCPSegmentsPerSecond_, kSegmentsRetransmittedPerSecond_}};
        Set<String>  fAvailableInstances_;
#endif
        CapturerWithContext_Windows_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
#if qUseWMICollectionSupport_
            fAvailableInstances_ = fNetworkWMICollector_.GetAvailableInstaces ();
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            {
                Debug::TraceContextBumper ctx ("ALL WMI Network Avialable instances");
                for (String i : fAvailableInstances_) {
                    DbgTrace (L"wmiInstanceName='%s'", i.c_str ());
                }
            }
#endif
            capture_ (); // for the side-effect of filling in fNetworkWMICollector_ with interfaces and doing initial capture so WMI can compute averages
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if qUseWMICollectionSupport_
            , fNetworkWMICollector_ (from.fNetworkWMICollector_)
            , fTCPv4WMICollector_ (from.fTCPv4WMICollector_)
            , fTCPv6WMICollector_ (from.fTCPv6WMICollector_)
            , fAvailableInstances_ (from.fAvailableInstances_)
#endif
        {
#if qUseWMICollectionSupport_
            capture_ (); // for the side-effect of filling in fNetworkWMICollector_ with interfaces and doing initial capture so WMI can compute averages
#endif
        }
        Instruments::Network::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Network::Info capture_ ()
        {
            using Instruments::Network::Info;
            using Instruments::Network::InterfaceInfo;
            using IO::Network::Interface;

            Info                      result;
            Collection<InterfaceInfo> interfaceResults;
            IOStatistics              accumSummary;
            Iterable<Interface>       networkInterfacs{IO::Network::GetInterfaces ()};
#if qUseWMICollectionSupport_
            fNetworkWMICollector_.Collect ();
            fTCPv4WMICollector_.Collect ();
            fTCPv6WMICollector_.Collect ();
#endif
            {
                for (IO::Network::Interface networkInterface : networkInterfacs) {
                    InterfaceInfo ii;
#if 0
                    ii.fInternalInterfaceID = networkInterface.fInternalInterfaceID;
                    ii.fDisplayName = networkInterface.fFriendlyName;
                    ii.fInterfaceType = networkInterface.fType;
                    ii.fInterfaceStatus = networkInterface.fStatus;
#endif
                    ii.fInterface = networkInterface;
#if qUseWMICollectionSupport_
                    Read_WMI_ (networkInterface, &ii);
#endif
                    accumSummary += ii.fIOStatistics;
                    interfaceResults.Add (ii);
                }
            }
            result.fInterfaces = interfaceResults;
            {
                MIB_IPSTATS stats{}; // maybe more useful stats we could pull out of this?
                memset (&stats, 0, sizeof (stats));
                Execution::Platform::Windows::ThrowIfNot_NO_ERROR (::GetIpStatistics (&stats));
                accumSummary.fTotalPacketsReceived = stats.dwInReceives;
                accumSummary.fTotalPacketsSent     = stats.dwOutRequests;
            }
            result.fSummaryIOStatistics = accumSummary;
            NoteCompletedCapture_ ();
            return result;
        }
#if qUseWMICollectionSupport_
        void Read_WMI_ (const IO::Network::Interface& iFace, Instruments::Network::InterfaceInfo* updateResult)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Network::....Read_WMI_");
#endif
            /*
             *  @todo - this mapping of descriptions to WMI instance names is an INCREDIBLE KLUDGE. Not sure how to do this properly.
             *          a research question.
             *          --LGP 2015-04-16
             */
            String wmiInstanceName = iFace.fDescription.Value ().ReplaceAll (L"(", L"[").ReplaceAll (L")", L"]").ReplaceAll (L"#", L"_");

#if USE_NOISY_TRACE_IN_THIS_MODULE_
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
    struct CapturerWithContext_
        : Debug::AssertExternallySynchronizedLock
#if qPlatform_POSIX
        ,
          CapturerWithContext_POSIX_
#elif qPlatform_Windows
        ,
          CapturerWithContext_Windows_
#endif
    {
#if qPlatform_POSIX
        using inherited = CapturerWithContext_POSIX_;
#elif qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Instruments::Network::Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Network::capture_");
#endif
            return inherited::capture ();
        }
    };
}

namespace {
    const MeasurementType kNetworkInterfacesMeasurement_ = MeasurementType (String_Constant (L"Network-Interfaces"));
}

/*
 ********************************************************************************
 ***************** Instruments::Network::GetObjectVariantMapper *****************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Network::GetObjectVariantMapper ()
{
    using StructFieldInfo                     = ObjectVariantMapper::StructFieldInfo;
    static const ObjectVariantMapper sMapper_ = []() -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<String>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Interface::Type> ());
        mapper.AddCommonType<Optional<Interface::Type>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Interface::Status> ());
        mapper.AddCommonType<Set<Interface::Status>> ();
        mapper.AddCommonType<Optional<Set<Interface::Status>>> ();
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        using Interface = InterfaceInfo::Interface;
        mapper.AddClass<Interface> (initializer_list<StructFieldInfo>{
            {L"Interface-Internal-ID", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fInternalInterfaceID)},
            {L"Friendly-Name", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fFriendlyName)},
            {L"Description", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fDescription), StructFieldInfo::eOmitNullFields},
            {L"Interface-Type", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fType), StructFieldInfo::eOmitNullFields},
            {L"Hardware-Address", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fHardwareAddress), StructFieldInfo::eOmitNullFields},
            {L"Interface-Status", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fStatus), StructFieldInfo::eOmitNullFields},
            {L"Transmit-Speed-Baud", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fTransmitSpeedBaud), StructFieldInfo::eOmitNullFields},
            {L"Receive-Link-Speed-Baud", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Interface, fReceiveLinkSpeedBaud), StructFieldInfo::eOmitNullFields},
            // TODO ADD:
            //Containers::Set<InternetAddress>            fBindings;
        });
        mapper.AddClass<IOStatistics> (initializer_list<StructFieldInfo>{
            {L"Total-Bytes-Sent", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalBytesSent), StructFieldInfo::eOmitNullFields},
            {L"Total-Bytes-Received", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalBytesReceived), StructFieldInfo::eOmitNullFields},
            {L"Bytes-Per-Second-Sent", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fBytesPerSecondSent), StructFieldInfo::eOmitNullFields},
            {L"Bytes-Per-Second-Received", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fBytesPerSecondReceived), StructFieldInfo::eOmitNullFields},
            {L"Total-TCP-Segments", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalTCPSegments), StructFieldInfo::eOmitNullFields},
            {L"TCP-Segments-Per-Second", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTCPSegmentsPerSecond), StructFieldInfo::eOmitNullFields},
            {L"Total-TCP-Retransmitted-Segments", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalTCPRetransmittedSegments), StructFieldInfo::eOmitNullFields},
            {L"TCP-Retransmitted-Segments-Per-Second", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTCPRetransmittedSegmentsPerSecond), StructFieldInfo::eOmitNullFields},
            {L"Total-Packets-Sent", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalPacketsSent), StructFieldInfo::eOmitNullFields},
            {L"Total-Packets-Received", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalPacketsReceived), StructFieldInfo::eOmitNullFields},
            {L"Packets-Per-Second-Sent", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fPacketsPerSecondSent), StructFieldInfo::eOmitNullFields},
            {L"Packets-Per-Second-Received", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fPacketsPerSecondReceived), StructFieldInfo::eOmitNullFields},
            {L"Total-Errors", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalErrors), StructFieldInfo::eOmitNullFields},
            {L"Total-Packets-Dropped", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatistics, fTotalPacketsDropped), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddClass<InterfaceInfo> (initializer_list<StructFieldInfo>{
            {L"Interface", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo, fInterface)},
            {L"IO-Statistics", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo, fIOStatistics)},
        });
        mapper.AddCommonType<Collection<InterfaceInfo>> ();
        mapper.AddCommonType<Optional<Collection<InterfaceInfo>>> ();
        mapper.AddCommonType<Optional<IOStatistics>> ();
        mapper.AddClass<Info> (initializer_list<StructFieldInfo>{
            {L"Interfaces", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fInterfaces), StructFieldInfo::eOmitNullFields},
            {L"Summary-IO-Statistics", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fSummaryIOStatistics), StructFieldInfo::eOmitNullFields},
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    }();
    return sMapper_;
}

namespace {
    class MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;

    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet Capture () override
        {
            MeasurementSet results;
            Measurement    m{kNetworkInterfacesMeasurement_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType before         = fCaptureContext.fLastCapturedAt;
            Info                rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.fLastCapturedAt);
            }
            return rawMeasurement;
        }
        virtual unique_ptr<ICapturer> Clone () const override
        {
            return make_unique<MyCapturer_> (fCaptureContext);
        }
    };
}

/*
 ********************************************************************************
 ****************** Instruments::Network::GetInstrument *************************
 ********************************************************************************
 */
Instrument SystemPerformance::Instruments::Network::GetInstrument (Options options)
{
    return Instrument (
        InstrumentNameType (String_Constant{L"Network"}),
        Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_{options})),
        {kNetworkInterfacesMeasurement_},
        GetObjectVariantMapper ());
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::Network::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_* myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
