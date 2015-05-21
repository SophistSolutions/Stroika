/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <sys/sysinfo.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#include    <psapi.h>
#include    <Wdbgexts.h>
#endif

#include    "../../../Foundation/Characters/CString/Utilities.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Characters/StringBuilder.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/Execution/Thread.h"
#if     qPlatform_POSIX
#include    "../../../Foundation/Execution/Platform/POSIX/Users.h"
#elif   qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Users.h"
#endif
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "../../../Foundation/IO/FileSystem/FileSystem.h"
#include    "../../../Foundation/IO/FileSystem/PathName.h"
#include    "../../../Foundation/Memory/BLOB.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Streams/BasicBinaryInputOutputStream.h"
#include    "../../../Foundation/Streams/BufferedBinaryInputStream.h"
#include    "../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"
#include    "../../../Foundation/Streams/iostream/FStreamSupport.h"

#include    "ProcessDetails.h"


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails;

using   Characters::String_Constant;
using   IO::FileSystem::BinaryFileInputStream;
using   Time::DurationSecondsType;






#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif






#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif






#if     defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "psapi.lib")
#endif





const EnumNames<ProcessType::RunStatus>   ProcessType::Stroika_Enum_Names(RunStatus)
{
    { ProcessType::RunStatus::eRunning, L"Running" },
    { ProcessType::RunStatus::eSleeping, L"Sleeping" },
    { ProcessType::RunStatus::eWaitingOnDisk, L"WaitingOnDisk" },
    { ProcessType::RunStatus::eWaitingOnPaging, L"WaitingOnPaging" },
    { ProcessType::RunStatus::eZombie, L"Zombie" },
    { ProcessType::RunStatus::eSuspended, L"Suspended" },
};









/*
 ********************************************************************************
 ************** Instruments::ProcessDetails::GetObjectVariantMapper *************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::ProcessDetails::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<ProcessType::RunStatus> (ProcessType::Stroika_Enum_Names(RunStatus)));
        mapper.AddCommonType<Optional<String>> ();
        mapper.AddCommonType<Optional<ProcessType::RunStatus>> ();
        mapper.AddCommonType<Optional<pid_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<unsigned int>> ();
        mapper.AddCommonType<Optional<MemorySizeType>> ();
        mapper.AddCommonType<Optional<Time::DateTime>> ();
        mapper.AddCommonType<Optional<DurationSecondsType>> ();
        mapper.AddCommonType<Optional<Mapping<String, String>>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<ProcessType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fKernelProcess), String_Constant (L"Kernel-Process"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fParentProcessID), String_Constant (L"Parent-Process-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fUserName), String_Constant (L"User-Name"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCommandLine), String_Constant (L"Command-Line"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCurrentWorkingDirectory), String_Constant (L"Current-Working-Directory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEnvironmentVariables), String_Constant (L"Environment-Variables"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEXEPath), String_Constant (L"EXE-Path"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fRoot), String_Constant (L"Root"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fProcessStartedAt), String_Constant (L"Process-Started-At"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fRunStatus), String_Constant (L"Run-Status"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fVirtualMemorySize), String_Constant (L"Virtual-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fResidentMemorySize), String_Constant (L"Resident-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPrivateBytes), String_Constant (L"Private-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPageFaultCount), String_Constant (L"Page-Fault-Count"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fWorkingSetSize), String_Constant (L"Working-Set-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fTotalCPUTimeEverUsed), String_Constant (L"Total-CPUTime-Ever-Used"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPercentCPUTime), String_Constant (L"Percent-CPUTime-Used"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fThreadCount), String_Constant (L"Thread-Count"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCombinedIOReadRate), String_Constant (L"Combined-IO-Read-Rate"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCombinedIOWriteRate), String_Constant (L"Combined-IO-Write-Rate"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCombinedIOReadBytes), String_Constant (L"Combined-IO-Read-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCombinedIOWriteBytes), String_Constant (L"Combined-IO-Write-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<ProcessMapType> ();
        return mapper;
    } ();
    return sMapper_;
}














namespace {
    struct  CapturerWithContext_COMMON_ {
        Options                     fOptions_;
        DurationSecondsType         fMinimumAveragingInterval_;
        DurationSecondsType         fPostponeCaptureUntil_ { 0 };
        DateTime                    fLastCapturedAt;
        // skip reporting static (known at process start) data on subsequent reports
        // only used if fCachePolicy == CachePolicy::eOmitUnchangedValues
        Set<pid_t>                  fStaticSuppressedAgain;
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
        struct PerfStats_ {
            DurationSecondsType     fCapturedAt;
            Optional<double>        fTotalCPUTimeEverUsed;
            Optional<double>        fCombinedIOReadBytes;
            Optional<double>        fCombinedIOWriteBytes;
        };
        Mapping<pid_t, PerfStats_>  fContextStats_;

        CapturerWithContext_POSIX_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture ();        // for side-effect of setting fContextStats_
        }

        ProcessMapType  capture ()
        {
            ProcessMapType  result {};
            Execution::SleepUntil (fPostponeCaptureUntil_);
            if (fOptions_.fAllowUse_ProcFS) {
                result = ExtractFromProcFS_ ();
            }
            else if (fOptions_.fAllowUse_PS) {
                result = capture_using_ps_ ();
            }
            fLastCapturedAt = DateTime::Now ();
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            return result;
        }

        // One character from the string "RSDZTW" where R is running,
        // S is sleeping in an interruptible wait, D is waiting in uninterruptible disk sleep,
        // Z is zombie, T is traced or stopped (on a signal), and W is paging.
        Optional<ProcessType::RunStatus>    cvtStatusCharToStatus_ (char state)
        {
            switch (state) {
                case 'R':
                    return ProcessType::RunStatus::eRunning;
                case 'S':
                    return ProcessType::RunStatus::eSleeping;
                case 'D':
                    return ProcessType::RunStatus::eWaitingOnDisk;
                case 'Z':
                    return ProcessType::RunStatus::eZombie;
                case 'T':
                    return ProcessType::RunStatus::eSuspended;
                case 'W':
                    return ProcessType::RunStatus::eWaitingOnPaging;
            }
            return Optional<ProcessType::RunStatus> ();
        }

        ProcessMapType  ExtractFromProcFS_ ()
        {
            /// Most status - like time - come from http://linux.die.net/man/5/proc
            ///proc/[pid]/stat
            //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
            //
            ProcessMapType  results;

            Mapping<pid_t, PerfStats_>  newContextStats;

            /*
             *  NOTE: the Linux procfs allows access to PROCESS info or THREADINFO (what linux calls lightweight processes).
             *
             *  You can tell if a process is a real process id or thread id, by seeing if the tgid (sometimes tid) or task
             *  group id) is the same as the PID. If yes, its a process. If no, its a thread in the tgid process.
             *
             *  However, iterating over the files in teh /proc filesystem APPEARS to only produce real processes, and to skip
             *  the lightweight process thread ids,  so we don't need to specially filter them out. However, I've not found
             *  this claim documented anywhere, so beware...
             */
            for (String dir : IO::FileSystem::DirectoryIterable (String_Constant (L"/proc"))) {
                bool isAllNumeric = not dir.FindFirstThat ([] (Character c) -> bool { return not c.IsDigit (); });
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ExtractFromProcFS_::reading proc files");
                DbgTrace (L"isAllNumeric=%d, dir= %s", isAllNumeric, dir.c_str ());
#endif
                if (isAllNumeric) {
                    pid_t               pid     { String2Int<pid_t> (dir) };
                    DurationSecondsType now     { Time::GetTickCount () };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("reading for pid = %d", pid);
#endif
                    String      processDirPath  =   IO::FileSystem::AssureDirectoryPathSlashTerminated (String_Constant (L"/proc/") + dir);
                    bool        grabStaticData  =   fOptions_.fCachePolicy == CachePolicy::eIncludeAllRequestedValues or not fStaticSuppressedAgain.Contains (pid);

                    ProcessType processDetails;

                    if (fOptions_.fCaptureCurrentWorkingDirectory) {
                        processDetails.fCurrentWorkingDirectory = OptionallyResolveShortcut_ (processDirPath + String_Constant (L"cwd"));
                    }

                    if (grabStaticData) {
                        processDetails.fCommandLine = OptionallyReadFileString_ (processDirPath + String_Constant (L"cmdline"));
                        if (fOptions_.fCaptureEnvironmentVariables) {
                            processDetails.fEnvironmentVariables = OptionallyReadFileStringsMap_ (processDirPath + String_Constant (L"environ"));
                        }
                        processDetails.fEXEPath = OptionallyResolveShortcut_ (processDirPath + String_Constant (L"exe"));
                        if (processDetails.fEXEPath and processDetails.fEXEPath->EndsWith (L" (deleted)")) {
                            processDetails.fEXEPath = processDetails.fEXEPath->CircularSubString (0, -10);
                        }
                        /*
                         *      \note   In POSIX/fAllowUse_ProcFS mode Fix EXEPath/commandline for 'kernel' processes.
                         *              http://unix.stackexchange.com/questions/191594/how-can-i-determine-if-a-process-is-a-system-process
                         *              Can use value from reading EXE or is parent process id is 0, or parent process id is kernel process
                         *              (means kernel process/thread).
                         *
                         *  Improve this logic below - checking for exact error code from readlink..as they say in that article.
                         */
                        pd.fKernelProcess = processDetails.fEXEPath.IsMissing ();
                        if (fOptions_.fCaptureRoot) {
                            processDetails.fRoot = OptionallyResolveShortcut_ (processDirPath + String_Constant (L"root"));
                        }
                    }

                    static  const   double  kClockTick_ = ::sysconf (_SC_CLK_TCK);

                    try {
                        StatFileInfo_   stats    =  ReadStatFile_ (processDirPath + String_Constant (L"stat"));

                        processDetails.fRunStatus = cvtStatusCharToStatus_ (stats.state);

                        static  const   size_t  kPageSizeInBytes_ = ::sysconf (_SC_PAGESIZE);

                        if (grabStaticData) {
                            static  const time_t    kSecsSinceBoot_ = [] () {
                                struct sysinfo info;
                                ::sysinfo (&info);
                                return time(NULL) - info.uptime;
                            } ();
                            //starttime %llu (was %lu before Linux 2.6)
                            //(22) The time the process started after system boot. In kernels before Linux 2.6,
                            // this value was expressed in jiffies. Since Linux 2.6,
                            // the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
                            processDetails.fProcessStartedAt = DateTime (static_cast<time_t> (stats.start_time / kClockTick_ + kSecsSinceBoot_));
                        }

                        processDetails.fTotalCPUTimeEverUsed = (double (stats.utime) + double (stats.stime)) / kClockTick_;
                        if (Optional<PerfStats_> p = fContextStats_.Lookup (pid)) {
                            if (p->fTotalCPUTimeEverUsed) {
                                processDetails.fPercentCPUTime =   (*processDetails.fTotalCPUTimeEverUsed - *p->fTotalCPUTimeEverUsed) * 100.0 / (now - p->fCapturedAt);
                            }
                        }
                        if (stats.nlwp != 0) {
                            processDetails.fThreadCount = stats.nlwp;
                        }
                        if (grabStaticData) {
                            processDetails.fParentProcessID = stats.ppid;
                        }
                        processDetails.fVirtualMemorySize = stats.vsize;
                        processDetails.fResidentMemorySize = stats.rss * kPageSizeInBytes_;

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"loaded processDetails.fProcessStartedAt=%s wuit stats.start_time = %lld", (*processDetails.fProcessStartedAt).Format ().c_str (), stats.start_time);
                        DbgTrace (L"loaded processDetails.fTotalCPUTimeEverUsed=%f wuit stats.utime = %lld, stats.stime = %lld", (*processDetails.fTotalCPUTimeEverUsed), stats.utime , stats.stime);
#endif
                    }
                    catch (...) {
                    }

                    if (grabStaticData) {
                        try {
                            if (processDetails.fKernelProcess == true) {
                                // I think these are always running as root -- LGP 2015-05-21
                                processDetails.fUserName = String_Constant { L"root" };
                            }
                            else {
                                proc_status_data_   stats    =  Readproc_proc_status_data_ (processDirPath + String_Constant (L"status"));
                                processDetails.fUserName = Execution::Platform::POSIX::uid_t2UserName (stats.ruid);
                            }
                        }
                        catch (...) {
                        }
                    }

                    try {
                        // @todo maybe able to optimize and not check this if processDetails.fKernelProcess == true
                        Optional<proc_io_data_>   stats    =  Readproc_io_data_ (processDirPath + String_Constant (L"io"));
                        if (stats.IsPresent ()) {
                            processDetails.fCombinedIOReadBytes = (*stats).read_bytes;
                            processDetails.fCombinedIOWriteBytes = (*stats).write_bytes;
                            if (Optional<PerfStats_> p = fContextStats_.Lookup (pid)) {
                                if (p->fCombinedIOReadBytes) {
                                    processDetails.fCombinedIOReadRate =   (*processDetails.fCombinedIOReadBytes - *p->fCombinedIOReadBytes) / (now - p->fCapturedAt);
                                }
                                if (p->fCombinedIOWriteBytes) {
                                    processDetails.fCombinedIOWriteRate =   (*processDetails.fCombinedIOWriteBytes - *p->fCombinedIOWriteBytes) / (now - p->fCapturedAt);
                                }
                            }
                        }
                    }
                    catch (...) {
                    }

                    if (processDetails.fTotalCPUTimeEverUsed or processDetails.fCombinedIOReadBytes or processDetails.fCombinedIOWriteBytes) {
                        newContextStats.Add (pid, PerfStats_ { now, processDetails.fTotalCPUTimeEverUsed, processDetails.fCombinedIOReadBytes, processDetails.fCombinedIOWriteBytes });
                    }
                    results.Add (pid, processDetails);
                }
            }
            fContextStats_ = newContextStats;
            if (fOptions_.fCachePolicy == CachePolicy::eOmitUnchangedValues) {
                fStaticSuppressedAgain = Set<pid_t> (results.Keys ());
            }
            return results;
        }
        template    <typename T>
        Optional<T> OptionallyReadIfFileExists_ (const String& fullPath, const function<T(const Streams::BinaryInputStream&)>& reader)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return reader (BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable)));
            }
            return Optional<T> ();
        }

        // this reads /proc format files - meaning that a trialing nul-byte is the EOS
        String  ReadFileString_(const Streams::BinaryInputStream& in)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ReadFileString_");
#endif
            StringBuilder sb;
            for (Memory::Optional<Memory::Byte> b; (b = in.Read ()).IsPresent ();) {
                if (*b == '\0') {
                    break;
                }
                else {
                    sb.Append ((char) (*b));    // for now assume no charset
                }
            }
            return (sb.As<String> ());
        }
        String  ReadFileString_(const String& fullPath)
        {
            return ReadFileString_ (BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable));
        }
        Sequence<String>  ReadFileStrings_(const String& fullPath)
        {
            Sequence<String>            results;
            Streams::BinaryInputStream  in = BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable);
            StringBuilder               sb;
            for (Memory::Optional<Memory::Byte> b; (b = in.Read ()).IsPresent ();) {
                if (*b == '\0') {
                    results.Append (sb.As<String> ());
                    sb.clear();
                }
                else {
                    sb.Append ((char) (*b));    // for now assume no charset
                }
            }
            return results;
        }
        Mapping<String, String>  ReadFileStringsMap_(const String& fullPath)
        {
            Mapping<String, String>    results;
            for (String i : ReadFileStrings_ (fullPath)) {
                auto tokens = i.Tokenize (Set<Character> { '=' });
                if (tokens.size () == 2) {
                    results.Add (tokens[0], tokens[1]);
                }
            }
            return results;
        }

        // if fails (cuz not readable) dont throw but return missing, but avoid noisy stroika exception logging
        Optional<String>    OptionallyReadFileString_(const String& fullPath)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileString_ (fullPath));
            }
            return Optional<String> ();
        }
        // if fails (cuz not readable) dont throw but return missing, but avoid noisy stroika exception logging
        Optional<String>    OptionallyResolveShortcut_ (const String& shortcutPath)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (shortcutPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return IO::FileSystem::FileSystem::Default ().ResolveShortcut (shortcutPath));
            }
            return Optional<String> ();
        }
        Optional<Mapping<String, String>>  OptionallyReadFileStringsMap_(const String& fullPath)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileStringsMap_ (fullPath));
            }
            return Optional<Mapping<String, String>> ();
        }

        struct  StatFileInfo_ {
            //@todo REDO BASED on http://linux.die.net/man/5/proc,  search for '/proc/[pid]/stat'

            // trim down and find better source - but for now use 'procps-3.2.8\proc\'
            int                 ppid;
            char                state;      // stat,status     single-char code for process state (S=sleeping)
            unsigned long long  utime;      // stat            user-mode CPU time accumulated by process
            unsigned long long  stime;      // stat            kernel-mode CPU time accumulated by process
            unsigned long long  cutime;     // stat            cumulative utime of process and reaped children
            unsigned long long  cstime;     // stat            cumulative stime of process and reaped children
            unsigned long long  start_time; // stat            start time of process -- seconds since 1-1-70

            long
            priority,   // stat            kernel scheduling priority
            nice,       // stat            standard unix nice level of process
            rss,        // stat            resident set size from /proc/#/stat (pages)
            alarm,      // stat            ?
            // the next 7 members come from /proc/#/statm
            size,       // statm           total # of pages of memory
            resident,   // statm           number of resident set (non-swapped) pages (4k)
            share,      // statm           number of pages of shared (mmap'd) memory
            trs,        // statm           text resident set size
            lrs,        // statm           shared-lib resident set size
            drs,        // statm           data resident set size
            dt;     // statm           dirty pages

            unsigned long
            vm_size,        // status          same as vsize in kb
            vm_lock,        // status          locked pages in kb
            vm_rss,         // status          same as rss in kb
            vm_data,        // status          data size
            vm_stack,       // status          stack size
            vm_exe,         // status          executable size
            vm_lib,         // status          library size (all pages, not just used ones)
            rtprio,     // stat            real-time priority
            sched,      // stat            scheduling class
            vsize,      // stat            number of pages of virtual memory ...
            rss_rlim,   // stat            resident set size limit?
            flags,      // stat            kernel flags for the process
            min_flt,    // stat            number of minor page faults since process start
            maj_flt,    // stat            number of major page faults since process start
            cmin_flt,   // stat            cumulative min_flt of process and child processes
            cmaj_flt;   // stat            cumulative maj_flt of process and child processes

            int
            pgrp,       // stat            process group id
            session,    // stat            session id
            nlwp,       // stat,status     number of threads, or 0 if no clue
            tgid,       // (special)       task group ID, the POSIX PID (see also: tid)
            tty,        // stat            full device number of controlling terminal
            euid, egid,     // stat(),status   effective
            ruid, rgid,     // status          real
            suid, sgid,     // status          saved
            fuid, fgid,     // status          fs (used for file access only)
            tpgid,      // stat            terminal process group id
            exit_signal,    // stat            might not be SIGCHLD
            processor;

        };
        StatFileInfo_   ReadStatFile_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ReadStatFile_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif
            StatFileInfo_    result {};
            Streams::BinaryInputStream   in = BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable);
            Byte    data[10 * 1024];
            size_t nBytes = in.Read (begin (data), end (data));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("nBytes read = %d", nBytes);
#endif

            const char* S = reinterpret_cast<const char*> (data);

            ///@TODO - FIX - THIS CODE UNSAFE - CAN CRASH!

            {
                S = strchr(S, '(') + 1;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S = %x", S);
#endif
                const char* tmp = strrchr(S, ')');
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S(tmp) = %x", tmp);
#endif
                S = tmp + 2;                 // skip ") "
            }

            // MSVC SILLY WARNING ABOUT USING swscanf_s
            // (warning doesnt appear to check if we have mismatch between types and format args provided.
            //      --LGP 2015-01-07
            DISABLE_COMPILER_MSC_WARNING_START(4996)
            int num = sscanf(S,
                             "%c "
                             "%d %d %d %d %d "
                             "%lu %lu %lu %lu %lu "
                             "%llu %llu %llu %llu "  /* utime stime cutime cstime */
                             "%ld %ld "
                             "%d "
                             "%ld "
                             "%llu "  /* start_time */
                             "%lu "
                             "%ld "
#if 0
                             /*
                             "%lu %"KLF"u %"KLF"u %"KLF"u %"KLF"u %"KLF"u "
                             "%*s %*s %*s %*s " // discard, no RT signals & Linux 2.1 used hex
                             "%"KLF"u %*lu %*lu "
                             "%d %d "
                             "%lu %lu"
                             */
#endif
                             ,
                             &result.state,
                             &result.ppid, &result.pgrp, &result.session, &result.tty, &result.tpgid,
                             &result.flags, &result.min_flt, &result.cmin_flt, &result.maj_flt, &result.cmaj_flt,
                             &result.utime, &result.stime, &result.cutime, &result.cstime,
                             &result.priority, &result.nice,
                             &result.nlwp,
                             &result.alarm,
                             &result.start_time,
                             &result.vsize,
                             &result.rss
#if 0
                             & result.rss_rlim, &result.start_code, &result.end_code, &result.start_stack, &result.kstk_esp, &result.kstk_eip,
                             /*     P->signal, P->blocked, P->sigignore, P->sigcatch,   */ /* can't use */
                             &result.wchan, /* &P->nswap, &P->cnswap, */  /* nswap and cnswap dead for 2.4.xx and up */
                             /* -- Linux 2.0.35 ends here -- */
                             &result.exit_signal, &result.processor,  /* 2.2.1 ends with "exit_signal" */
                             /* -- Linux 2.2.8 to 2.5.17 end here -- */
                             &result.rtprio, &result.sched  /* both added to 2.5.18 */
#endif
                            );
            DISABLE_COMPILER_MSC_WARNING_END(4996)// MSVC SILLY WARNING ABOUT USING swscanf_s

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("result.start_time=%lld", result.start_time);
            DbgTrace ("result.vsize=%ld", result.vsize);
            DbgTrace ("result.rss=%ld", result.rss);
            DbgTrace ("result.utime=%lld", result.utime);
#endif

            return result;
        }

        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/3828/io'
        struct proc_io_data_ {
            uint64_t read_bytes;
            uint64_t write_bytes;
        };
        Optional<proc_io_data_>   Readproc_io_data_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::Readproc_io_data_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif

            if (not IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Skipping read cuz no access");
#endif
                return Optional<proc_io_data_> ();
            }
            proc_io_data_    result {};
            ifstream r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf [0] = '\0';
                if (r.getline (buf, sizeof(buf))) {
                    const char kReadLbl_ [] = "read_bytes:";
                    const char kWriteLbl_ [] = "write_bytes:";
                    if (strncmp (buf, kReadLbl_, strlen (kReadLbl_)) == 0) {
                        result.read_bytes = Characters::CString::String2Int<decltype (result.read_bytes)> (buf + strlen (kReadLbl_));
                    }
                    else if (strncmp (buf, kWriteLbl_, strlen (kWriteLbl_)) == 0) {
                        result.write_bytes = Characters::CString::String2Int<decltype (result.write_bytes)> (buf + strlen (kWriteLbl_));
                    }
                }
            }
            return result;
        }

        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/PID/status'
        struct proc_status_data_ {
            uid_t ruid;
        };
        proc_status_data_   Readproc_proc_status_data_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::Readproc_proc_status_data_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif
            proc_status_data_    result {};
            ifstream r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf [0] = '\0';
                if (r.getline (buf, sizeof(buf))) {
                    const char kUidLbl [] = "Uid:";
                    if (strncmp (buf, kUidLbl, strlen(kUidLbl)) == 0) {
                        char* S = buf + strlen(kUidLbl);
                        int ruid = strtol (S, &S, 10);
                        int euid = strtol (S, &S, 10);
                        int suid = strtol (S, &S, 10);
                        int fuid = strtol (S, &S, 10);
                        result.ruid = ruid;
                    }
                }
            }
            return result;
        }
        // consider using this as a backup if /procfs/ not present...
        ProcessMapType  capture_using_ps_ ()
        {
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::capture_using_ps_");
            ProcessMapType  result;
            /*
             *  THOUGHT ABOUT STIME BUT TOO HARD TO PARSE???
             *
             *  EXAMPLE OUTPUT:
             *          lewis@lewis-UbuntuDevVM3:~/Sandbox/Stroika-DevRoot$ ps -e -o "pid,ppid,s,time,rss,vsz,user,nlwp,cmd" | head
             *            PID  PPID S     TIME   RSS    VSZ USER     NLWP CMD
             *              1     0 S 00:00:01  3696 116896 root        1 /lib/systemd/systemd --system --deserialize 18
             *              2     0 S 00:00:00     0      0 root        1 [kthreadd]
             *              3     2 S 00:00:00     0      0 root        1 [ksoftirqd/0]
             *              5     2 S 00:00:00     0      0 root        1 [kworker/0:0H]
             *              7     2 S 00:00:08     0      0 root        1 [rcu_sched]
             *              8     2 S 00:00:00     0      0 root        1 [rcu_bh]
             *              9     2 S 00:00:06     0      0 root        1 [rcuos/0]
             *             10     2 S 00:00:00     0      0 root        1 [rcuob/0]
             *             11     2 S 00:00:00     0      0 root        1 [migration/0]
             */
            using   Execution::ProcessRunner;
            const   int kColCountIncludingCmd_ { 9 };
            ProcessRunner   pr (L"ps -e -o \"pid,ppid,s,time,rss,vsz,user,nlwp,cmd\"");
            Streams::BasicBinaryInputOutputStream   useStdOut;
            pr.SetStdOut (useStdOut);
            pr.Run ();
            String out;
            Streams::TextInputStreamBinaryAdapter   stdOut  =   Streams::TextInputStreamBinaryAdapter (useStdOut);
            bool    skippedHeader   = false;
            size_t  headerLen       =   0;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    headerLen = i.RTrim ().length ();
                    continue;
                }
                Sequence<String>    l    =  i.Tokenize ();
                if (l.size () < kColCountIncludingCmd_) {
                    DbgTrace ("skipping line cuz len=%d", l.size ());
                    continue;
                }
                ProcessType processDetails;
                pid_t   pid = Characters::String2Int<int> (l[0].Trim ());
                processDetails.fParentProcessID = Characters::String2Int<int> (l[1].Trim ());
                {
                    String s = l[2].Trim ();
                    if (s.length () == 1) {
                        processDetails.fRunStatus = cvtStatusCharToStatus_ (static_cast<char> (s[0].As<wchar_t> ()));
                    }
                }
                {
                    string  tmp =   l[3].AsUTF8 ();
                    int hours = 0;
                    int minutes = 0;
                    int seconds = 0;
                    sscanf (tmp.c_str (), "%d:%d:%d", &hours, &minutes, &seconds);
                    processDetails.fTotalCPUTimeEverUsed = hours * 60 * 60 + minutes * 60 + seconds;
                }
                processDetails.fResidentMemorySize =  Characters::String2Int<int> (l[4].Trim ()) * 1024;    // RSS in /proc/xx/stat is * pagesize but this is *1024
                processDetails.fVirtualMemorySize =  Characters::String2Int<int> (l[5].Trim ()) * 1024;
                processDetails.fUserName = l[6].Trim ();
                processDetails.fThreadCount =  Characters::String2Int<unsigned int> (l[7].Trim ());
                {
                    // wrong - must grab EVERYHTING from i past a certain point
                    // Since our first line has headings, its length is our target, minus the 3 chars for CMD
                    const size_t kCmdNameStartsAt_ = headerLen - 3;
                    processDetails.fCommandLine = i.size () <= kCmdNameStartsAt_ ? String () : i.SubString (kCmdNameStartsAt_).RTrim ();
                }
                {
                    String  cmdLineAsString =   processDetails.fCommandLine.Value ();
                    processDetails.fKernelProcess = not cmdLineAsString.empty () and cmdLineAsString[0] == '[';
                    // Fake but usable answer
                    Sequence<String>    t    =  cmdLineAsString.Tokenize ();
                    if (not t.empty ()) {
                        processDetails.fEXEPath = t[0];
                    }
                }
                result.Add (pid, processDetails);
            }
            return result;
        }
    };
};
#endif



#if     qPlatform_Windows
namespace {
    struct UNICODE_STRING {
        USHORT Length;
        USHORT MaximumLength;
        PWSTR Buffer;
    };
    struct PROCESS_BASIC_INFORMATION {
        PVOID Reserved1;
        PVOID /*PPEB*/ PebBaseAddress;
        PVOID Reserved2[2];
        ULONG_PTR UniqueProcessId;
        PVOID Reserved3;
    };
    PVOID GetPebAddress_ (HANDLE ProcessHandle)
    {
        static  LONG    (WINAPI * NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength) =  (LONG    (WINAPI*)(HANDLE , ULONG , PVOID , ULONG , PULONG ))::GetProcAddress (::LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
        PROCESS_BASIC_INFORMATION pbi;
        NtQueryInformationProcess (ProcessHandle, 0, &pbi, sizeof(pbi), NULL);
        return pbi.PebBaseAddress;
    }
}
#endif



#if     qUseWMICollectionSupport_
namespace {
    const   String_Constant     kProcessID_             { L"ID Process" };
    const   String_Constant     kThreadCount_           { L"Thread Count" };
    const   String_Constant     kIOReadBytesPerSecond_  { L"IO Read Bytes/sec" };
    const   String_Constant     kIOWriteBytesPerSecond_ { L"IO Write Bytes/sec" };
    const   String_Constant     kPercentProcessorTime_  { L"% Processor Time" };            // % Processor Time is the percentage of elapsed time that all of process threads
    // used the processor to execution instructions. An instruction is the basic unit of
    // execution in a computer, a thread is the object that executes instructions, and a
    // process is the object created when a program is run. Code executed to handle some
    // hardware interrupts and trap conditions are included in this count.
    const   String_Constant     kElapsedTime_           { L"Elapsed Time" };                // The total elapsed time, in seconds, that this process has been running.
}
#endif





#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if     qUseWMICollectionSupport_
        WMICollector            fProcessWMICollector_ { String_Constant { L"Process" }, {WMICollector::kWildcardInstance},  {kProcessID_, kThreadCount_, kIOReadBytesPerSecond_, kIOWriteBytesPerSecond_, kPercentProcessorTime_, kElapsedTime_ } };
#endif
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
#if   qUseWMICollectionSupport_
            IgnoreExceptionsForCall (fProcessWMICollector_.Collect ()); // prefill with each process capture
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            fLastCapturedAt = DateTime::Now ();
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if     qUseWMICollectionSupport_
            , fProcessWMICollector_ (from.fProcessWMICollector_)
#endif

        {
#if   qUseWMICollectionSupport_
            IgnoreExceptionsForCall (fProcessWMICollector_.Collect ()); // hack cuz no way to copy
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            fLastCapturedAt = DateTime::Now ();
#endif
        }
        ProcessMapType  capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
#if     qUseWMICollectionSupport_
            DurationSecondsType   timeOfPrevCollection = fProcessWMICollector_.GetTimeOfLastCollection ();
            IgnoreExceptionsForCall (fProcessWMICollector_.Collect ()); // hack cuz no way to copy
            DurationSecondsType   timeCollecting { fProcessWMICollector_.GetTimeOfLastCollection () - timeOfPrevCollection };

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            for (String i : fProcessWMICollector_.GetAvailableInstaces ()) {
                DbgTrace (L"WMI instance name %s", i.c_str ());
            }
#endif

            // NOTE THIS IS BUGGY - MUST READ BACK AS INT NOT DOUBLE
            Mapping<pid_t, String>  pid2InstanceMap;
            for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kProcessID_)) {
                pid2InstanceMap.Add (static_cast<int> (i.fValue), i.fKey);
            }
#endif

            ProcessMapType  results;
            for (pid_t pid : GetAllProcessIDs_ ()) {
                ProcessType     processInfo;
                bool            grabStaticData  =   fOptions_.fCachePolicy == CachePolicy::eIncludeAllRequestedValues or not fStaticSuppressedAgain.Contains (pid);
                {
                    HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                    if (hProcess != nullptr) {
                        Execution::Finally cleanup {[hProcess] ()
                        {
                            Verify (::CloseHandle (hProcess));
                        }
                                                   };
                        if (grabStaticData) {
                            Optional<String>    processEXEPath;
                            Optional<pid_t>     parentProcessID;
                            Optional<String>    cmdLine;
                            Optional<String>    userName;
                            LookupProcessPath_ (hProcess,  &processEXEPath, &parentProcessID, &cmdLine, &userName);
                            processEXEPath.CopyToIf (&processInfo.fEXEPath);
                            parentProcessID.CopyToIf (&processInfo.fParentProcessID);
                            cmdLine.CopyToIf (&processInfo.fCommandLine);
                            userName.CopyToIf (&processInfo.fUserName);
                        }
                        {
                            PROCESS_MEMORY_COUNTERS_EX  memInfo;
                            if (::GetProcessMemoryInfo (hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*> (&memInfo), sizeof(memInfo))) {
                                processInfo.fWorkingSetSize = memInfo.WorkingSetSize;
                                processInfo.fPrivateBytes = memInfo.PrivateUsage;
                                processInfo.fPageFaultCount = memInfo.PageFaultCount;
                            }
                        }
                    }
                }
#if   qUseWMICollectionSupport_
                {
                    String instanceVal = pid2InstanceMap.LookupValue (pid);

                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kThreadCount_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fThreadCount = static_cast<unsigned int> (i.fValue);
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kIOReadBytesPerSecond_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fCombinedIOReadRate = i.fValue;
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kIOWriteBytesPerSecond_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fCombinedIOWriteRate = i.fValue;
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kPercentProcessorTime_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fPercentCPUTime = i.fValue;
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    if (grabStaticData) {
                        for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kElapsedTime_)) {
                            if (instanceVal == i.fKey) {
                                processInfo.fProcessStartedAt = DateTime::Now ().AddSeconds (-static_cast<time_t> (i.fValue));
                                break;
                            }
                        }
                    }
                }
#endif
                results.Add (pid, processInfo);
            }
            fLastCapturedAt = DateTime::Now ();
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            if (fOptions_.fCachePolicy == CachePolicy::eOmitUnchangedValues) {
                fStaticSuppressedAgain = Set<pid_t> (results.Keys ());
            }
            return results;
        }
        Set<pid_t>  GetAllProcessIDs_ ()
        {
            DWORD aProcesses[10 * 1024];
            DWORD cbNeeded;

            Set<pid_t> result;
            if (not ::EnumProcesses (aProcesses, sizeof (aProcesses), &cbNeeded)) {
                AssertNotReached ();
                return result;
            }

            // Calculate how many process identifiers were returned.
            DWORD   cProcesses = cbNeeded / sizeof (DWORD);
            for (DWORD i = 0; i < cProcesses; ++i) {
                result.Add (aProcesses[i]);
            }
            return result;
        }
        void    LookupProcessPath_ (HANDLE hProcess, Optional<String>* processEXEPath, Optional<pid_t>* parentProcessID, Optional<String>* cmdLine, Optional<String>* userName)
        {
            RequireNotNull (hProcess);
            RequireNotNull (processEXEPath);
            RequireNotNull (parentProcessID);
            RequireNotNull (cmdLine);
            RequireNotNull (userName);
            HMODULE     hMod        {};    // note no need to free handles returned by EnumProcessModules () accorind to man-page for EnumProcessModules
            DWORD       cbNeeded    {};
            if (::EnumProcessModules (hProcess, &hMod, sizeof (hMod), &cbNeeded)) {
                TCHAR moduleFullPath[MAX_PATH];
                moduleFullPath[0] = '\0';
                if (::GetModuleFileNameEx (hProcess, hMod, moduleFullPath, NEltsOf(moduleFullPath)) != 0) {
                    *processEXEPath =  String::FromSDKString (moduleFullPath);
                }
            }

            {
                const   ULONG   ProcessBasicInformation  = 0;
                static  LONG    (WINAPI * NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength) =  (LONG    (WINAPI*)(HANDLE , ULONG , PVOID , ULONG , PULONG ))::GetProcAddress (::LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
                if (NtQueryInformationProcess) {
                    ULONG_PTR pbi[6];
                    ULONG ulSize = 0;
                    if (NtQueryInformationProcess (hProcess, ProcessBasicInformation,  &pbi, sizeof (pbi), &ulSize) >= 0 && ulSize == sizeof(pbi)) {
                        *parentProcessID =  static_cast<pid_t> (pbi[5]);

                        // Cribbed from http://windows-config.googlecode.com/svn-history/r59/trunk/doc/cmdline/cmdline.cpp
                        void*   pebAddress = GetPebAddress_ (hProcess);
                        if (pebAddress != nullptr) {
                            void*   rtlUserProcParamsAddress {};
#ifdef  _WIN64
                            const int kUserProcParamsOffset_ = 0x20;
                            const int kCmdLineOffset_ = 112;
#else
                            const int kUserProcParamsOffset_ = 0x10;
                            const int kCmdLineOffset_ = 0x40;
#endif
                            /* get the address of ProcessParameters */
                            if (not ::ReadProcessMemory(hProcess, (PCHAR)pebAddress + kUserProcParamsOffset_, &rtlUserProcParamsAddress, sizeof(PVOID), NULL)) {
                                goto SkipCmdLine_;
                            }
                            UNICODE_STRING commandLine;

                            /* read the CommandLine UNICODE_STRING structure */
                            if (not ::ReadProcessMemory (hProcess, (PCHAR)rtlUserProcParamsAddress + kCmdLineOffset_,  &commandLine, sizeof(commandLine), NULL)) {
                                goto SkipCmdLine_;
                            }
                            {
                                size_t  strLen = commandLine.Length / sizeof (WCHAR);   // length field in bytes
                                Memory::SmallStackBuffer<WCHAR> commandLineContents (strLen + 1);
                                /* read the command line */
                                if (not ReadProcessMemory(hProcess, commandLine.Buffer, commandLineContents.begin (), commandLine.Length, NULL)) {
                                    goto SkipCmdLine_;
                                }
                                commandLineContents[strLen] = 0;
                                *cmdLine = commandLineContents.begin ();
                            }
SkipCmdLine_:
                            ;
                        }

                    }
                }
            }
            {
                /*
                 *  This can fail for a variety of reasons - mostly lack of security access. Capture the data if we can, and just don't
                 *  if we cannot.
                 */
                HANDLE processToken = 0;
                if (::OpenProcessToken (hProcess, TOKEN_QUERY, &processToken) != 0)  {
                    Execution::Finally cleanup {[processToken] ()
                    {
                        Verify (::CloseHandle (processToken));
                    }
                                               };
                    DWORD       nlen {};
                    // no idea why needed, but TOKEN_USER buffer not big enuf empirically - LGP 2015-04-30
                    //      https://msdn.microsoft.com/en-us/library/windows/desktop/aa379626(v=vs.85).aspx
                    //          TokenUser
                    //              The buffer receives a TOKEN_USER structure that contains the user account of the token.
                    Byte        tokenUserBuf[1024];
                    TOKEN_USER* tokenUser = reinterpret_cast<TOKEN_USER*> (begin (tokenUserBuf));
                    if (::GetTokenInformation (processToken, TokenUser, tokenUser, sizeof (tokenUserBuf), &nlen) != 0) {
                        Assert (nlen >= sizeof (TOKEN_USER));
                        // @todo not sure we need this IgnoreExceptionsForCall
                        *userName = Execution::Platform::Windows::SID22UserName (tokenUser->User.Sid);
                        //IgnoreExceptionsForCall (*userName = Execution::Platform::Windows::SID22UserName (tokenUser->User.Sid));
                    }
                }
            }
        }
    };
};
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
        CapturerWithContext_ (const Options& options)
            : inherited (options)
        {
        }
        ProcessMapType capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::ProcessDetails capture");
#endif
            return inherited::capture ();
        }
    };
}


const   MeasurementType SystemPerformance::Instruments::ProcessDetails::kProcessMapMeasurement = MeasurementType (String_Constant (L"Process-Details"));






/*
 ********************************************************************************
 ************** Instruments::ProcessDetails::GetInstrument **********************
 ********************************************************************************
 */
Instrument          SystemPerformance::Instruments::ProcessDetails::GetInstrument (const Options& options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return Instrument (
               InstrumentNameType (String_Constant (L"Process-Details")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = useCaptureContext.fLastCapturedAt;
        auto rawMeasurement = useCaptureContext.capture ();
        results.fMeasuredAt = DateTimeRange (before, useCaptureContext.fLastCapturedAt);
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kProcessMapMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kProcessMapMeasurement},
    GetObjectVariantMapper ()
           );
}
