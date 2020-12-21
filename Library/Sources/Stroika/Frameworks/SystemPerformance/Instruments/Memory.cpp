/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/Characters/FloatConversion.h"
#include "../../../Foundation/Characters/String2Int.h"
#include "../../../Foundation/Configuration/SystemConfiguration.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"
#include "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../../Foundation/Debug/Assertions.h"
#include "../../../Foundation/Debug/Trace.h"
#include "../../../Foundation/Execution/Exceptions.h"
#include "../../../Foundation/Execution/ProcessRunner.h"
#include "../../../Foundation/Execution/Sleep.h"
#include "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../../Foundation/Streams/InputStream.h"
#include "../../../Foundation/Streams/MemoryStream.h"
#include "../../../Foundation/Streams/TextReader.h"

#include "Memory.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;
using namespace Stroika::Frameworks::SystemPerformance::Instruments::Memory;

using Characters::Character;
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

#if qUseWMICollectionSupport_
#include "../Support/WMICollector.h"

using SystemPerformance::Support::WMICollector;
#endif

#if qUseWMICollectionSupport_
namespace {
    const String kInstanceName_{L"_Total"sv};

    const String kCommittedBytes_{L"Committed Bytes"sv};
    const String kCommitLimit_{L"Commit Limit"sv};
    const String kHardPageFaultsPerSec_{L"Pages/sec"sv};
    const String kPagesOutPerSec_{L"Pages Output/sec"sv};

    const String kFreeMem_{L"Free & Zero Page List Bytes"sv};

    // Something of an empirical WAG (kHardwareReserved*) but not super important to get right -- LGP 2015-09-24
    const String kHardwareReserved1_{L"System Driver Resident Bytes"sv};
    const String kHardwareReserved2_{L"System Driver Total Bytes"sv};
}
#endif

/*
 ********************************************************************************
 ************ Instruments::Memory::Info::PhysicalRAMDetailsType *****************
 ********************************************************************************
 */
String Instruments::Memory::Info::PhysicalRAMDetailsType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
 ********************************************************************************
 ************ Instruments::Memory::Info::VirtualMemoryDetailsType ***************
 ********************************************************************************
 */
String Instruments::Memory::Info::VirtualMemoryDetailsType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
 ********************************************************************************
 **************** Instruments::Memory::Info::PagingDetailsType ******************
 ********************************************************************************
 */
String Instruments::Memory::Info::PagingDetailsType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
 ********************************************************************************
 ********************** Instruments::Memory::Info *******************************
 ********************************************************************************
 */
String Instruments::Memory::Info::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

namespace {
    struct CapturerWithContext_COMMON_ {
        Options             fOptions_;
        DurationSecondsType fMinimumAveragingInterval_;
        DurationSecondsType fPostponeCaptureUntil_{0};
        DurationSecondsType fLastCapturedAt_{};
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_{options}
            , fMinimumAveragingInterval_{options.fMinimumAveragingInterval}
        {
        }
        DurationSecondsType GetLastCaptureAt () const { return fLastCapturedAt_; }
        void                NoteCompletedCapture_ ()
        {
            auto now               = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt_       = now;
        }
    };
}

#if qPlatform_Linux
namespace {
    struct CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
        uint64_t                  fSaved_MajorPageFaultsSinceBoot{};
        uint64_t                  fSaved_MinorPageFaultsSinceBoot{};
        uint64_t                  fSaved_PageOutsSinceBoot{};
        Time::DurationSecondsType fSaved_VMPageStats_At{};

        CapturerWithContext_Linux_ (Options options)
            : CapturerWithContext_COMMON_{options}
        {
            // for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed."); // Don't propagate in case just listing collectors
            }
        }
        CapturerWithContext_Linux_ (const CapturerWithContext_Linux_&) = default; // copy by value fine - no need to re-wait...

        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info result;
            Read_ProcMemInfo (&result);
            try {
                Read_ProcVMStat_ (&result);
            }
            catch (...) {
                DbgTrace (L"Ignoring error in Read_ProcVMStat_: %s", Characters::ToString (current_exception ()).c_str ());
            }
            NoteCompletedCapture_ ();
            return result;
        }
        void Read_ProcMemInfo (Instruments::Memory::Info* updateResult)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Read_ProcMemInfo");
#endif
            auto ReadMemInfoLine_ = [] (optional<uint64_t>* result, const String& n, const Sequence<String>& line) {
                if (line.size () >= 3 and line[0] == n) {
                    String unit   = line[2];
                    double factor = (unit == L"kB") ? 1024 : 1;
                    *result       = Math::Round<uint64_t> (Characters::String2Float<double> (line[1]) * factor);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                }
            };
            /*
             *  @todo   minor performance note: we current do about 10 (tha many strings * 45 (about that many lines in file) compares.
             *          We couuld read data and form a map so lookups faster. Or at least keep a list of items alreayd found and not
             *          look for them more, and stop when none left to look for (wont work if some like sreclaimable not found).
             */
            static const filesystem::path                          kProcMemInfoFileName_{"/proc/meminfo"};
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{':', ' ', '\t'}};
            // Note - /procfs files always unseekable
            optional<uint64_t> memTotal;
            optional<uint64_t> slabReclaimable;
            optional<uint64_t> slab; // older kernels don't have slabReclaimable
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                static const String kMemTotalLabel_{L"MemTotal"sv};
                static const String kMemFreelLabel_{L"MemFree"sv};
                static const String kMemAvailableLabel_{L"MemAvailable"sv};
                static const String kActiveLabel_{L"Active"sv};
                static const String kInactiveLabel_{L"Inactive"sv};
                static const String kCommitLimitLabel_{L"CommitLimit"sv};
                static const String kCommitted_ASLabel_{L"Committed_AS"sv};
                static const String kSwapTotalLabel_{L"SwapTotal"sv};
                static const String kSReclaimableLabel_{L"SReclaimable"sv};
                static const String kSlabLabel_{L"Slab"sv};
                ReadMemInfoLine_ (&memTotal, kMemTotalLabel_, line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fFree, kMemFreelLabel_, line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fAvailable, kMemAvailableLabel_, line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fActive, kActiveLabel_, line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fInactive, kInactiveLabel_, line);
                ReadMemInfoLine_ (&updateResult->fVirtualMemory.fCommitLimit, kCommitLimitLabel_, line);
                /*
                 *  From docs on https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.txt about
                 *  Commited_AS - its unclear if this is the best measure of commited bytes.
                 */
                ReadMemInfoLine_ (&updateResult->fVirtualMemory.fCommittedBytes, kCommitted_ASLabel_, line);
                ReadMemInfoLine_ (&updateResult->fVirtualMemory.fPagefileTotalSize, kSwapTotalLabel_, line);
                ReadMemInfoLine_ (&slabReclaimable, kSReclaimableLabel_, line);
                ReadMemInfoLine_ (&slab, kSlabLabel_, line);
            }
            if (memTotal and updateResult->fPhysicalMemory.fFree and updateResult->fPhysicalMemory.fInactive and updateResult->fPhysicalMemory.fActive) {
                updateResult->fPhysicalMemory.fOSReserved = *memTotal - *updateResult->fPhysicalMemory.fFree - *updateResult->fPhysicalMemory.fInactive - *updateResult->fPhysicalMemory.fActive;
            }
            if (not updateResult->fPhysicalMemory.fAvailable.has_value () and updateResult->fPhysicalMemory.fFree and updateResult->fPhysicalMemory.fInactive) {
                if (not slabReclaimable.has_value ()) {
                    // wag
                    slabReclaimable = ValueOrDefault (slab) / 2;
                }
                updateResult->fPhysicalMemory.fAvailable = *updateResult->fPhysicalMemory.fFree + *updateResult->fPhysicalMemory.fInactive + ValueOrDefault (slabReclaimable);
            }
        }
        void Read_ProcVMStat_ (Instruments::Memory::Info* updateResult)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Read_ProcVMStat_");
#endif
            auto ReadVMStatLine_ = [] (optional<uint64_t>* result, const String& n, const Sequence<String>& line) -> unsigned int {
                if (line.size () >= 2 and line[0] == n) {
                    *result = Characters::String2Int<uint64_t> (line[1]);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                    return 1;
                }
                return 0;
            };
            {
                static const filesystem::path kProcVMStatFileName_{"/proc/vmstat"};
                optional<uint64_t>            pgfault;
                optional<uint64_t>            pgpgout;
                {
                    unsigned int nFound{};
                    // Note - /procfs files always unseekable
                    DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
                    for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcVMStatFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                        static const String kpgfaultLabel_{L"pgfault"sv};
                        static const String kpgpgoutLabel_{L"pgpgout"sv};
                        static const String kpgmajfaultLabel_{L"pgmajfault"sv};
                        nFound += ReadVMStatLine_ (&pgfault, kpgfaultLabel_, line);
                        // Unsure if this should be pgpgout or pgpgout, or none of the above. On a system with no swap, I seem to get both happening,
                        // which makes no sense
                        nFound += ReadVMStatLine_ (&pgpgout, kpgpgoutLabel_, line); // tried pgpgout but I don't know what it is but doesn't appear to be pages out - noneof this well documented
                        nFound += ReadVMStatLine_ (&updateResult->fPaging.fMajorPageFaultsSinceBoot, kpgmajfaultLabel_, line);
                        if (nFound >= 3) {
                            break; // avoid reading rest if all found
                        }
                    }
                }
                Time::DurationSecondsType now            = Time::GetTickCount ();
                updateResult->fPaging.fPageOutsSinceBoot = pgpgout;
                if (pgfault and updateResult->fPaging.fMajorPageFaultsSinceBoot) {
                    updateResult->fPaging.fMinorPageFaultsSinceBoot = *pgfault - *updateResult->fPaging.fMajorPageFaultsSinceBoot;
                }
                auto doAve_ = [] (Time::DurationSecondsType savedVMPageStatsAt, Time::DurationSecondsType now, uint64_t* savedBaseline, optional<uint64_t> faultsSinceBoot, optional<double>* faultsPerSecond) {
                    if (faultsSinceBoot) {
                        if (savedVMPageStatsAt != 0) {
                            *faultsPerSecond = (*faultsSinceBoot - *savedBaseline) / (now - savedVMPageStatsAt);
                        }
                        *savedBaseline = *faultsSinceBoot;
                    }
                };
                doAve_ (fSaved_VMPageStats_At, now, &fSaved_MinorPageFaultsSinceBoot, updateResult->fPaging.fMinorPageFaultsSinceBoot, &updateResult->fPaging.fMinorPageFaultsPerSecond);
                doAve_ (fSaved_VMPageStats_At, now, &fSaved_MajorPageFaultsSinceBoot, updateResult->fPaging.fMajorPageFaultsSinceBoot, &updateResult->fPaging.fMajorPageFaultsPerSecond);
                doAve_ (fSaved_VMPageStats_At, now, &fSaved_PageOutsSinceBoot, updateResult->fPaging.fPageOutsSinceBoot, &updateResult->fPaging.fPageOutsPerSecond);
                fSaved_VMPageStats_At = now;
            }
        }
    };
}
#endif

#if qPlatform_Windows
namespace {
    struct CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if qUseWMICollectionSupport_
        WMICollector fMemoryWMICollector_{L"Memory"sv, {kInstanceName_}, {kCommittedBytes_, kCommitLimit_, kHardPageFaultsPerSec_, kPagesOutPerSec_, kFreeMem_, kHardwareReserved1_, kHardwareReserved2_}};
#endif
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_{options}
        {
            capture_ (); // to pre-seed context
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            for (String i : fMemoryWMICollector_.GetAvailableCounters ()) {
                DbgTrace (L"Memory:Countername: %s", i.c_str ());
            }
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_
        {
            from
        }
#if qUseWMICollectionSupport_
        , fMemoryWMICollector_ (from.fMemoryWMICollector_)
#endif
        {
#if qUseWMICollectionSupport_
            capture_ (); // to pre-seed context
#endif
        }

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info result;
            uint64_t                  totalRAM{};
            Read_GlobalMemoryStatusEx_ (&result, &totalRAM);
#if qUseWMICollectionSupport_
            Read_WMI_ (&result, totalRAM);
#endif
            // I've found no docs to clearly state one way or another, but empirically from looking at the graph in
            // Resource Monitor, the amount reported as 'hardware' - which I'm thinking is roughly 'osreserved' is
            // subtracted from 'standby'.
            if (result.fPhysicalMemory.fOSReserved) {
                Memory::AccumulateIf (&result.fPhysicalMemory.fInactive, result.fPhysicalMemory.fOSReserved, std::minus{});
            }
            NoteCompletedCapture_ ();
            return result;
        }
        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        void Read_GlobalMemoryStatusEx_ (Instruments::Memory::Info* updateResult, uint64_t* totalRAM)
        {
            RequireNotNull (totalRAM);
            MEMORYSTATUSEX statex{};
            statex.dwLength = sizeof (statex);
            Verify (::GlobalMemoryStatusEx (&statex) != 0);
            updateResult->fPhysicalMemory.fFree             = statex.ullAvailPhys; // overridden later, but a good first estimate if we don't use WMI
            *totalRAM                                       = statex.ullTotalPhys;
            updateResult->fVirtualMemory.fPagefileTotalSize = statex.ullTotalPageFile;

            /*
             *  dwMemoryLoad
             *  A number between 0 and 100 that specifies the approximate percentage of physical
             *  memory that is in use (0 indicates no memory use and 100 indicates full memory use)
             */
            updateResult->fPhysicalMemory.fActive = statex.ullTotalPhys * statex.dwMemoryLoad / 100;
        }
#if qUseWMICollectionSupport_
        void Read_WMI_ (Instruments::Memory::Info* updateResult, uint64_t totalRAM)
        {
            fMemoryWMICollector_.Collect ();
            Memory::CopyToIf (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommittedBytes_), &updateResult->fVirtualMemory.fCommittedBytes);
            Memory::CopyToIf (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_), &updateResult->fVirtualMemory.fCommitLimit);
            Memory::CopyToIf (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kHardPageFaultsPerSec_), &updateResult->fPaging.fMajorPageFaultsPerSecond);
            Memory::CopyToIf (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kPagesOutPerSec_), &updateResult->fPaging.fPageOutsPerSecond);
            Memory::CopyToIf (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kFreeMem_), &updateResult->fPhysicalMemory.fFree);
            if (optional<double> freeMem = fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kFreeMem_)) {
                if (updateResult->fPhysicalMemory.fActive) {
                    // Active + Inactive + Free == TotalRAM
                    updateResult->fPhysicalMemory.fInactive = totalRAM - *updateResult->fPhysicalMemory.fActive - static_cast<uint64_t> (*freeMem);
                }
            }
            updateResult->fPhysicalMemory.fOSReserved = nullopt;
            Memory::AccumulateIf (&updateResult->fPhysicalMemory.fOSReserved, fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kHardwareReserved1_));
            Memory::AccumulateIf (&updateResult->fPhysicalMemory.fOSReserved, fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kHardwareReserved2_));
            // fPhysicalMemory.fAvailable WAG TMPHACK - probably should add "hardware in use" memory + private WS of each process + shared memory "WS" - but not easy to compute...
            updateResult->fPhysicalMemory.fAvailable = updateResult->fPhysicalMemory.fFree + updateResult->fPhysicalMemory.fInactive;
        }
#endif
    };
}
#endif

namespace {
    struct CapturerWithContext_
        : Debug::AssertExternallySynchronizedLock
#if qPlatform_Linux
        ,
          CapturerWithContext_Linux_
#elif qPlatform_Windows
        ,
          CapturerWithContext_Windows_
#else
        ,
          CapturerWithContext_COMMON_
#endif
    {
#if qPlatform_Linux
        using inherited = CapturerWithContext_Linux_;
#elif qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#else
        using inherited = CapturerWithContext_COMMON_;
#endif
        CapturerWithContext_ (Options options)
            : inherited{options}
        {
        }
        Instruments::Memory::Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture");
#endif
#if qPlatform_Linux or qPlatform_Windows
            Info result = inherited::capture ();
#else
            Info result;
#endif
            Ensure (ValueOrDefault (result.fPhysicalMemory.fActive) + ValueOrDefault (result.fPhysicalMemory.fInactive) + ValueOrDefault (result.fPhysicalMemory.fFree) + ValueOrDefault (result.fPhysicalMemory.fOSReserved) <= GetSystemConfiguration_Memory ().fTotalPhysicalRAM);
            if (result.fPhysicalMemory.fActive and result.fPhysicalMemory.fInactive and result.fPhysicalMemory.fFree and result.fPhysicalMemory.fOSReserved) {
                Ensure (ValueOrDefault (result.fPhysicalMemory.fActive) + ValueOrDefault (result.fPhysicalMemory.fInactive) + ValueOrDefault (result.fPhysicalMemory.fFree) + ValueOrDefault (result.fPhysicalMemory.fOSReserved) == GetSystemConfiguration_Memory ().fTotalPhysicalRAM);
            }
            return result;
        }
    };
}

/*
 ********************************************************************************
 ****************** Instruments::Memory::GetObjectVariantMapper *****************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Memory::GetObjectVariantMapper ()
{
    using StructFieldInfo                     = ObjectVariantMapper::StructFieldInfo;
    static const ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info::PhysicalRAMDetailsType> (initializer_list<StructFieldInfo>{
            {L"Available", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fAvailable), StructFieldInfo::eOmitNullFields},
            {L"Active", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fActive), StructFieldInfo::eOmitNullFields},
            {L"Inactive", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fInactive), StructFieldInfo::eOmitNullFields},
            {L"Free", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fFree), StructFieldInfo::eOmitNullFields},
            {L"OS-Reserved", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fOSReserved), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddClass<Info::VirtualMemoryDetailsType> (initializer_list<StructFieldInfo>{
            {L"Commit-Limit", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::VirtualMemoryDetailsType, fCommitLimit), StructFieldInfo::eOmitNullFields},
            {L"Committed-Bytes", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::VirtualMemoryDetailsType, fCommittedBytes), StructFieldInfo::eOmitNullFields},
            {L"Pagefile-Total-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::VirtualMemoryDetailsType, fPagefileTotalSize), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddClass<Info::PagingDetailsType> (initializer_list<StructFieldInfo>{
            {L"Major-Faults-Since-Boot", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMajorPageFaultsSinceBoot), StructFieldInfo::eOmitNullFields},
            {L"Minor-Faults-Since-Boot", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMinorPageFaultsSinceBoot), StructFieldInfo::eOmitNullFields},
            {L"Page-Outs-Since-Boot", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fPageOutsSinceBoot), StructFieldInfo::eOmitNullFields},
            {L"Major-Faults-Per-Second", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMajorPageFaultsPerSecond), StructFieldInfo::eOmitNullFields},
            {L"Minor-Faults-Per-Second", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMinorPageFaultsPerSecond), StructFieldInfo::eOmitNullFields},
            {L"Page-Outs-Per-Second", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fPageOutsPerSecond), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddClass<Info> (initializer_list<StructFieldInfo>{
            {L"Physical-Memory", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fPhysicalMemory)},
            {L"Virtual-Memory", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fVirtualMemory)},
            {L"Paging", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fPaging)},
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    }();
    return sMapper_;
}

namespace {
    const MeasurementType kMemoryUsageMeasurement_ = MeasurementType (L"Memory-Usage"sv);
}

namespace {
    class MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;

    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext{ctx}
        {
        }
        virtual MeasurementSet Capture () override
        {
            Debug::TraceContextBumper ctx{"SystemPerformance::Instrument...Memory...MyCapturer_::Capture ()"};
            MeasurementSet            results;
            results.fMeasurements.Add (Measurement{kMemoryUsageMeasurement_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))});
            return results;
        }
        nonvirtual Info Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType before         = fCaptureContext.GetLastCaptureAt ();
            Info                rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.GetLastCaptureAt ());
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
 ******************* Instruments::Memory::GetInstrument *************************
 ********************************************************************************
 */
Instrument SystemPerformance::Instruments::Memory::GetInstrument (Options options)
{
    return Instrument (
        InstrumentNameType{L"Memory"sv},
        Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_{options})),
        {kMemoryUsageMeasurement_},
        GetObjectVariantMapper ());
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::Memory::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    Debug::TraceContextBumper ctx{"SystemPerformance::Instrument::CaptureOneMeasurement<Memory::Info>"};
    MyCapturer_*              myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
