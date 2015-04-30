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
#include    "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/Streams/BinaryInputStream.h"

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

using   Stroika::Frameworks::SystemPerformance::Instruments::Memory::Options;


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








#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ {
        DurationSecondsType			fMinTimeBeforeFirstCapture_;
        DurationSecondsType			fPostponeCaptureUntil_ { 0 };
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        Time::DurationSecondsType   fSaved_MajorPageFaultsSinceBoot_At {};

        CapturerWithContext_POSIX_ (Options options)
        {
            capture_ ();    // for side-effect of  updating aved_MajorPageFaultsSinc etc
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_&) = default;   // copy by value fine - no need to re-wait...

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
			Execution::SleepUntil (fPostponeCaptureUntil_);
            Read_ProcMemInfo (&result);
            Read_ProcVMStat_ (&result);
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinTimeBeforeFirstCapture_;
            return result;
        }

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
                ReadMemInfoLine_ (&updateResult->fCommitLimit, String_Constant (L"CommitLimit"), line);
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
                static  const   String_Constant kProcVMStatFileName_ { L"/proc/vmstat" };
                Optional<uint64_t>  pgfault;
                // Note - /procfs files always unseekable
                DataExchange::CharacterDelimitedLines::Reader reader {{ ' ', '\t' }};
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
                if (updateResult->fMajorPageFaultsSinceBoot.IsPresent ()) {
                    Time::DurationSecondsType   now = Time::GetTickCount ();
                    if (fSaved_MajorPageFaultsSinceBoot_At != 0) {
                        updateResult->fMajorPageFaultsPerSecond = (*updateResult->fMajorPageFaultsSinceBoot - fSaved_MajorPageFaultsSinceBoot) / (now - fSaved_MajorPageFaultsSinceBoot_At);
                    }
                    fSaved_MajorPageFaultsSinceBoot = *updateResult->fMajorPageFaultsSinceBoot;
                    fSaved_MajorPageFaultsSinceBoot_At = now;
                }
            }
        }
    };
}
#endif




#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ {
        DurationSecondsType		fMinTimeBeforeFirstCapture_;
        DurationSecondsType     fPostponeCaptureUntil_ { 0 };
#if     qUseWMICollectionSupport_
        WMICollector			fMemoryWMICollector_ { String_Constant { L"Memory" }, {kInstanceName_},  {kCommittedBytes_, kCommitLimit_, kPagesPerSec_ } };
#endif

        CapturerWithContext_Windows_ (Options options)
            : fMinTimeBeforeFirstCapture_ (options.fMinimumAveragingInterval)
        {
			capture_ ();	// to pre-seed context
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : fMinTimeBeforeFirstCapture_ (from.fMinTimeBeforeFirstCapture_)
#if     qUseWMICollectionSupport_
            , fMemoryWMICollector_ (from.fMemoryWMICollector_)
#endif
        {
#if   qUseWMICollectionSupport_
			capture_ ();	// to pre-seed context
#endif
        }

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
			Execution::SleepUntil (fPostponeCaptureUntil_);
            Read_GlobalMemoryStatusEx_(&result);
#if     qUseWMICollectionSupport_
            Read_WMI_ (&result);
#endif
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinTimeBeforeFirstCapture_;
            return result;
        }
        void    Read_GlobalMemoryStatusEx_ (Instruments::Memory::Info* updateResult)
        {
            MEMORYSTATUSEX statex;
            memset (&statex, 0, sizeof (statex));
            statex.dwLength = sizeof (statex);
            Verify (::GlobalMemoryStatusEx (&statex) != 0);
            updateResult->fFreePhysicalMemory = statex.ullAvailPhys;
        }
#if     qUseWMICollectionSupport_
        void    Read_WMI_ (Instruments::Memory::Info* updateResult)
        {
            fMemoryWMICollector_.Collect ();
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommittedBytes_).CopyToIf (&updateResult->fUsedVirtualMemory);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_).CopyToIf (&updateResult->fCommitLimit);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_).CopyToIf (&updateResult->fTotalVirtualMemory);        // bad names - RETHINK
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kPagesPerSec_).CopyToIf (&updateResult->fMajorPageFaultsPerSecond);
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
        Instruments::Memory::Info capture_ ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture_");
#endif
            return inherited::capture_ ();
        }
    };
}






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
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fFreePhysicalMemory), String_Constant (L"Free-Physical-Memory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalVirtualMemory), String_Constant (L"Total-Virtual-Memory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fCommitLimit), String_Constant (L"Commit-Limit"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fUsedVirtualMemory), String_Constant (L"Used-Virtual-Memory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fLargestAvailableVirtualChunk), String_Constant (L"Largest-Available-Virtual-Chunk"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMajorPageFaultsSinceBoot), String_Constant (L"Major-Page-Faults-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsSinceBoot), String_Constant (L"Minor-Page-Faults-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMajorPageFaultsPerSecond), String_Constant (L"Major-Page-Faults-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsPerSecond), String_Constant (L"Minor-Page-Faults-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
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
Instrument  SystemPerformance::Instruments::Memory::GetInstrument (Options options)
{
    static  const   MeasurementType kMemoryUsage_         =   MeasurementType (String_Constant (L"Memory-Usage"));
    static  const   MeasurementType kSystemMemoryMeasurement_ = MeasurementType (String_Constant (L"System-Memory"));

    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return Instrument (
               InstrumentNameType (String_Constant (L"Memory")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Instruments::Memory::Info rawMeasurement = useCaptureContext.capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kSystemMemoryMeasurement_;
        results.fMeasurements.Add (m);
        return results;
    },
    {kMemoryUsage_},
    GetObjectVariantMapper ()
           );
}
