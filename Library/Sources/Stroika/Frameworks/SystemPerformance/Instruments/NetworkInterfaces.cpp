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

#include    "../CommonMeasurementTypes.h"

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

    const   String_Constant     kSegmentsRetransmittedPerSecond_    { L"Segments Retransmitted/sec" };
}
#endif



#if     qPlatform_Windows
#pragma comment(lib, "iphlpapi.lib")
#endif




#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ {

        struct Last {
            uint64_t  fTotalBytesReceived;
            uint64_t  fTotalBytesSent;
            uint64_t  fTotalPacketsReceived;
            uint64_t  fTotalPacketsSent;
            DurationSecondsType  fAt;
        };
        Mapping<String, Last>    fLast;
        CapturerWithContext_POSIX_ (Options options)
        {
            if (options.fMinimumAveragingInterval > 0) {
                capture_ ();    // hack for side-effect of  updating aved_MajorPageFaultsSinc etc
                Execution::Sleep (options.fMinimumAveragingInterval);
            }
        }
        Collection<Instruments::NetworkInterfaces::InterfaceInfo> capture_ ()
        {
            using   Instruments::NetworkInterfaces::InterfaceInfo;
            Collection<Instruments::NetworkInterfaces::InterfaceInfo>   result;
            {
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
                        ii.fInternalInterfaceID = line[0];  // @todo code cleanup - should have API to do this lookup/compare
                        ii.fDisplayName = line[0];
                        ii.fTotalBytesReceived = Characters::String2Int<uint64_t> (line[1]);
                        ii.fTotalBytesSent = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 1]);
                        ii.fTotalPacketsReceived = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 2]);
                        ii.fTotalPacketsSent = Characters::String2Int<uint64_t> (line[2]);
                        ii.fTotalErrors = Characters::String2Int<uint64_t> (line[3]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 3]);
                        ii.fTotalPacketsDropped = Characters::String2Int<uint64_t> (line[4]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 4]);

                        DurationSecondsType now = Time::GetTickCount ();
                        if (auto o = fLast.Lookup (ii.fInternalInterfaceID)) {
                            double scanTime = now - o->fAt;
                            if (scanTime > 0) {
                                ii.fBytesPerSecondReceived = (*ii.fTotalBytesReceived - o->fTotalBytesReceived) / scanTime;
                                ii.fBytesPerSecondSent = (*ii.fTotalBytesSent - o->fTotalBytesSent) / scanTime;
                                ii.fPacketsPerSecondReceived = (*ii.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                                ii.fPacketsPerSecondSent = (*ii.fTotalPacketsReceived - o->fTotalPacketsReceived) / scanTime;
                            }
                        }

                        result.Add (ii);
                        fLast.Add (ii.fInternalInterfaceID, Last { *ii.fTotalBytesReceived, *ii.fTotalBytesSent, *ii.fTotalPacketsReceived, *ii.fTotalPacketsSent, now });
                    }
                    else {
                        DbgTrace (L"Line %d bad in file %s", nLine, kProcFileName_.c_str ());
                    }
                }
            }
            return result;
        }
    };
}
#endif






#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ {
#if     qUseWMICollectionSupport_
        WMICollector        fNetworkWMICollector_ { String_Constant { L"Network Interface" }, {},  { kBytesReceivedPerSecond_, kBytesSentPerSecond_, kPacketsReceivedPerSecond_, kPacketsSentPerSecond_ } };
        WMICollector        fTCPv4WMICollector_ { String_Constant { L"TCPv4" }, {},  { kSegmentsRetransmittedPerSecond_ } };
        WMICollector        fTCPv6WMICollector_ { String_Constant { L"TCPv6" }, {},  { kSegmentsRetransmittedPerSecond_ } };
        DurationSecondsType fMinTimeBeforeFirstCapture;
        Set<String>         fAvailableInstances_;
#endif
        CapturerWithContext_Windows_ (Options options)
#if     qUseWMICollectionSupport_
            : fMinTimeBeforeFirstCapture (options.fMinimumAveragingInterval)
#endif
        {
#if     qUseWMICollectionSupport_
            fAvailableInstances_ = fNetworkWMICollector_.GetAvailableInstaces ();
            capture_ ();    // for the side-effect of filling in fNetworkWMICollector_ with interfaces and doing initial capture so WMI can compute averages
            Execution::Sleep (options.fMinimumAveragingInterval);
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
#if     qUseWMICollectionSupport_
            : fNetworkWMICollector_ (from.fNetworkWMICollector_)
            , fTCPv4WMICollector_ (from.fTCPv4WMICollector_)
            , fTCPv6WMICollector_ (from.fTCPv6WMICollector_)
            , fMinTimeBeforeFirstCapture (from.fMinTimeBeforeFirstCapture)
            , fAvailableInstances_ (from.fAvailableInstances_)
#endif
        {
#if   qUseWMICollectionSupport_
            capture_ ();    // for the side-effect of filling in fNetworkWMICollector_ with interfaces and doing initial capture so WMI can compute averages
            Execution::Sleep (fMinTimeBeforeFirstCapture);
#endif
        }
        Collection<Instruments::NetworkInterfaces::InterfaceInfo> capture_ ()
        {
            using   IO::Network::Interface;
            using   Instruments::NetworkInterfaces::InterfaceInfo;

            Collection<InterfaceInfo>   result;
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
#if 0
                    MIB_IPSTATS stats {};
                    Execution::Platform::Windows::ThrowIfNot_NO_ERROR (::GetIpStatistics (&stats));
                    //ii.fTotalPacketsReceived = stats.dwInReceives;
                    //ii.fTotalPacketsSent = stats.dwOutRequests;
#endif
#if     qUseWMICollectionSupport_
                    Read_WMI_ (networkInterface, &ii);
#endif
                    result.Add (ii);
                }
            }
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
                if (auto o = fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kBytesReceivedPerSecond_)) {
                    updateResult->fBytesPerSecondReceived = *o;
                }
                if (auto o = fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kBytesSentPerSecond_)) {
                    updateResult->fBytesPerSecondSent = *o;
                }
                if (auto o = fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kPacketsReceivedPerSecond_)) {
                    updateResult->fPacketsPerSecondReceived = *o;
                }
                if (auto o = fNetworkWMICollector_.PeekCurrentValue (wmiInstanceName, kPacketsSentPerSecond_)) {
                    updateResult->fPacketsPerSecondSent = *o ;
                }
                if (auto o = fTCPv4WMICollector_.PeekCurrentValue (wmiInstanceName, kSegmentsRetransmittedPerSecond_)) {
                    updateResult->fTCPRetransmittedSegmentsPerSecond = *o ;
                }
                if (auto o = fTCPv6WMICollector_.PeekCurrentValue (wmiInstanceName, kSegmentsRetransmittedPerSecond_)) {
                    updateResult->fTCPRetransmittedSegmentsPerSecond = updateResult->fTCPRetransmittedSegmentsPerSecond.Value () + *o ;
                }
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
        Collection<Instruments::NetworkInterfaces::InterfaceInfo> capture_ ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::NetworkInterfaces::capture_");
#endif
            return inherited::capture_ ();
        }
    };
}





const   MeasurementType Instruments::NetworkInterfaces::kNetworkInterfacesMeasurement = MeasurementType (String_Constant (L"Network-Interfaces"));




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
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<InterfaceInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInternalInterfaceID), String_Constant (L"Interface-Internal-ID") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fDisplayName), String_Constant (L"Display-Name") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInterfaceID), String_Constant (L"Interface-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalBytesSent), String_Constant (L"Total-Bytes-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalBytesReceived), String_Constant (L"Total-Bytes-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fBytesPerSecondSent), String_Constant (L"Bytes-Per-Second-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fBytesPerSecondReceived), String_Constant (L"Bytes-Per-Second-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTCPRetransmittedSegmentsPerSecond), String_Constant (L"TCP-Retransmitted-Segments-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalPacketsSent), String_Constant (L"Total-Packets-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalPacketsReceived), String_Constant (L"Total-Packets-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fPacketsPerSecondSent), String_Constant (L"Packets-Per-Second-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fPacketsPerSecondReceived), String_Constant (L"Packets-Per-Second-Received"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalErrors), String_Constant (L"Total-Errors"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalPacketsDropped), String_Constant (L"Total-Packets-Dropped"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<Collection<InterfaceInfo>> ();
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
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"NetworkInterfaces")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Collection<Instruments::NetworkInterfaces::InterfaceInfo> rawMeasurement = useCaptureContext.capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kNetworkInterfacesMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kNetworkInterfacesMeasurement},
    GetObjectVariantMapper ()
                                          );
    return kInstrument_;
}
