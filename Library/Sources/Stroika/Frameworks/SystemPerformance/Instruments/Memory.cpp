/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/Streams/BinaryInputStream.h"

#include    "../CommonMeasurementTypes.h"

#include    "Memory.h"


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
using   Time::DurationSecondsType;



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
    const   String_Constant     kInstanceName_      { L"_Total" };

    const   String_Constant     kCommittedBytes_    { L"Committed Bytes" };
    const   String_Constant     kCommitLimit_       { L"Commit Limit" };
    const   String_Constant     kPagesPerSec_       { L"Pages/sec" };           // hard page faults/sec
}
#endif






namespace {
    struct  CapturerWithContext_ {
#if     qUseWMICollectionSupport_
        WMICollector        fMemoryWMICollector_ { String_Constant { L"Memory" }, {kInstanceName_},  {kCommittedBytes_, kCommitLimit_, kPagesPerSec_ } };
        DurationSecondsType fMinTimeBeforeFirstCapture;
#endif

        CapturerWithContext_ (DurationSecondsType minTimeBeforeFirstCapture = 1.0)
#if     qUseWMICollectionSupport_
            :   fMinTimeBeforeFirstCapture (minTimeBeforeFirstCapture)
#endif
        {
#if     qUseWMICollectionSupport_
            fMemoryWMICollector_.Collect ();
            Execution::Sleep (minTimeBeforeFirstCapture);
#endif
        }
        CapturerWithContext_ (const CapturerWithContext_& from)
#if     qUseWMICollectionSupport_
            : fMemoryWMICollector_ (from.fMemoryWMICollector_)
            ,   fMinTimeBeforeFirstCapture (from.fMinTimeBeforeFirstCapture)
#endif
        {
#if   qUseWMICollectionSupport_
            fMemoryWMICollector_.Collect ();
            Execution::Sleep (fMinTimeBeforeFirstCapture);
#endif
        }

        Instruments::Memory::Info capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture_");
#endif

            constexpr   bool    kManuallyComputePagesPerSecond_ { true };

            Instruments::Memory::Info   result;
#if     qPlatform_POSIX
            Read_ProcMemInfo (&result);
            Read_ProcVMStat_ (&result);
#elif   qPlatform_Windows
            Read_GlobalMemoryStatusEx_(&result);
#if     qUseWMICollectionSupport_
            Read_WMI_ (&result);
#endif
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

#if     qPlatform_POSIX
        void    Read_ProcMemInfo (Instruments::Memory::Info* updateResult)
        {
            auto    ReadMemInfoLine_  = [] (Optional<uint64_t>* result, const String & n, const Sequence<String>& line) {
                if (line.size () >= 3 and line[0] == n) {
                    String  unit = line[2];
                    double  factor = (unit == L"kB") ? 1024 : 1;
                    *result = static_cast<uint64_t> (round (Characters::String2Float<double> (line[1]) * factor));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                }
            };
            static  const   String_Constant kProcMemInfoFileName_ { L"/proc/meminfo" };
            //const String_Constant kProcMemInfoFileName_ { L"c:\\Sandbox\\VMSharedFolder\\meminfo" };
            DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcMemInfoFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                ReadMemInfoLine_ (&updateResult->fFreePhysicalMemory, String_Constant (L"MemFree"), line);
                ReadMemInfoLine_ (&updateResult->fTotalVirtualMemory, String_Constant (L"VmallocTotal"), line);
                ReadMemInfoLine_ (&updateResult->fUsedVirtualMemory, String_Constant (L"VmallocUsed"), line);
                ReadMemInfoLine_ (&updateResult->fLargestAvailableVirtualChunk, String_Constant (L"VmallocChunk"), line);
            }
        }
        void    Read_ProcVMStat_ (Instruments::Memory::Info* updateResult)
        {
            auto    ReadVMStatLine_ = [] (Optional<uint64_t>* result, const String & n, const Sequence<String>& line) {
                if (line.size () >= 2 and line[0] == n) {
                    *result = Characters::String2Int<uint64_t> (line[1]);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                }
            };
            {
                DataExchange::CharacterDelimitedLines::Reader reader {{ ' ', '\t' }};
                static  const   String_Constant kProcVMStatFileName_ { L"/proc/vmstat" };
                Optional<uint64_t>  pgfault;
                // Note - /procfs files always unseekable
                for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcVMStatFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                    ReadVMStatLine_ (&pgfault, String_Constant (L"pgfault"), line);
                    ReadVMStatLine_ (&updateResult->fMajorPageFaultsSinceBoot, String_Constant (L"pgmajfault"), line);
                }
                if (pgfault.IsPresent () and updateResult->fMajorPageFaultsSinceBoot.IsPresent ()) {
                    updateResult->fMinorPageFaultsSinceBoot = *pgfault - *updateResult->fMajorPageFaultsSinceBoot;
                }
            }
        }
#elif   qPlatform_Windows
        void    Read_GlobalMemoryStatusEx_ (Instruments::Memory::Info* updateResult)
        {
            MEMORYSTATUSEX statex;
            memset (&statex, 0, sizeof (statex));
            statex.dwLength = sizeof (statex);
            Verify (::GlobalMemoryStatusEx (&statex) != 0);
            updateResult->fFreePhysicalMemory = statex.ullAvailPhys;
        }
        void    Read_WMI_ (Instruments::Memory::Info* updateResult)
        {
            fMemoryWMICollector_.Collect ();
            {
                if (auto o = fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommittedBytes_)) {
                    updateResult->fUsedVirtualMemory = *o ;
                }
                if (auto o = fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_)) {
                    // bad names - RETHINK
                    updateResult->fTotalVirtualMemory = *o ;
                }
                if (auto o = fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kPagesPerSec_)) {
                    updateResult->fMajorPageFaultsPerSecond = *o ;
                }
            }
        }
#endif
    };
}



const   MeasurementType Instruments::Memory::kSystemMemoryMeasurement = MeasurementType (String_Constant (L"System-Memory"));




/*
 ********************************************************************************
 ******************** Instruments::Memory::GetObjectVariantMapper ***************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Memory::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fFreePhysicalMemory), String_Constant (L"Free-Physical-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalVirtualMemory), String_Constant (L"Total-Virtual-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fUsedVirtualMemory), String_Constant (L"UsedV-irtual-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fLargestAvailableVirtualChunk), String_Constant (L"Largest-Available-Virtual-Chunk") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMajorPageFaultsSinceBoot), String_Constant (L"Major-Page-Faults-Since-Boot") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsSinceBoot), String_Constant (L"Minor-Page-Faults-Since-Boot") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMajorPageFaultsPerSecond), String_Constant (L"Major-Page-Faults-Per-Second") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsPerSecond), String_Constant (L"Minor-Page-Faults-Per-Second") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}




/*
 ********************************************************************************
 ******************* Instruments::Memory::GetInstrument *************************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::Memory::GetInstrument ()
{
    CapturerWithContext_ useCaptureContext;  // capture context so copyable in mutable lambda
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"Memory")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Instruments::Memory::Info rawMeasurement = useCaptureContext.capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kSystemMemoryMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kMemoryUsage},
    GetObjectVariantMapper ()
                                          );
    return kInstrument_;
}
