/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/Streams/BinaryInputStream.h"

#include    "../CommonMeasurementTypes.h"

#include    "NetworkInterfaces.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;


using   Characters::Character;
using   Characters::String_Constant;
using   Containers::Mapping;
using   Containers::Sequence;
using   Containers::Set;
using   IO::FileSystem::BinaryFileInputStream;


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



#if 0
namespace {
    template <typename T>
    void    ReadMemInfoLine_ (Optional<T>* result, const String& n, const Sequence<String>& line)
    {
        if (line.size () >= 3 and line[0] == n) {
            String  unit = line[2];
            double  factor = (unit == L"kB") ? 1024 : 1;
            *result = static_cast<T> (round (Characters::String2Float<double> (line[1]) * factor));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
        }
    }
    template <typename T>
    void    ReadVMStatLine_ (Optional<T>* result, const String& n, const Sequence<String>& line)
    {
        if (line.size () >= 2 and line[0] == n) {
            *result = Characters::String2Int<T> (line[1]);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
        }
    }
    Instruments::Memory::Info capture_ ()
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (SDKSTR ("Instruments::Memory::Info capture_"));
#endif

        constexpr   bool    kManuallyComputePagesPerSecond_ { true };

        Instruments::Memory::Info   result;
#if     qPlatform_POSIX
        {
            DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
            const   String_Constant kProcMemInfoFileName_ { L"/proc/meminfo" };
            //const String_Constant kProcMemInfoFileName_ { L"c:\\Sandbox\\VMSharedFolder\\meminfo" };
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadAs2DArray (BinaryFileInputStream::mk (kProcMemInfoFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                ReadMemInfoLine_ (&result.fFreePhysicalMemory, String_Constant (L"MemFree"), line);
                ReadMemInfoLine_ (&result.fTotalVirtualMemory, String_Constant (L"VmallocTotal"), line);
                ReadMemInfoLine_ (&result.fUsedVirtualMemory, String_Constant (L"VmallocUsed"), line);
                ReadMemInfoLine_ (&result.fLargestAvailableVirtualChunk, String_Constant (L"VmallocChunk"), line);
            }
        }
        {
            DataExchange::CharacterDelimitedLines::Reader reader {{ ' ', '\t' }};
            const   String_Constant kProcVMStatFileName_ { L"/proc/vmstat" };
            Optional<uint64_t>  pgfault;
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadAs2DArray (BinaryFileInputStream::mk (kProcVMStatFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                ReadVMStatLine_ (&pgfault, String_Constant (L"pgfault"), line);
                ReadVMStatLine_ (&result.fMajorPageFaultsSinceBoot, String_Constant (L"pgmajfault"), line);
            }
            if (pgfault.IsPresent () and result.fMajorPageFaultsSinceBoot.IsPresent ()) {
                result.fMinorPageFaultsSinceBoot = *pgfault - *result.fMajorPageFaultsSinceBoot;
            }
        }
#elif   qPlatform_Windows
#endif
        if (kManuallyComputePagesPerSecond_) {
            static  mutex                       s_Mutex_;
            static  uint64_t                    s_Saved_MajorPageFaultsSinceBoot {};
            static  Time::DurationSecondsType   s_Saved_MajorPageFaultsSinceBoot_At {};
            if (result.fMajorPageFaultsSinceBoot.IsPresent ()) {
                Time::DurationSecondsType   now = Time::GetTickCount ();
                auto    critSec { Execution::make_unique_lock (s_Mutex_) };
                if (s_Saved_MajorPageFaultsSinceBoot_At != 0) {
                    result.fMajorPageFaultsPerSecond = (*result.fMajorPageFaultsSinceBoot - s_Saved_MajorPageFaultsSinceBoot) / (now - s_Saved_MajorPageFaultsSinceBoot_At);
                }
                s_Saved_MajorPageFaultsSinceBoot = *result.fMajorPageFaultsSinceBoot;
                s_Saved_MajorPageFaultsSinceBoot_At = now;
            }
        }
        return result;
    }
}
#endif



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
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            //{ Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsPerSecond), String_Constant (L"Minor-Page-Faults-Per-Second") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
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
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"NetworkInterfaces")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Instruments::NetworkInterfaces::Info rawMeasurement;// = capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kNetworkInterfacesMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kNetworkInterfacesMeasurement}
                                          );
    return kInstrument_;
}
