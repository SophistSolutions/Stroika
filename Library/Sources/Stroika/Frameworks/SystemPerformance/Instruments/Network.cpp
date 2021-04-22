/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <WinSock2.h>

#include <Iphlpapi.h>
#endif

#include "../../../Foundation/Characters/FloatConversion.h"
#include "../../../Foundation/Characters/String2Int.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"
#include "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../../Foundation/Debug/Assertions.h"
#include "../../../Foundation/Debug/Trace.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "../../../Foundation/Execution/ProcessRunner.h"
#include "../../../Foundation/Execution/Synchronized.h"
#include "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../../Foundation/Streams/InputStream.h"
#include "../../../Foundation/Streams/MemoryStream.h"
#include "../../../Foundation/Streams/TextReader.h"

#include "../Support/InstrumentHelpers.h"

#include "Network.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Characters::Character;
using Containers::Mapping;
using Containers::Sequence;
using Containers::Set;
using IO::FileSystem::FileInputStream;
using Time::DurationSecondsType;

using Instruments::Network::Info;
using Instruments::Network::IOStatistics;
using Instruments::Network::Options;

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
    const String kBytesReceivedPerSecond_{L"Bytes Received/sec"sv};
    const String kBytesSentPerSecond_{L"Bytes Sent/sec"sv};
    const String kPacketsReceivedPerSecond_{L"Packets Received/sec"sv};
    const String kPacketsSentPerSecond_{L"Packets Sent/sec"sv};
    const String kTCPSegmentsPerSecond_{L"Segments/sec"sv};
    const String kSegmentsRetransmittedPerSecond_{L"Segments Retransmitted/sec"sv};
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
    Memory::AccumulateIf (&fTotalBytesSent, rhs.fTotalBytesSent);
    Memory::AccumulateIf (&fTotalBytesReceived, rhs.fTotalBytesReceived);
    Memory::AccumulateIf (&fBytesPerSecondSent, rhs.fBytesPerSecondSent);
    Memory::AccumulateIf (&fBytesPerSecondReceived, rhs.fBytesPerSecondReceived);
    Memory::AccumulateIf (&fTotalTCPSegments, rhs.fTotalTCPSegments);
    Memory::AccumulateIf (&fTCPSegmentsPerSecond, rhs.fTCPSegmentsPerSecond);
    Memory::AccumulateIf (&fTotalTCPRetransmittedSegments, rhs.fTotalTCPRetransmittedSegments);
    Memory::AccumulateIf (&fTCPRetransmittedSegmentsPerSecond, rhs.fTCPRetransmittedSegmentsPerSecond);
    Memory::AccumulateIf (&fTotalPacketsSent, rhs.fTotalPacketsSent);
    Memory::AccumulateIf (&fTotalPacketsReceived, rhs.fTotalPacketsReceived);
    Memory::AccumulateIf (&fPacketsPerSecondSent, rhs.fPacketsPerSecondSent);
    Memory::AccumulateIf (&fPacketsPerSecondReceived, rhs.fPacketsPerSecondReceived);
    Memory::AccumulateIf (&fTotalErrors, rhs.fTotalErrors);
    Memory::AccumulateIf (&fTotalPacketsDropped, rhs.fTotalPacketsDropped);
    return *this;
}

/*
 ********************************************************************************
 ******************* Instruments::Network::IOStatistics *************************
 ********************************************************************************
 */
String Instruments::Network::IOStatistics::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Network::Instrument::kObjectVariantMapper.FromObject (*this));
}

/*
********************************************************************************
********************** Instruments::Network::Info ******************************
********************************************************************************
*/
String Instruments::Network::Info::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Network::Instrument::kObjectVariantMapper.FromObject (*this));
}

/*
********************************************************************************
********************** Instruments::Network::InterfaceInfo *********************
********************************************************************************
*/
String Instruments::Network::InterfaceInfo::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Network::Instrument::kObjectVariantMapper.FromObject (*this));
}

namespace {
    template <typename CONTEXT>
    using InstrumentRepBase_ = SystemPerformance::Support::InstrumentRep_COMMON<Options, CONTEXT>;
}

#if qPlatform_POSIX
namespace {
    struct Last {
        uint64_t            fTotalBytesReceived;
        uint64_t            fTotalBytesSent;
        uint64_t            fTotalPacketsReceived;
        uint64_t            fTotalPacketsSent;
        DurationSecondsType fAt;
    };
    struct LastSum {
        uint64_t            fTotalTCPSegments;
        uint64_t            fTotalTCPRetransmittedSegments;
        DurationSecondsType fAt;
    };

    struct _Context : SystemPerformance::Support::Context {
        Mapping<String, Last> fLast;
        optional<LastSum>     fLastSum;
    };

    struct InstrumentRep_POSIX_ : InstrumentRepBase_<_Context> {

        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        Instruments::Network::Info _InternalCapture ()
        {
            using Instruments::Network::Info;
            using Instruments::Network::InterfaceInfo;

            Collection<InterfaceInfo> interfaceResults;
            IOStatistics              accumSummary;
#if qSupportProcNet_
            // Some Linux builds may not have all (any) of the /proc stuff mounted (e.g. WSL 1)
            IgnoreExceptionsExceptThreadAbortForCall (Read_proc_net_dev_ (&interfaceResults, &accumSummary));
            IgnoreExceptionsExceptThreadAbortForCall (Read_proc_net_snmp_ (&accumSummary));
#endif

            DurationSecondsType  now         = Time::GetTickCount ();
            auto                 contextLock = scoped_lock{_fContext};
            shared_ptr<_Context> context     = _fContext.rwget ().rwref ();
            if (context->fLastSum and accumSummary.fTotalTCPSegments) {
                Time::DurationSecondsType timespan{now - context->fLastSum->fAt};
                if (timespan >= _fOptions.fMinimumAveragingInterval) {
                    accumSummary.fTCPSegmentsPerSecond = (NullCoalesce (accumSummary.fTotalTCPSegments) - context->fLastSum->fTotalTCPSegments) / timespan;
                }
            }
            if (context->fLastSum and accumSummary.fTotalTCPRetransmittedSegments) {
                Time::DurationSecondsType timespan{now - context->fLastSum->fAt};
                if (timespan >= _fOptions.fMinimumAveragingInterval) {
                    accumSummary.fTCPRetransmittedSegmentsPerSecond = (NullCoalesce (accumSummary.fTotalTCPRetransmittedSegments) - context->fLastSum->fTotalTCPRetransmittedSegments) / timespan;
                }
            }
            if (accumSummary.fTotalTCPSegments and accumSummary.fTotalTCPRetransmittedSegments) {
                context->fLastSum = LastSum{*accumSummary.fTotalTCPSegments, *accumSummary.fTotalTCPRetransmittedSegments, now};
            }
            _NoteCompletedCapture ();
            return Info{interfaceResults, accumSummary};
        }
#if qSupportProcNet_
        void Read_proc_net_dev_ (Collection<Instruments::Network::InterfaceInfo>* interfaceResults, IOStatistics* accumSummary)
        {
            SystemInterfacesMgr systemInterfacesMgr;
            using Instruments::Network::InterfaceInfo;
            RequireNotNull (interfaceResults);
            RequireNotNull (accumSummary);
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{':', ' ', '\t'}};
            static const filesystem::path                          kProcFileName_{"/proc/net/dev"};
            //static    const String kProcFileName_ { L"c:\\Sandbox\\VMSharedFolder\\proc-net-dev"sv };
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
                    if (auto info = systemInterfacesMgr.GetById (line[0])) {
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
                    if (auto o = _fContext.cget ().cref ()->fLast.Lookup (ii.fInterface.fInternalInterfaceID)) {
                        double scanTime = now - o->fAt;
                        if (scanTime > _fOptions.fMinimumAveragingInterval) {
                            ii.fIOStatistics.fBytesPerSecondReceived   = (*ii.fIOStatistics.fTotalBytesReceived - o->fTotalBytesReceived) / scanTime;
                            ii.fIOStatistics.fBytesPerSecondSent       = (*ii.fIOStatistics.fTotalBytesSent - o->fTotalBytesSent) / scanTime;
                            ii.fIOStatistics.fPacketsPerSecondReceived = (*ii.fIOStatistics.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                            ii.fIOStatistics.fPacketsPerSecondSent     = (*ii.fIOStatistics.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                        }
                    }
                    (*accumSummary) += ii.fIOStatistics;
                    interfaceResults->Add (ii);
                    _fContext.rwget ().rwref ()->fLast.Add (ii.fInterface.fInternalInterfaceID, Last{*ii.fIOStatistics.fTotalBytesReceived, *ii.fIOStatistics.fTotalBytesSent, *ii.fIOStatistics.fTotalPacketsReceived, *ii.fIOStatistics.fTotalPacketsSent, now});
                }
                else {
                    DbgTrace (L"Line %d bad in file %s", nLine, kProcFileName_.c_str ());
                }
            }
        }
        void Read_proc_net_netstat_ (IOStatistics* accumSummary)
        {
            AssertNotReached (); // don't use this for now
            RequireNotNull (accumSummary);
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            static const filesystem::path                          kProcFileName_{"/proc/net/netstat"};
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
            static const filesystem::path                          kProcFileName_{"/proc/net/snmp"};
            // Note - /procfs files always unseekable
            bool                    firstTime = true;
            Mapping<String, size_t> labelMap;
            optional<uint64_t>      totalTCPSegments;
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
                        static const String kInSegs_{L"InSegs"sv};
                        static const String kOutSegs_{L"OutSegs"sv};
                        static const String kRetransSegs_{L"RetransSegs"sv};
                        if (auto idx = labelMap.Lookup (kInSegs_)) {
                            if (*idx < line.length ()) {
                                Memory::AccumulateIf (&totalTCPSegments, Characters::String2Int<uint64_t> (line[*idx]));
                            }
                        }
                        if (auto idx = labelMap.Lookup (kOutSegs_)) {
                            if (*idx < line.length ()) {
                                Memory::AccumulateIf (&totalTCPSegments, Characters::String2Int<uint64_t> (line[*idx]));
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
            Memory::CopyToIf (totalTCPSegments, &accumSummary->fTotalTCPSegments);
        }
#endif
    };
}
#endif

#if qPlatform_Windows
namespace {
    struct _Context : SystemPerformance::Support::Context {
#if qUseWMICollectionSupport_
        WMICollector fNetworkWMICollector_{L"Network Interface"sv, {}, {kBytesReceivedPerSecond_, kBytesSentPerSecond_, kPacketsReceivedPerSecond_, kPacketsSentPerSecond_}};
        WMICollector fTCPv4WMICollector_{L"TCPv4"sv, {}, {kTCPSegmentsPerSecond_, kSegmentsRetransmittedPerSecond_}};
        WMICollector fTCPv6WMICollector_{L"TCPv6"sv, {}, {kTCPSegmentsPerSecond_, kSegmentsRetransmittedPerSecond_}};
        Set<String>  fAvailableInstances_{fNetworkWMICollector_.GetAvailableInstaces ()};
#endif
    };

    struct InstrumentRep_Windows_ : InstrumentRepBase_<_Context> {

        InstrumentRep_Windows_ (const Options& options, const shared_ptr<_Context>& context)
            : InstrumentRepBase_<_Context>{options, context}
        {
#if qUseWMICollectionSupport_
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            {
                Debug::TraceContextBumper ctx{"ALL WMI Network Avialable instances"};
                for (String i : fAvailableInstances_) {
                    DbgTrace (L"wmiInstanceName='%s'", i.c_str ());
                }
            }
#endif
#endif
        }

        Instruments::Network::Info _InternalCapture ()
        {
            using Instruments::Network::Info;
            using Instruments::Network::InterfaceInfo;
            using IO::Network::Interface;

            Info                      result;
            Collection<InterfaceInfo> interfaceResults;
            IOStatistics              accumSummary;
            SystemInterfacesMgr       systemInterfacesMgr;
            Iterable<Interface>       networkInterfacs{systemInterfacesMgr.GetAll ()};
#if qUseWMICollectionSupport_
            _fContext.rwget ().rwref ()->fNetworkWMICollector_.Collect ();
            _fContext.rwget ().rwref ()->fTCPv4WMICollector_.Collect ();
            _fContext.rwget ().rwref ()->fTCPv6WMICollector_.Collect ();
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
                Execution::Platform::Windows::ThrowIfNot_NO_ERROR (::GetIpStatistics (&stats));
                accumSummary.fTotalPacketsReceived = stats.dwInReceives;
                accumSummary.fTotalPacketsSent     = stats.dwOutRequests;
            }
            result.fSummaryIOStatistics = accumSummary;
            _NoteCompletedCapture ();
            return result;
        }
#if qUseWMICollectionSupport_
        void Read_WMI_ (const IO::Network::Interface& iFace, Instruments::Network::InterfaceInfo* updateResult)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::Network::....Read_WMI_"};
#endif
            /*
             *  @todo - this mapping of descriptions to WMI instance names is an INCREDIBLE KLUDGE. Not sure how to do this properly.
             *          a research question.
             *          --LGP 2015-04-16
             */
            String wmiInstanceName = NullCoalesce (iFace.fDescription).ReplaceAll (L"(", L"[").ReplaceAll (L")", L"]").ReplaceAll (L"#", L"_");

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
            if (_fContext.cget ().cref ()->fAvailableInstances_.Contains (wmiInstanceName)) {
                auto lock    = scoped_lock{_fContext};
                auto context = _fContext.cget ().cref ();
                context->fNetworkWMICollector_.AddInstancesIf (wmiInstanceName);
                context->fTCPv4WMICollector_.AddInstancesIf (wmiInstanceName);
                context->fTCPv6WMICollector_.AddInstancesIf (wmiInstanceName);
            }

            if (_fContext.cget ().cref ()->fAvailableInstances_.Contains (wmiInstanceName)) {
                auto lock    = scoped_lock{_fContext};
                auto context = _fContext.rwget ().rwref ();
                /*
                 *  Note because WMI maintains these per/second numbers, its not clear there is a need for me to strip them out if
                 *  lastCapture time since now < = _fOptions.fMinimumAveragingInterval
                 */
                Memory::CopyToIf (context->fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kBytesReceivedPerSecond_), &updateResult->fIOStatistics.fBytesPerSecondReceived);
                Memory::CopyToIf (context->fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kBytesSentPerSecond_), &updateResult->fIOStatistics.fBytesPerSecondSent);
                Memory::CopyToIf (context->fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kPacketsReceivedPerSecond_), &updateResult->fIOStatistics.fPacketsPerSecondReceived);
                Memory::CopyToIf (context->fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kPacketsSentPerSecond_), &updateResult->fIOStatistics.fPacketsPerSecondSent);

                Memory::AccumulateIf (&updateResult->fIOStatistics.fTCPSegmentsPerSecond, context->fTCPv4WMICollector_.PeekCurrentValue (wmiInstanceName, kTCPSegmentsPerSecond_));
                Memory::AccumulateIf (&updateResult->fIOStatistics.fTCPSegmentsPerSecond, context->fTCPv6WMICollector_.PeekCurrentValue (wmiInstanceName, kTCPSegmentsPerSecond_));

                Memory::AccumulateIf (&updateResult->fIOStatistics.fTCPRetransmittedSegmentsPerSecond, context->fTCPv4WMICollector_.PeekCurrentValue (wmiInstanceName, kSegmentsRetransmittedPerSecond_));
                Memory::AccumulateIf (&updateResult->fIOStatistics.fTCPRetransmittedSegmentsPerSecond, context->fTCPv6WMICollector_.PeekCurrentValue (wmiInstanceName, kSegmentsRetransmittedPerSecond_));
            }
        }
#endif
    };
}
#endif

namespace {
    const MeasurementType kNetworkInterfacesMeasurement_ = MeasurementType{L"Network-Interfaces"sv};
}

namespace {
    struct NetworkInstrumentRep_
#if qPlatform_POSIX
        : InstrumentRep_POSIX_
#elif qPlatform_Windows
        : InstrumentRep_Windows_
#else
        : InstrumentRepBase_<SystemPerformance::Support::Context>
#endif
    {
#if qPlatform_POSIX
        using inherited = InstrumentRep_POSIX_;
#elif qPlatform_Windows
        using inherited = InstrumentRep_Windows_;
#else
        using inherited = InstrumentRepBase_<SystemPerformance::Support::Context>;
#endif
        NetworkInstrumentRep_ (const Options& options, const shared_ptr<_Context>& context = make_shared<_Context> ())
            : inherited{options, context}
        {
            Require (_fOptions.fMinimumAveragingInterval > 0);
        }
        virtual MeasurementSet Capture () override
        {
            Debug::TraceContextBumper ctx{"SystemPerformance::Instrument...Network...NetworkInstrumentRep_::Capture ()"};
            MeasurementSet            results;
            Measurement               m{kNetworkInterfacesMeasurement_, Instruments::Network::Instrument::kObjectVariantMapper.FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            auto before         = _GetCaptureContextTime ();
            Info rawMeasurement = _InternalCapture ();
            if (outMeasuredAt != nullptr) {
                using Traversal::Openness;
                *outMeasuredAt = Range<DurationSecondsType> (before, _GetCaptureContextTime ().value_or (Time::GetTickCount ()), Openness::eClosed, Openness::eClosed);
            }
            return rawMeasurement;
        }
        virtual unique_ptr<IRep> Clone () const override
        {
            return make_unique<NetworkInstrumentRep_> (_fOptions, _fContext.load ());
        }
        Instruments::Network::Info _InternalCapture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::Network::capture_"};
#endif
            return inherited::_InternalCapture ();
        }
    };
}

/*
 ********************************************************************************
 ********************** Instruments::Network::Instrument ************************
 ********************************************************************************
 */
const ObjectVariantMapper Instruments::Network::Instrument::kObjectVariantMapper = [] () -> ObjectVariantMapper {
    using StructFieldInfo = ObjectVariantMapper::StructFieldInfo;
    ObjectVariantMapper mapper;
    mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Interface::Type> ());
    mapper.AddCommonType<optional<Interface::Type>> ();
    mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Interface::Status> ());
    mapper.AddCommonType<Set<Interface::Status>> ();
    mapper.AddCommonType<optional<Set<Interface::Status>>> ();
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
    mapper.AddClass<InterfaceInfo::Interface> (initializer_list<StructFieldInfo>{
        {L"Interface-Internal-ID", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fInternalInterfaceID)},
        {L"Friendly-Name", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fFriendlyName)},
        {L"Description", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fDescription), StructFieldInfo::eOmitNullFields},
        {L"Interface-Type", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fType), StructFieldInfo::eOmitNullFields},
        {L"Hardware-Address", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fHardwareAddress), StructFieldInfo::eOmitNullFields},
        {L"Interface-Status", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fStatus), StructFieldInfo::eOmitNullFields},
        {L"Transmit-Speed-Baud", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fTransmitSpeedBaud), StructFieldInfo::eOmitNullFields},
        {L"Receive-Link-Speed-Baud", Stroika_Foundation_DataExchange_StructFieldMetaInfo (InterfaceInfo::Interface, fReceiveLinkSpeedBaud), StructFieldInfo::eOmitNullFields},
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
    mapper.AddCommonType<optional<Collection<InterfaceInfo>>> ();
    mapper.AddCommonType<optional<IOStatistics>> ();
    mapper.AddClass<Info> (initializer_list<StructFieldInfo>{
        {L"Interfaces", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fInterfaces), StructFieldInfo::eOmitNullFields},
        {L"Summary-IO-Statistics", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fSummaryIOStatistics), StructFieldInfo::eOmitNullFields},
    });
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
    return mapper;
}();

Instruments::Network::Instrument::Instrument (const Options& options)
    : SystemPerformance::Instrument{
          InstrumentNameType{L"Network"sv},
          make_unique<NetworkInstrumentRep_> (options),
          {kNetworkInterfacesMeasurement_},
          {KeyValuePair<type_index, MeasurementType>{typeid (Info), kNetworkInterfacesMeasurement_}},
          kObjectVariantMapper}
{
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::Network::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    Debug::TraceContextBumper ctx{"SystemPerformance::Instrument::CaptureOneMeasurement<Network::Info>"};
    NetworkInstrumentRep_*    myCap = dynamic_cast<NetworkInstrumentRep_*> (fCaptureRep_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
