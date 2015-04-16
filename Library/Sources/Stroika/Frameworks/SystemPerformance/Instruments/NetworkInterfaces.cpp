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




// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1




#if     qPlatform_Windows
#pragma comment(lib, "iphlpapi.lib")
#endif




#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ {
        CapturerWithContext_POSIX_ (DurationSecondsType minTimeBeforeFirstCapture = 1.0)
        {
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
                        ii.fInternalInterfaceID = ii.fInterfaceID;  // @todo code cleanup - should have API to do this lookup/compare
                        ii.fTotalBytesRecieved = Characters::String2Int<uint64_t> (line[1]);
                        ii.fTotalBytesSent = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 1]);
                        ii.fTotalPacketsSent = Characters::String2Int<uint64_t> (line[2]);
                        ii.fTotalPacketsRecieved = Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 2]);
                        ii.fTotalErrors = Characters::String2Int<uint64_t> (line[3]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 3]);
                        ii.fTotalPacketsDropped = Characters::String2Int<uint64_t> (line[4]) + Characters::String2Int<uint64_t> (line[kOffset2XMit_ + 4]);
                        result.Add (ii);
                    }
                    else {
                        DbgTrace (L"Line %d bad in file %s", nLine, kProcFileName_.c_str ());
                    }
                }
            }
            return result;
        }
    }
}
#endif







#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ {
        CapturerWithContext_Windows_ (DurationSecondsType minTimeBeforeFirstCapture = 1.0)
        {
        }
        Collection<Instruments::NetworkInterfaces::InterfaceInfo> capture_ ()
        {
            using   Instruments::NetworkInterfaces::InterfaceInfo;
            Collection<Instruments::NetworkInterfaces::InterfaceInfo>   result;

            /*
             *  TODO
             *      @todo xxx
                        Use the functions GetIfTable() or GetIfEntry() :

                        http://msdn.microsoft.com/library/en-us/iphlp/iphlp/getiftable.asp?frame=true
                        http://msdn.microsoft.com/library/en-us/iphlp/iphlp/getifentry.asp?frame=true

                        which will return the MIB_IFROW object for the selected interface :

                        http://msdn.microsoft.com/library/en-us/mib/mib/mib_ifrow.asp

                        Specifically the dwInOctets and dwOutOctets will be important to you : they specify the received and sent number of bytes over that interface.
             */
            Iterable<IO::Network::Interface> networkInterfacs {  IO::Network::GetInterfaces () };
            for (IO::Network::Interface networkInterface : networkInterfacs) {
                MIB_IPSTATS stats {};
                Execution::Platform::Windows::ThrowIfNot_NO_ERROR (::GetIpStatistics (&stats));
                InterfaceInfo   ii;
                ii.fInternalInterfaceID = networkInterface.fInternalInterfaceID;
                // @todo - FIX
                // rough guess and not sure how to break down by interface??? - Maybe use GetInterfaces and see what is up?? But ambiguous...
                ii.fTotalPacketsRecieved = stats.dwInReceives;
                ii.fTotalPacketsSent = stats.dwOutRequests;
                result.Add (ii);
            }
#if     0
            wprintf(L"Default initial TTL: \t\t\t\t\t%u\n", pStats->dwDefaultTTL);

            wprintf(L"Number of received datagrams: \t\t\t\t%u\n", pStats->dwInReceives);
            wprintf(L"Number of received datagrams with header errors: \t%u\n", pStats->dwInHdrErrors);
            wprintf(L"Number of received datagrams with address errors: \t%u\n", pStats->dwInAddrErrors);

            wprintf(L"Number of datagrams forwarded: \t\t\t\t%ld\n", pStats->dwForwDatagrams);

            wprintf(L"Number of received datagrams with an unknown protocol: \t%u\n", pStats->dwInUnknownProtos);
            wprintf(L"Number of received datagrams discarded: \t\t%u\n", pStats->dwInDiscards);
            wprintf(L"Number of received datagrams delivered: \t\t%u\n", pStats->dwInDelivers);

            wprintf(L"Number of outgoing datagrams requested to transmit: \t%u\n", pStats->dwOutRequests);
            wprintf(L"Number of outgoing datagrams discarded for routing: \t%u\n", pStats->dwRoutingDiscards);
            wprintf(L"Number of outgoing datagrams discarded: \t\t%u\n", pStats->dwOutDiscards);
            wprintf(L"Number of outgoing datagrams with no route to destination discarded: %u\n", pStats->dwOutNoRoutes);

            wprintf(L"Fragment reassembly timeout: \t\t\t\t%u\n", pStats->dwReasmTimeout);
            wprintf(L"Number of datagrams that required reassembly: \t\t%u\n", pStats->dwReasmReqds);
            wprintf(L"Number of datagrams successfully reassembled: \t\t%u\n", pStats->dwReasmOks);
            wprintf(L"Number of datagrams that could not be reassembled: \t%u\n", pStats->dwReasmFails);

            wprintf(L"Number of datagrams fragmented successfully: \t\t%u\n", pStats->dwFragOks);
            wprintf(L"Number of datagrams not fragmented and discarded: \t%u\n", pStats->dwFragFails);
            wprintf(L"Number of fragments created: \t\t\t\t%u\n", pStats->dwFragCreates);

            wprintf(L"Number of interfaces: \t\t\t\t\t%u\n", pStats->dwNumIf);
            wprintf(L"Number of IP addresses: \t\t\t\t%u\n", pStats->dwNumAddr);
            wprintf(L"Number of routes: \t\t\t\t\t%u\n", pStats->dwNumRoutes);
#endif
            return result;
        }
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
        CapturerWithContext_ (DurationSecondsType minTimeBeforeFirstCapture = 1.0)
            : inherited (minTimeBeforeFirstCapture)
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
        mapper.AddCommonType<Optional<String>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<InterfaceInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInternalInterfaceID), String_Constant (L"Interface-Internal-ID") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fInterfaceID), String_Constant (L"Interface-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalBytesSent), String_Constant (L"Total-Bytes-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalBytesRecieved), String_Constant (L"Total-Bytes-Recieved"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalPacketsSent), String_Constant (L"Total-Packets-Sent"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (InterfaceInfo, fTotalPacketsRecieved), String_Constant (L"Total-Packets-Recieved"), StructureFieldInfo::NullFieldHandling::eOmit },
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
Instrument  SystemPerformance::Instruments::NetworkInterfaces::GetInstrument ()
{
    CapturerWithContext_ useCaptureContext;  // capture context so copyable in mutable lambda
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
