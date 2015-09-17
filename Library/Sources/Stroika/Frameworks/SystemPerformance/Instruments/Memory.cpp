/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_AIX
#include    <libperfstat.h>
#endif

#include    "../../../Foundation/Characters/FloatConversion.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Configuration/SystemConfiguration.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Execution/ErrNoException.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include    "../../../Foundation/Streams/InputStream.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/TextReader.h"

#include    "Memory.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::Memory;


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
    struct  CapturerWithContext_COMMON_ {
        Options                 fOptions_;
        DurationSecondsType     fMinimumAveragingInterval_;
        DurationSecondsType     fPostponeCaptureUntil_ { 0 };
        DurationSecondsType     fLastCapturedAt_ {};
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DurationSecondsType    GetLastCaptureAt () const { return fLastCapturedAt_; }
        void    NoteCompletedCapture_ ()
        {
            auto now = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt_ = now;
        }
    };
}







#if     qPlatform_AIX
namespace {
    struct  CapturerWithContext_AIX_ : CapturerWithContext_COMMON_ {
        uint64_t                    fSaved_MinorPageFaultsSinceBoot {};
        Time::DurationSecondsType   fSaved_MinorPageFaultsSinceBoot_At {};
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        Time::DurationSecondsType   fSaved_MajorPageFaultsSinceBoot_At {};

        CapturerWithContext_AIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_AIX_ (const CapturerWithContext_AIX_&) = default;   // copy by value fine - no need to re-wait...

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
            result = capture_perfstat_ ();
            NoteCompletedCapture_ ();
            return result;
        }
        Instruments::Memory::Info capture_perfstat_ ()
        {
            Instruments::Memory::Info   result;

            perfstat_memory_total_t memResults;
            Execution::ThrowErrNoIfNegative (::perfstat_memory_total (nullptr, &memResults,  sizeof (memResults), 1));

            // From /usr/include/libperfstat.h:
            //      u_longlong_t real_free;     /* free real memory (in 4KB pages) */
            result.fFreePhysicalMemory = memResults.real_free * 4 * 1024;

            // From /usr/include/libperfstat.h:
            //      u_longlong_t virt_total;    /* total virtual memory (in 4KB pages)
            result.fCommitLimit = memResults.virt_total * 4 * 1024;             //

            // WAG  /* reserved paging space (in 4KB pages) */
            // u_longlong_t real_inuse;    /* real memory which is in use (in 4KB pages) */

            // From /usr/include/libperfstat.h: u_longlong_t pgsp_total;    /* total paging space (in 4KB pages) */
            result.fPagefileTotalSize  = memResults.pgsp_total * 4 * 1024;

            // From /usr/include/libperfstat.h:
            //      u_longlong_t pgsp_total;    /* total paging space (in 4KB pages) */
            //      u_longlong_t real_inuse;    /* real memory which is in use (in 4KB pages) */
            //      u_longlong_t pgsp_free;     /* free paging space (in 4KB pages) */
            result.fCommittedBytes = (memResults.real_inuse + (memResults.pgsp_total - memResults.pgsp_free)) * 4 * 1024;

            result.fMinorPageFaultsSinceBoot = memResults.pgexct;
            result.fMajorPageFaultsSinceBoot = memResults.pgins + memResults.pgouts;

            if (result.fMinorPageFaultsSinceBoot.IsPresent ()) {
                Time::DurationSecondsType   now = Time::GetTickCount ();
                if (fSaved_MinorPageFaultsSinceBoot_At != 0) {
                    result.fMinorPageFaultsPerSecond = (*result.fMinorPageFaultsSinceBoot - fSaved_MinorPageFaultsSinceBoot) / (now - fSaved_MinorPageFaultsSinceBoot_At);
                }
                fSaved_MinorPageFaultsSinceBoot = *result.fMinorPageFaultsSinceBoot;
                fSaved_MinorPageFaultsSinceBoot_At = now;
            }
            if (result.fMajorPageFaultsSinceBoot.IsPresent ()) {
                Time::DurationSecondsType   now = Time::GetTickCount ();
                if (fSaved_MajorPageFaultsSinceBoot_At != 0) {
                    result.fMajorPageFaultsPerSecond = (*result.fMajorPageFaultsSinceBoot - fSaved_MajorPageFaultsSinceBoot) / (now - fSaved_MajorPageFaultsSinceBoot_At);
                }
                fSaved_MajorPageFaultsSinceBoot = *result.fMajorPageFaultsSinceBoot;
                fSaved_MajorPageFaultsSinceBoot_At = now;
            }

            return result;
        }
        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
    };
}
#endif






#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ : CapturerWithContext_COMMON_ {
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        Time::DurationSecondsType   fSaved_MajorPageFaultsSinceBoot_At {};

        CapturerWithContext_POSIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_&) = default;   // copy by value fine - no need to re-wait...

        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
            Read_ProcMemInfo (&result);
            Read_ProcVMStat_ (&result);
            NoteCompletedCapture_ ();
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
            Optional<uint64_t>  SwapCached;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                ReadMemInfoLine_ (&updateResult->fFreePhysicalMemory, String_Constant (L"MemFree"), line);
                ReadMemInfoLine_ (&updateResult->fCommitLimit, String_Constant (L"CommitLimit"), line);
                ReadMemInfoLine_ (&updateResult->fCommittedBytes, String_Constant (L"Committed_AS"), line);
                ReadMemInfoLine_ (&updateResult->fLargestAvailableVirtualChunk, String_Constant (L"VmallocChunk"), line);
                ReadMemInfoLine_ (&updateResult->fPagefileTotalSize, String_Constant (L"SwapTotal"), line);
                ReadMemInfoLine_ (&SwapCached, String_Constant (L"SwapCached"), line);
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
                for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcVMStatFileName_, FileInputStream::eNotSeekable))) {
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
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if     qUseWMICollectionSupport_
        WMICollector            fMemoryWMICollector_ { String_Constant { L"Memory" }, {kInstanceName_},  {kCommittedBytes_, kCommitLimit_, kPagesPerSec_ } };
#endif
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture_ ();    // to pre-seed context
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if     qUseWMICollectionSupport_
            , fMemoryWMICollector_ (from.fMemoryWMICollector_)
#endif
        {
#if   qUseWMICollectionSupport_
            capture_ ();    // to pre-seed context
#endif
        }

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
            Read_GlobalMemoryStatusEx_(&result);
#if     qUseWMICollectionSupport_
            Read_WMI_ (&result);
#endif
            NoteCompletedCapture_ ();
            return result;
        }
        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
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
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommittedBytes_).CopyToIf (&updateResult->fCommittedBytes);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_).CopyToIf (&updateResult->fCommitLimit);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kPagesPerSec_).CopyToIf (&updateResult->fMajorPageFaultsPerSecond);
#if 0
            updateResult->fTotalVMInUse = updateResult->fCommittedBytes;
            updateResult->fTotalPagefileBackedVirtualMemory = updateResult->fCommitLimit;
#endif
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
        Instruments::Memory::Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture");
#endif
            return inherited::capture ();
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
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fFreePhysicalMemory), String_Constant (L"Free-Physical-Memory"), StructureFieldInfo::NullFieldHandling::eOmit },
            //{ Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalVirtualMemory), String_Constant (L"Total-Virtual-Memory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fCommitLimit), String_Constant (L"Commit-Limit"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fCommittedBytes), String_Constant (L"Committed-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fPagefileTotalSize), String_Constant (L"Pagefile-Total-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
//            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalVMInUse), String_Constant (L"Total-Virtual-Memory-In-Use"), StructureFieldInfo::NullFieldHandling::eOmit },
//            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalPagefileBackedVirtualMemory), String_Constant (L"Total-Pagefile-Backed-Virtual-Memory"), StructureFieldInfo::NullFieldHandling::eOmit },
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


namespace {
    const   MeasurementType kMemoryUsageMeasurement_         =   MeasurementType (String_Constant (L"Memory-Usage"));
}




namespace {
    class   MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;
    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet  Capture ()
        {
            MeasurementSet  results;
            results.fMeasurements.Add (Measurement { kMemoryUsageMeasurement_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))});
            return results;
        }
        nonvirtual Info  Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType before = fCaptureContext.GetLastCaptureAt ();
            Info                rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.GetLastCaptureAt ());
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
 ******************* Instruments::Memory::GetInstrument *************************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::Memory::GetInstrument (Options options)
{
    return Instrument (
               InstrumentNameType  { String_Constant (L"Memory") },
#if     qCompilerAndStdLib_make_unique_Buggy
               Instrument::SharedByValueCaptureRepType (unique_ptr<MyCapturer_> (new MyCapturer_ (CapturerWithContext_ { options }))),
#else
               Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_ { options })),
#endif
    { kMemoryUsageMeasurement_ },
    GetObjectVariantMapper ()
           );
}







/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template    <>
Instruments::Memory::Info   SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

