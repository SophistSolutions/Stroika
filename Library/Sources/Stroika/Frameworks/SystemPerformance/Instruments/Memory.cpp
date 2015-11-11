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
    const   String_Constant     kInstanceName_          { L"_Total" };

    const   String_Constant     kCommittedBytes_        { L"Committed Bytes" };
    const   String_Constant     kCommitLimit_           { L"Commit Limit" };
    const   String_Constant     kHardPageFaultsPerSec_  { L"Pages/sec" };
    const   String_Constant     kPagesOutPerSec_        { L"Pages Output/sec" };

    const   String_Constant     kFreeMem_               { L"Free & Zero Page List Bytes" };

    // Something of an empirical WAG (kHardwareReserved*) but not super important to get right -- LGP 2015-09-24
    const   String_Constant     kHardwareReserved1_     { L"System Driver Resident Bytes" };
    const   String_Constant     kHardwareReserved2_     { L"System Driver Total Bytes" };

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
        Time::DurationSecondsType   fSaved_VMPageStats_At {};
        uint64_t                    fSaved_MinorPageFaultsSinceBoot {};
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        uint64_t                    fSaved_PageOutsSinceBoot {};

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
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("capture_perfstat_");
#endif
            Instruments::Memory::Info   result;

            perfstat_memory_total_t memResults;
            Execution::ThrowErrNoIfNegative (::perfstat_memory_total (nullptr, &memResults,  sizeof (memResults), 1));

            /*
             * From /usr/include/libperfstat.h:
             *      u_longlong_t real_free;     /* free real memory (in 4KB pages)
             *      u_longlong_t real_avail     - number of pages (in 4KB pages) of memory available without paging out working segments
             *      u_longlong_t real_inuse;    * real memory which is in use (in 4KB pages)
             *      u_longlong_t pgsp_total;    * total paging space (in 4KB pages)
             *      u_longlong_t pgsp_free;     * free paging space (in 4KB pages)
             *      u_longlong_t virt_active;   Active virtual pages. Virtual pages are considered active if they have been accessed
             *      u_longlong_t virt_total;    * total virtual memory (in 4KB pages)
             *
             *      u_longlong_t pgins;         number of pages paged in
             *      u_longlong_t pgouts;        number of pages paged out
             *      u_longlong_t pgspins;       number of page ins from paging space
             *      u_longlong_t pgspouts;      number of page outs from paging space
             *
             *  Empirically, and logically from the (vague) definitions (perfstat_memory_total_t), real_total  = real_inuse + real_free
             *
             *  Some assorted unused stats from
             *          /usr/include/libperfstat.h:
             *
             *      u_longlong_t real_system;    number of pages used by system segments.
             *                                  * This is the sum of all the used pages in segment marked for system usage.
             *                                  * Since segment classifications are not always guaranteed to be accurate,
             *                                  * This number is only an approximation.
             *      u_longlong_t real_user;         * number of pages used by non-system segments.
             *                                  * This is the sum of all pages used in segments not marked for system usage.
             *                                  * Since segment classifications are not always guaranteed to be accurate,
             *                                  * This number is only an approximation.
             *      u_longlong_t real_process;  * number of pages used by process segments.
             *      u_longlong_t scans;         * number of page scans by clock
             *      u_longlong_t cycles;        * number of page replacement cycles
             *      u_longlong_t pgsteals;      * number of page steals
             *      u_longlong_t numperm;       * number of frames used for files (in 4KB pages)
             */
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            // [---MAIN---][0000.007]       real_total=983040
            // [---MAIN---][0000.007]       real_free=280802
            // [---MAIN---][0000.007]       real_inuse=702238
            // [---MAIN---][0000.007]       real_pinned=237141      *not used* see kIncludeUnPinnedActiveRAMAsAvailable_
            // [---MAIN---][0000.007]       real_avail=606596
            // [---MAIN---][0000.007]       real_user=472485        *not used*
            // [---MAIN---][0000.007]       real_system=195546      *not used*
            // [---MAIN---][0000.007]       real_process=148875     *not used*
            // [---MAIN---][0000.007]       virt_active=332889
            // [---MAIN---][0000.007]       virt_total=1998848
            DbgTrace ("real_total=%lld", memResults.real_total);
            DbgTrace ("real_free=%lld", memResults.real_free);
            DbgTrace ("real_inuse=%lld", memResults.real_inuse);
            DbgTrace ("real_pinned=%lld", memResults.real_pinned);
            DbgTrace ("real_avail=%lld", memResults.real_avail);
            DbgTrace ("real_user=%lld", memResults.real_user);
            DbgTrace ("real_system=%lld", memResults.real_system);
            DbgTrace ("real_process=%lld", memResults.real_process);
            DbgTrace ("virt_active=%lld", memResults.virt_active);
            DbgTrace ("virt_total=%lld", memResults.virt_total);
#endif

            result.fPhysicalMemory.fFree = memResults.real_free * 4 * 1024;

            /*
             *  Pinned pages cannot be paged out.
             *      https://www-01.ibm.com/support/knowledgecenter/ssw_aix_71/com.ibm.aix.performance/support_pinned_mem.htm
             *          "Pinning a memory region prohibits the pager from stealing pages from the pages backing the pinned memory region"
             *
             *  What we want to call active is really LARGER than this, but this is at least an estimate of actively in use memory.
             *  (FOR NOW IGNORE ABOVE BUT LATER WE MAY WANT TO FACTOR 'pinned' in - maybe assuring activePhysRam at least as much
             *  as pinned?)
             *
             *  SINCE we know:
             *      real_total  = real_inuse + real_free;
             *      since we define RAMTOTAL = INACTIVE + ACTIVE + FREE  + OS_RESERVED
             *  setting real_inuse + real_free = INACTIVE + ACTIVE + FREE + OS_RESERVED =>
             *      ACTIVE + INACTIVE + OS_RESERVED = real_inuse;
             *
             *  It APPEARS (empirically looking at results) that
             *      real_avail = real_free +  INACTIVE
             *
             *  SO:
             *      INACIVE = is real_avail - real_free
             *      ACTIVE= (real_inuse - (real_avail-real_free) - OS_RESERVED);
             */
            if (memResults.real_avail < memResults.real_free) {
                // This is crazy, and makes no sense, but happens sometimes on AIX 7.1. Generously, lets assume its a race, and that
                // free memory is updated separately from when available memory is updated. Otherwise, it really makes no
                // sense.
                //
                // Since i have no idea which one is wrong, I'm going to split the difference, and adjust both to be the same
                DbgTrace ("Very strange: memResults.real_avail (%lld) < memResults.real_free (%lld)", memResults.real_avail, memResults.real_free);
                u_longlong_t    tmp = (memResults.real_avail + memResults.real_free) / 2;
                memResults.real_avail = tmp;
                memResults.real_free = tmp;
            }
            result.fPhysicalMemory.fOSReserved = static_cast<uint64_t> (0); // since we cannot find - it would be subtracted from active or inactive if we had something here
            result.fPhysicalMemory.fInactive = (memResults.real_avail - memResults.real_free) * 4 * 1024;
            result.fPhysicalMemory.fActive = (memResults.real_inuse - memResults.real_avail + memResults.real_free) * 4 * 1024 - *result.fPhysicalMemory.fOSReserved;

            // Check for bad that that just cannot happen (I think would be kernel/libperf bug)
            // Not 100% sure if total RAM can change while running on AIX, but it can on some systems
            uint64_t   totalRAM_  =   GetSystemConfiguration_Memory ().fTotalPhysicalRAM;
            Assert (result.fPhysicalMemory.fActive.Value () <= totalRAM_);
            Assert (result.fPhysicalMemory.fInactive.Value () <= totalRAM_);
            Assert (result.fPhysicalMemory.fFree.Value () <= totalRAM_);
            if (result.fPhysicalMemory.fActive.Value () + result.fPhysicalMemory.fInactive.Value () + result.fPhysicalMemory.fFree.Value () + result.fPhysicalMemory.fOSReserved.Value () != totalRAM_) {
                // Due to apparent races gathering these stats, they may be out of sync. So split the difference (no other obvious way
                // to reconcile them)
                int64_t overage = result.fPhysicalMemory.fActive.Value () + result.fPhysicalMemory.fInactive.Value () + result.fPhysicalMemory.fFree.Value () + result.fPhysicalMemory.fOSReserved.Value ();
                overage -= totalRAM_;
                DbgTrace ("Adjusting reported RAM values for overrage %lld", static_cast<long long int> (overage));
                if (result.fPhysicalMemory.fFree.Value () > overage / 3) {
                    result.fPhysicalMemory.fFree -= overage / 3;
                    overage -= overage / 3;
                }
                else {
                    overage -= result.fPhysicalMemory.fFree.Value ();
                    result.fPhysicalMemory.fFree = static_cast<uint64_t> (0);
                }
                if (result.fPhysicalMemory.fActive.Value () > overage / 2) {
                    result.fPhysicalMemory.fActive -= overage / 2;
                    overage -= overage / 2;
                }
                else {
                    overage -= result.fPhysicalMemory.fActive.Value ();
                    result.fPhysicalMemory.fActive = static_cast<uint64_t> (0);
                }
                if (result.fPhysicalMemory.fInactive.Value () > overage) {
                    result.fPhysicalMemory.fActive -= overage;
                    overage -= overage;
                }
                else {
                    overage -= result.fPhysicalMemory.fInactive.Value ();
                    result.fPhysicalMemory.fInactive = static_cast<uint64_t> (0);
                }
            }
            Assert (result.fPhysicalMemory.fActive.Value () + result.fPhysicalMemory.fInactive.Value () + result.fPhysicalMemory.fFree.Value () + result.fPhysicalMemory.fOSReserved.Value () == GetSystemConfiguration_Memory ().fTotalPhysicalRAM);

            /*
             *  This is our best estimate of what is available. On LINUX, we can also add in 'SReclaimable' - kernel RAM
             *  we could use if needed.
             */
            result.fPhysicalMemory.fAvailable = memResults.real_avail * 4 * 1024;

            /*
             *  This makes Sterling's graphs look better, but I think including unpinned active RAM is
             *  not likely correct. Maybe if we included non-dirty RAM? but dirty doesnt appear to
             *  lock. So disable this for now.
             */
            const   bool    kIncludeUnPinnedActiveRAMAsAvailable_       { false };
            if (kIncludeUnPinnedActiveRAMAsAvailable_) {
                // What we are calling available doesn't count un-pinned
                uint64_t    pinnedRAM   =   memResults.real_pinned * 4 * 1024;
                if (result.fPhysicalMemory.fActive > pinnedRAM) {
                    result.fPhysicalMemory.fAvailable += (*result.fPhysicalMemory.fActive - pinnedRAM);
                }
            }

            /*
             *  This number (virt_active) by nmon to summarize virtual memory status. But very little else...
             *
             *  Tried (memResults.real_inuse + (memResults.pgsp_total - memResults.pgsp_free)) * 4 * 1024;
             *  but that didn't produce a good/representative answer.
             */
            result.fVirtualMemory.fCommittedBytes = (memResults.virt_active) * 4 * 1024;

            result.fVirtualMemory.fCommitLimit = memResults.virt_total * 4 * 1024;

            result.fVirtualMemory.fPagefileTotalSize  = memResults.pgsp_total * 4 * 1024;

            result.fPaging.fMajorPageFaultsSinceBoot = memResults.pgspins;
            result.fPaging.fMinorPageFaultsSinceBoot = memResults.pgins - memResults.pgspins;
            result.fPaging.fPageOutsSinceBoot = memResults.pgouts;

            Time::DurationSecondsType   now = Time::GetTickCount ();
            auto    doAve_ = [] (Time::DurationSecondsType savedVMPageStatsAt, Time::DurationSecondsType now, uint64_t* savedBaseline, Optional<uint64_t> faultsSinceBoot, Optional<double>* faultsPerSecond) {
                if (faultsSinceBoot) {
                    if (savedVMPageStatsAt != 0) {
                        *faultsPerSecond = (*faultsSinceBoot - *savedBaseline) / (now - savedVMPageStatsAt);
                    }
                    *savedBaseline = *faultsSinceBoot;
                }
            };
            doAve_ (fSaved_VMPageStats_At, now, &fSaved_MinorPageFaultsSinceBoot, result.fPaging.fMinorPageFaultsSinceBoot, &result.fPaging.fMinorPageFaultsPerSecond);
            doAve_ (fSaved_VMPageStats_At, now, &fSaved_MajorPageFaultsSinceBoot, result.fPaging.fMajorPageFaultsSinceBoot, &result.fPaging.fMajorPageFaultsPerSecond);
            doAve_ (fSaved_VMPageStats_At, now, &fSaved_PageOutsSinceBoot, memResults.pgouts, &result.fPaging.fPageOutsPerSecond);
            fSaved_VMPageStats_At = now;

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






#if     qPlatform_Linux
namespace {
    struct  CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        uint64_t                    fSaved_MinorPageFaultsSinceBoot {};
        uint64_t                    fSaved_PageOutsSinceBoot {};
        Time::DurationSecondsType   fSaved_VMPageStats_At {};

        CapturerWithContext_Linux_ (Options options)
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
        CapturerWithContext_Linux_ (const CapturerWithContext_Linux_&) = default;   // copy by value fine - no need to re-wait...

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
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Read_ProcMemInfo");
#endif
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
            /*
             *  @todo   minor performance note: we current do about 10 (tha many strings * 45 (about that many lines in file) compares.
             *          We couuld read data and form a map so lookups faster. Or at least keep a list of items alreayd found and not
             *          look for them more, and stop when none left to look for (wont work if some like sreclaimable not found).
             */
            static  const   String_Constant kProcMemInfoFileName_ { L"/proc/meminfo" };
            DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
            // Note - /procfs files always unseekable
            Optional<uint64_t>  memTotal;
            Optional<uint64_t>  slabReclaimable;
            Optional<uint64_t>  slab;               // older kernels dont have slabReclaimable
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                static  const   String_Constant kMemTotalLabel_     { L"MemTotal" };
                static  const   String_Constant kMemFreelLabel_     { L"MemFree" };
                static  const   String_Constant kMemAvailableLabel_ { L"MemAvailable" };
                static  const   String_Constant kActiveLabel_       { L"Active" };
                static  const   String_Constant kInactiveLabel_     { L"Inactive" };
                static  const   String_Constant kCommitLimitLabel_  { L"CommitLimit" };
                static  const   String_Constant kCommitted_ASLabel_ { L"Committed_AS" };
                static  const   String_Constant kSwapTotalLabel_    { L"SwapTotal" };
                static  const   String_Constant kSReclaimableLabel_ { L"SReclaimable" };
                static  const   String_Constant kSlabLabel_         { L"Slab" };
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
            if (updateResult->fPhysicalMemory.fAvailable.IsMissing () and updateResult->fPhysicalMemory.fFree and updateResult->fPhysicalMemory.fInactive) {
                if (slabReclaimable.IsMissing ()) {
                    // wag
                    slabReclaimable = slab.Value () / 2;
                }
                updateResult->fPhysicalMemory.fAvailable = *updateResult->fPhysicalMemory.fFree + *updateResult->fPhysicalMemory.fInactive + slabReclaimable.Value ();
            }
        }
        void    Read_ProcVMStat_ (Instruments::Memory::Info* updateResult)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Read_ProcVMStat_");
#endif
            auto    ReadVMStatLine_ = [] (Optional<uint64_t>* result, const String & n, const Sequence<String>& line) -> unsigned int {
                if (line.size () >= 2 and line[0] == n)
                {
                    *result = Characters::String2Int<uint64_t> (line[1]);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                    return 1;
                }
                return 0;
            };
            {
                static  const   String_Constant kProcVMStatFileName_ { L"/proc/vmstat" };
                Optional<uint64_t>  pgfault;
                Optional<uint64_t>  pgpgout;
                {
                    unsigned    int nFound {};
                    // Note - /procfs files always unseekable
                    DataExchange::CharacterDelimitedLines::Reader reader {{ ' ', '\t' }};
                    for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcVMStatFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                        static  const   String_Constant kpgfaultLabel_      { L"pgfault" };
                        static  const   String_Constant kpgpgoutLabel_      { L"pgpgout" };
                        static  const   String_Constant kpgmajfaultLabel_   { L"pgmajfault" };
                        nFound += ReadVMStatLine_ (&pgfault, kpgfaultLabel_, line);
                        // Unsure if this should be pgpgout or pgpgout, or none of the above. On a system with no swap, I seem to get both happening,
                        // which makes no sense
                        nFound += ReadVMStatLine_ (&pgpgout, kpgpgoutLabel_, line);     // tried pgpgout but I dont know what it is but doesnt appear to be pages out - noneof this well documented
                        nFound += ReadVMStatLine_ (&updateResult->fPaging.fMajorPageFaultsSinceBoot, kpgmajfaultLabel_, line);
                        if (nFound >= 3) {
                            break;  // avoid reading rest if all found
                        }
                    }
                }
                Time::DurationSecondsType   now = Time::GetTickCount ();
                updateResult->fPaging.fPageOutsSinceBoot = pgpgout;
                if (pgfault and updateResult->fPaging.fMajorPageFaultsSinceBoot) {
                    updateResult->fPaging.fMinorPageFaultsSinceBoot = *pgfault - *updateResult->fPaging.fMajorPageFaultsSinceBoot;
                }
                auto    doAve_ = [] (Time::DurationSecondsType savedVMPageStatsAt, Time::DurationSecondsType now, uint64_t* savedBaseline, Optional<uint64_t> faultsSinceBoot, Optional<double>* faultsPerSecond) {
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





#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if     qUseWMICollectionSupport_
        WMICollector            fMemoryWMICollector_ { String_Constant { L"Memory" }, {kInstanceName_},  {kCommittedBytes_, kCommitLimit_, kHardPageFaultsPerSec_, kPagesOutPerSec_, kFreeMem_, kHardwareReserved1_, kHardwareReserved2_ } };
#endif
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture_ ();    // to pre-seed context
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            for (String i : fMemoryWMICollector_.GetAvailableCounters ()) {
                DbgTrace (L"Memory:Countername: %s", i.c_str ());
            }
#endif
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
            uint64_t                    totalRAM {};
            Read_GlobalMemoryStatusEx_ (&result, &totalRAM);
#if     qUseWMICollectionSupport_
            Read_WMI_ (&result, totalRAM);
#endif
            // I've found no docs to clearly state one way or another, but empirically from looking at the graph in
            // Resource Monitor, the amount reported as 'hardware' - which I'm thinking is roughly 'osreserved' is
            // subtracted from 'standby'.
            if (result.fPhysicalMemory.fOSReserved) {
                result.fPhysicalMemory.fInactive -= result.fPhysicalMemory.fOSReserved;
            }
            NoteCompletedCapture_ ();
            return result;
        }
        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        void    Read_GlobalMemoryStatusEx_ (Instruments::Memory::Info* updateResult, uint64_t* totalRAM)
        {
            RequireNotNull (totalRAM);
            MEMORYSTATUSEX  statex {};
            statex.dwLength = sizeof (statex);
            Verify (::GlobalMemoryStatusEx (&statex) != 0);
            updateResult->fPhysicalMemory.fFree = statex.ullAvailPhys;      // overridden later, but a good first estimate if we dont use WMI
            *totalRAM = statex.ullTotalPhys;
            updateResult->fVirtualMemory.fPagefileTotalSize = statex.ullTotalPageFile;

            /*
             *  dwMemoryLoad
             *  A number between 0 and 100 that specifies the approximate percentage of physical
             *  memory that is in use (0 indicates no memory use and 100 indicates full memory use)
             */
            updateResult->fPhysicalMemory.fActive = statex.ullTotalPhys * statex.dwMemoryLoad / 100;
        }
#if     qUseWMICollectionSupport_
        void    Read_WMI_ (Instruments::Memory::Info* updateResult, uint64_t totalRAM)
        {
            fMemoryWMICollector_.Collect ();
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommittedBytes_).CopyToIf (&updateResult->fVirtualMemory.fCommittedBytes);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_).CopyToIf (&updateResult->fVirtualMemory.fCommitLimit);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kHardPageFaultsPerSec_).CopyToIf (&updateResult->fPaging.fMajorPageFaultsPerSecond);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kPagesOutPerSec_).CopyToIf (&updateResult->fPaging.fPageOutsPerSecond);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kFreeMem_).CopyToIf (&updateResult->fPhysicalMemory.fFree);
            if (Optional<double> freeMem = fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kFreeMem_)) {
                if (updateResult->fPhysicalMemory.fActive) {
                    // Active + Inactive + Free == TotalRAM
                    updateResult->fPhysicalMemory.fInactive = totalRAM - *updateResult->fPhysicalMemory.fActive - static_cast<uint64_t> (*freeMem);
                }
            }
            updateResult->fPhysicalMemory.fOSReserved.clear ();
            updateResult->fPhysicalMemory.fOSReserved.AccumulateIf (Optional<uint64_t> (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kHardwareReserved1_)));
            updateResult->fPhysicalMemory.fOSReserved.AccumulateIf (Optional<uint64_t> (fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kHardwareReserved2_)));
            // fPhysicalMemory.fAvailable WAG TMPHACK - probably should add "hardware in use" memory + private WS of each process + shared memory "WS" - but not easy to compute...
            updateResult->fPhysicalMemory.fAvailable = updateResult->fPhysicalMemory.fFree + updateResult->fPhysicalMemory.fInactive;
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
#elif   qPlatform_Linux
            , CapturerWithContext_Linux_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_AIX
        using inherited = CapturerWithContext_AIX_;
#elif   qPlatform_Linux
        using inherited = CapturerWithContext_Linux_;
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
            Instruments::Memory::Info   result = inherited::capture ();
            Ensure (result.fPhysicalMemory.fActive.Value () + result.fPhysicalMemory.fInactive.Value () + result.fPhysicalMemory.fFree.Value () + result.fPhysicalMemory.fOSReserved.Value () == GetSystemConfiguration_Memory ().fTotalPhysicalRAM);
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
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info::PhysicalRAMDetailsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fAvailable), String_Constant (L"Available"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fActive), String_Constant (L"Active"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fInactive), String_Constant (L"Inactive"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fFree), String_Constant (L"Free"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PhysicalRAMDetailsType, fOSReserved), String_Constant (L"OS-Reserved"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<Info::VirtualMemoryDetailsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::VirtualMemoryDetailsType, fCommitLimit), String_Constant (L"Commit-Limit"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::VirtualMemoryDetailsType, fCommittedBytes), String_Constant (L"Committed-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::VirtualMemoryDetailsType, fPagefileTotalSize), String_Constant (L"Pagefile-Total-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<Info::PagingDetailsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMajorPageFaultsSinceBoot), String_Constant (L"Major-Faults-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMinorPageFaultsSinceBoot), String_Constant (L"Minor-Faults-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fPageOutsSinceBoot), String_Constant (L"Page-Outs-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMajorPageFaultsPerSecond), String_Constant (L"Major-Faults-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fMinorPageFaultsPerSecond), String_Constant (L"Minor-Faults-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::PagingDetailsType, fPageOutsPerSecond), String_Constant (L"Page-Outs-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fPhysicalMemory), String_Constant (L"Physical-Memory") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fVirtualMemory), String_Constant (L"Virtual-Memory") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fPaging), String_Constant (L"Paging") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
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
        virtual MeasurementSet  Capture () override
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

