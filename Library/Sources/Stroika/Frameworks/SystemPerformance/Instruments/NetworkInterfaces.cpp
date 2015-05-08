/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <WinSock2.h>
#include    <Iphlpapi.h>
#endif

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
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
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/Streams/BinaryInputStream.h"

#include    "NetworkInterfaces.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;


using   Characters::Character;
using   Characters::String_Constant;
using   Containers::Mapping;
using   Containers::Sequence;
using   Containers::Set;
using   IO::FileSystem::BinaryFileInputStream;
using   Time::DurationSecondsType;

using   Stroika::Frameworks::SystemPerformance::Instruments::NetworkInterfaces::InterfaceInfo;
using   Stroika::Frameworks::SystemPerformance::Instruments::NetworkInterfaces::IOStatistics;
using   Stroika::Frameworks::SystemPerformance::Instruments::NetworkInterfaces::Options;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
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
#pragma comment(lib, "iphlpapi.lib")
#endif





/*
 ********************************************************************************
 ****************** Instruments::NetworkInterfaces::InterfaceInfo ***************
 ********************************************************************************
 */
const Configuration::EnumNames<InterfaceInfo::Type>&   InterfaceInfo::Stroika_Enum_Names(Type)          =   IO::Network::Interface::Stroika_Enum_Names(Type);
const Configuration::EnumNames<InterfaceInfo::Status>&   InterfaceInfo::Stroika_Enum_Names(Status)      =   IO::Network::Interface::Stroika_Enum_Names(Status);




/*
 ********************************************************************************
 ****************** Instruments::NetworkInterfaces::IOStatistics ****************
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
        DurationSecondsType         fMinimumAveragingInterval_;
        DurationSecondsType         fPostponeCaptureUntil_ { 0 };
        DateTime                    fLastCapturedAt;
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        void    NoteCompletedCapture_ ()
        {
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            fLastCapturedAt = DateTime::Now ();
        }
    };
}







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
            capture_ ();    // hack for side-effect of  updating aved_MajorPageFaultsSinc etc
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_&) = default;   // copy by value fine - no need to re-wait...
        Instruments::NetworkInterfaces::Info    capture_ ()
        {
            using   Instruments::NetworkInterfaces::InterfaceInfo;
            using   Instruments::NetworkInterfaces::Info;

            Execution::SleepUntil (fPostponeCaptureUntil_);

            Collection<InterfaceInfo>   interfaceResults;
            IOStatistics                accumSummary;
            Read_proc_net_dev_ (&interfaceResults, &accumSummary);
            Read_proc_net_snmp_ (&accumSummary);

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
        void    Read_proc_net_dev_ (Collection<Instruments::NetworkInterfaces::InterfaceInfo>* interfaceResults, IOStatistics* accumSummary)
        {
            using   Instruments::NetworkInterfaces::InterfaceInfo;
            RequireNotNull (interfaceResults);
            RequireNotNull (accumSummary);
            DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
            static  const   String_Constant kProcFileName_ { L"/proc/net/dev" };
            //static    const String_Constant kProcFileName_ { L"c:\\Sandbox\\VMSharedFolder\\proc-net-dev" };
            // Note - /procfs files always unseekable
            unsigned int    nLine   = 0;
            unsigned int    n2Skip  = 2;
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::NetworkInterfaces::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                nLine++;
                if (n2Skip > 0) {
                    --n2Skip;
                    continue;
                }
                if (line.size () >= 17) {
                    constexpr   int kOffset2XMit_ = 8;
                    InterfaceInfo   ii;
                    ii.fInterfaceID = line[0];
                    ii.fInternalInterfaceID = line[0];
                    if (auto info = IO::Network::GetInterfaceById (line[0])) {
                        ii.fDisplayName = info->fFriendlyName;
                        ii.fInterfaceType = info->fType;
                        ii.fInterfaceStatus = info->fStatus;
                    }
                    else {
                        ii.fDisplayName = line[0];
                    }
                    ii.fIOStatistics.fTotalBytesReceived = Characters::String2Int<uint64_t> (line[1]);
                    ii.fIOStatistics.fTotalBytesSent = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 1]);
                    ii.fIOStatistics.fTotalPacketsReceived = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 2]);
                    ii.fIOStatistics.fTotalPacketsSent = Characters::String2Int<uint64_t> (line[2]);
                    ii.fIOStatistics.fTotalErrors = Characters::String2Int<uint64_t> (line[3]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 3]);
                    ii.fIOStatistics.fTotalPacketsDropped = Characters::String2Int<uint64_t> (line[4]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 4]);

                    DurationSecondsType now = Time::GetTickCount ();
                    if (auto o = fLast.Lookup (ii.fInternalInterfaceID)) {
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
                    fLast.Add (ii.fInternalInterfaceID, Last { *ii.fIOStatistics.fTotalBytesReceived, *ii.fIOStatistics.fTotalBytesSent, *ii.fIOStatistics.fTotalPacketsReceived, *ii.fIOStatistics.fTotalPacketsSent, now });
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
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::NetworkInterfaces::Info Read_proc_net_netstat_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
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
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::NetworkInterfaces::Info Read_proc_net_snmp_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
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
        Instruments::NetworkInterfaces::Info capture_ ()
        {
            using   IO::Network::Interface;
            using   Instruments::NetworkInterfaces::InterfaceInfo;
            using   Instruments::NetworkInterfaces::Info;

            Execution::SleepUntil (fPostponeCaptureUntil_);

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
                    ii.fInternalInterfaceID = networkInterface.fInternalInterfaceID;
                    ii.fDisplayName = networkInterface.fFriendlyName;
                    ii.fInterfaceType = networkInterface.fType;
                    ii.fInterfaceStatus = networkInterface.fStatus;
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
        void    Read_WMI_ (const IO::Network::Interface& iFace, Instruments::NetworkInterfaces::InterfaceInfo* updateResult)
        {
            /*
             *  @todo - this mapping of descriptions to WMI instance names is an INCREDIBLE KLUDGE. Not sure how to do this properly.
             *          a research question.
             *          --LGP 2015-04-16
             */
            String wmiInstanceName = iFace.fDescription.Value ().ReplaceAll (L"(", L"[").ReplaceAll (L")", L"]");

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
#if     qPlatform_POSIX
            , CapturerWithContext_POSIX_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_POSIX
        using inherited = CapturerWithContext_POSIX_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Instruments::NetworkInterfaces::Info    capture_ ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::NetworkInterfaces::capture_");
#endif
            return inherited::capture_ ();
        }
    };
}




namespace {
    const   MeasurementType kNetworkInterfacesMeasurement_ = MeasurementType (String_Constant (L"Network-Interfaces"));
}




/*
 ********************************************************************************
 ************** Instruments::NetworkInterfaces::GetObjectVariantMapper **********
 ********************************************************************************
 */
ObjectVariantMapper Instruments::NetworkInterfaces::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<String>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<InterfaceInfo::Type> (InterfaceInfo::Stroika_Enum_Names(Type)));
        mapper.AddCommonType<Optional<InterfaceInfo::Type>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<InterfaceInfo::Status> (InterfaceInfo::Stroika_Enum_Names(Status)));
        mapper.AddCommonType<Set<InterfaceInfo::Status>> ();
        mapper.AddCommonType<Optional<Set<InterfaceInfo::Status>>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<IOStatistics> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalBytesSent), String_Constant (L"Total-Bytes-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalBytesReceived), String_Constant (L"Total-Bytes-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fBytesPerSecondSent), String_Constant (L"Bytes-Per-Second-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fBytesPerSecondReceived), String_Constant (L"Bytes-Per-Second-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalTCPSegments), String_Constant (L"Total-TCP-Segments"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTCPSegmentsPerSecond), String_Constant (L"TCP-Segments-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalTCPRetransmittedSegments), String_Constant (L"Total-TCP-Retransmitted-Segments"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTCPRetransmittedSegmentsPerSecond), String_Constant (L"TCP-Retransmitted-Segments-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalPacketsSent), String_Constant (L"Total-Packets-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalPacketsReceived), String_Constant (L"Total-Packets-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fPacketsPerSecondSent), String_Constant (L"Packets-Per-Second-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fPacketsPerSecondReceived), String_Constant (L"Packets-Per-Second-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalErrors), String_Constant (L"Total-Errors"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (IOStatistics, fTotalPacketsDropped), String_Constant (L"Total-Packets-Dropped"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<InterfaceInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInternalInterfaceID), String_Constant (L"Interface-Internal-ID") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fDisplayName), String_Constant (L"Display-Name") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInterfaceID), String_Constant (L"Interface-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInterfaceType), String_Constant (L"Interface=Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInterfaceStatus), String_Constant (L"Interface-Status"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fIOStatistics), String_Constant (L"IO-Statistics") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<Collection<InterfaceInfo>> ();
        mapper.AddCommonType<Optional<Collection<InterfaceInfo>>> ();
        mapper.AddCommonType<Optional<IOStatistics>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fInterfaces), String_Constant (L"Interfaces"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fSummaryIOStatistics), String_Constant (L"Summary-IO-Statistics"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        return mapper;
    } ();
    return sMapper_;
}




/*
 ********************************************************************************
 ************ Instruments::NetworkInterfaces::GetInstrument *********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::NetworkInterfaces::GetInstrument (Options options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return  Instrument (
                InstrumentNameType (String_Constant (L"Network-Interfaces")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = useCaptureContext.fLastCapturedAt;
        Instruments::NetworkInterfaces::Info rawMeasurement = useCaptureContext.capture_ ();
        results.fMeasuredAt = DateTimeRange (before, useCaptureContext.fLastCapturedAt);
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kNetworkInterfacesMeasurement_;
        results.fMeasurements.Add (m);
        return results;
    },
    {kNetworkInterfacesMeasurement_},
    GetObjectVariantMapper ()
            );
}
