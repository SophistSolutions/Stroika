/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_AIX
#include    <libperfstat.h>
#include    <procinfo.h>
#include    <sys/procfs.h>
#elif   qPlatform_Linux
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
#include    "../../../Foundation/Configuration/SystemConfiguration.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/Execution/ErrNoException.h"
#include    "../../../Foundation/Execution/Module.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/Execution/Thread.h"
#if     qPlatform_POSIX
#include    "../../../Foundation/Execution/Platform/POSIX/Users.h"
#elif   qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include    "../../../Foundation/Execution/Platform/Windows/Users.h"
#endif
#include    "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include    "../../../Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "../../../Foundation/IO/FileSystem/FileSystem.h"
#include    "../../../Foundation/IO/FileSystem/PathName.h"
#include    "../../../Foundation/Memory/BLOB.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/BufferedInputStream.h"
#include    "../../../Foundation/Streams/TextReader.h"
#include    "../../../Foundation/Streams/iostream/FStreamSupport.h"

#include    "Process.h"


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
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::Process;

using   Characters::String_Constant;
using   IO::FileSystem::FileInputStream;
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




#if     qPlatform_Windows
namespace   {
    struct  SetPrivilegeInContext {
        enum    IgnoreError { eIgnoreError };
        HANDLE      fToken_     { INVALID_HANDLE_VALUE };
        SDKString   fPrivilege_;
        SetPrivilegeInContext (LPCTSTR privilege)
            : fPrivilege_ (privilege)
        {
            try {
                setupToken_ ();
                Execution::Platform::Windows::ThrowIfFalseGetLastError (SetPrivilege_ (fToken_, fPrivilege_.c_str (), true));
            }
            catch (...) {
                if (fToken_ != INVALID_HANDLE_VALUE) {
                    ::CloseHandle (fToken_);                    // no nee dto clear fToken_ cuz never fully constructed
                }
                Execution::DoReThrow ();
            }
        }
        SetPrivilegeInContext (LPCTSTR privilege, IgnoreError)
            : fPrivilege_ (privilege)
        {
            try {
                setupToken_ ();
                Execution::Platform::Windows::ThrowIfFalseGetLastError (SetPrivilege_ (fToken_, fPrivilege_.c_str (), true));
            }
            catch (...) {
                if (fToken_ != INVALID_HANDLE_VALUE) {
                    ::CloseHandle (fToken_);
                    fToken_ = INVALID_HANDLE_VALUE;     // do not double closed in DTOR
                }
            }
        }
        SetPrivilegeInContext (const SetPrivilegeInContext&) = delete;
        SetPrivilegeInContext& operator= (const SetPrivilegeInContext&) = delete;
        ~SetPrivilegeInContext ()
        {
            if (fToken_ != INVALID_HANDLE_VALUE) {
                IgnoreExceptionsForCall (SetPrivilege_ (fToken_, fPrivilege_.c_str (), false));
                Verify (::CloseHandle (fToken_));
            }
        }
    private:
        void    setupToken_ ()
        {
            if (not ::OpenThreadToken (::GetCurrentThread (), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &fToken_)) {
                if (::GetLastError () == ERROR_NO_TOKEN) {
                    Execution::Platform::Windows::ThrowIfFalseGetLastError (::ImpersonateSelf (SecurityImpersonation));
                    Execution::Platform::Windows::ThrowIfFalseGetLastError (::OpenThreadToken (::GetCurrentThread (), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &fToken_));
                }
                else {
                    Execution::Platform::Windows::Exception::DoThrow (::GetLastError ());
                }
            }
        }
        bool    SetPrivilege_ (HANDLE hToken, LPCTSTR Privilege, bool bEnablePrivilege)
        {
            LUID    luid;
            if (::LookupPrivilegeValue (NULL, Privilege, &luid) == 0) {
                return false;
            }

            //
            // first pass.  get current privilege setting
            //
            TOKEN_PRIVILEGES tp;
            tp.PrivilegeCount           = 1;
            tp.Privileges[0].Luid       = luid;
            tp.Privileges[0].Attributes = 0;

            TOKEN_PRIVILEGES    tpPrevious;
            DWORD               cbPrevious    =   sizeof (tpPrevious);
            ::AdjustTokenPrivileges (hToken, FALSE, &tp, sizeof(tp), &tpPrevious, &cbPrevious);
            if (::GetLastError () != ERROR_SUCCESS) {
                // wierd but docs for AdjustTokenPrivileges unclear if you can check for failure with return value - or rahter if not updating all privs
                // counts as failure...
                return false;
            }

            //
            // second pass.  set privilege based on previous setting
            //
            tpPrevious.PrivilegeCount       = 1;
            tpPrevious.Privileges[0].Luid   = luid;

            if (bEnablePrivilege) {
                tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
            }
            else {
                tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED & tpPrevious.Privileges[0].Attributes);
            }
            ::AdjustTokenPrivileges (hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL);
            if (::GetLastError () != ERROR_SUCCESS) {
                // wierd but docs for AdjustTokenPrivileges unclear if you can check for failure with return value - or rahter if not updating all privs
                // counts as failure...
                return false;
            }

            return true;
        }
    };
}
#endif






/*
 ********************************************************************************
 ***************** Instruments::Process::GetObjectVariantMapper *****************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Process::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<ProcessType::RunStatus> (ProcessType::Stroika_Enum_Names(RunStatus)));
        mapper.AddCommonType<Optional<String>> ();
        mapper.AddCommonType<Optional<ProcessType::RunStatus>> ();
        mapper.AddCommonType<Optional<pid_t>> ();
        mapper.AddCommonType<Optional<bool>> ();
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
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPrivateVirtualMemorySize), String_Constant (L"Private-Virtual-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fTotalVirtualMemorySize), String_Constant (L"Total-Virtual-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fResidentMemorySize), String_Constant (L"Resident-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPrivateBytes), String_Constant (L"Private-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPageFaultCount), String_Constant (L"Page-Fault-Count"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fMajorPageFaultCount), String_Constant (L"Major-Page-Fault-Count"), StructureFieldInfo::NullFieldHandling::eOmit },
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
        DurationSecondsType         fLastCapturedAt { };
        // skip reporting static (known at process start) data on subsequent reports
        // only used if fCachePolicy == CachePolicy::eOmitUnchangedValues
        Set<pid_t>                  fStaticSuppressedAgain;
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DurationSecondsType    GetLastCaptureAt () const { return fLastCapturedAt; }
        void    NoteCompletedCapture_ ()
        {
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            fLastCapturedAt = Time::GetTickCount ();
        }
    };
}










#if     qPlatform_AIX
namespace {
    /*
     *  Missing items we don't currently capture:
     *      >   fCurrentWorkingDirectory        * easy in procfs
     *      >   fEnvironmentVariables           * I think psinfo has ptr and we can read data from /proc/PID/as
     *      >   fRoot                           * easy in procfs
     *      >   fPageFaultCount
     *      >   fWorkingSetSize
     *
     *  Fragile or broken:
     *      >   fEXE...
     *      >   fCommandLine
     */
    struct  CapturerWithContext_AIX_ : CapturerWithContext_COMMON_ {
        struct PerfStats_ {
            DurationSecondsType     fCapturedAt;
            Optional<double>        fTotalCPUTimeEverUsed;
            Optional<double>        fCombinedIOReadBytes;
            Optional<double>        fCombinedIOWriteBytes;
        };
        Mapping<pid_t, PerfStats_>  fContextStats_;

        CapturerWithContext_AIX_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
            fStaticSuppressedAgain.clear ();    // cuz we never returned these
        }

        ProcessMapType  capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        ProcessMapType  capture_ ()
        {
            ProcessMapType  result {};
            // for now, PS doing better than procfs - so default to that...

            //@todo in API rename this perfstat
            // prefer procfs
            if (fOptions_.fAllowUse_ProcFS) {
                result = capture_using_perfstat_process_t_ ();
            }
            else if (fOptions_.fAllowUse_PS) {
                result = capture_using_ps_ ();
            }

            fLastCapturedAt = Time::GetTickCount ();
            fPostponeCaptureUntil_ = fLastCapturedAt + fMinimumAveragingInterval_;
            return result;
        }

        Mapping<pid_t, String>   capture_pid2CmdLineMapFromPS_ (const Iterable<pid_t>& pids)
        {
            Debug::TraceContextBumper ctx ("{}::CapturerWithContext_AIX_::capture_pid2CmdLineMapFromPS_");
            using   Execution::ProcessRunner;
            Mapping<pid_t, String>   result;
            if (not pids.empty ()) {
                /*
                 *  EXAMPLE OUTPUT:
                 *      $ /usr/bin/ps -p 14417932,11534484 -o "pid,args"
                 *           PID COMMAND
                 *          11534484 bash
                 *          14417932 Analitiqa-II-Agent --Run-As-Service
                 */
                StringBuilder   pidListStr;
                for (pid_t i : pids) {
                    if (not pidListStr.empty ()) {
                        pidListStr += L",";
                    }
                    pidListStr += Characters::Format (L"%d", i);
                }
                ProcessRunner   pr (Characters::Format (L"/usr/bin/ps -p %s -o \"pid,args\"", pidListStr.c_str ()));
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
                bool    skippedHeader   = false;
                for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                    i = i.Trim ();
                    if (not skippedHeader) {
                        skippedHeader = true;
                        continue;
                    }
                    size_t  iSpace = i.Find (' ');
                    if (iSpace == String::npos) {
                        DbgTrace (L"bad line in PS output: %s", i.c_str ());
                        continue;
                    }
                    pid_t   pid = Characters::String2Int<int> (i.SubString (0, iSpace));
                    String  cmdLine = i.SubString (iSpace + 1).RTrim ();
                    Assert (pids.Contains (pid));
                    result.Add (pid, cmdLine);
                }
            }
            return result;
        }
        struct  ProcFSInfo_ {
            String                              fCmdLine;
            pid_t                               fParentProcessID;
            bool                                fIsKernelThread;
            Optional<ProcessType::RunStatus>    fRunStatus;
            Time::DateTime                      fProcessStartedAt;
        };
        Mapping<pid_t, ProcFSInfo_>   capture_Pid2ProcFSInfoMap_ (const Iterable<pid_t>& pids)
        {
            Debug::TraceContextBumper ctx ("{}::CapturerWithContext_AIX_::capture_Pid2ProcFSInfoMap_");
            Mapping<pid_t, ProcFSInfo_>   result;
            for (pid_t pid : pids) {
                psinfo  psInfo;
                {
                    char    filename[1024];
                    snprintf(filename, NEltsOf (filename), "/proc/%d/psinfo", pid);
                    int fd = ::open (filename, O_RDONLY);
                    if (fd < 0) {
                        DbgTrace ("Failed to open '%s' (errno %d) - probably innocuous", filename, errno);
                        continue;
                    }
                    Execution::Finally cleanup {[fd] ()
                    {
                        Verify (::close (fd) == 0);
                    }
                                               };
                    int count = ::read (fd, &psInfo, sizeof (psInfo));
                    if (count != sizeof (psInfo)) {
                        DbgTrace ("Odd size read returned for '%s' sz=%d, (errno %d) - skipping", filename, count, errno);
                        continue;
                    }
                }
                bool    isKernelThread = false;
                // empirically, kernel threads always have PPID 0 or 1
                if (psInfo.pr_ppid == 0 or psInfo.pr_ppid == 1) {
                    pstatus  psStatus;
                    char    filename[1024];
                    snprintf(filename, NEltsOf (filename), "/proc/%d/status", pid);
                    int fd = ::open (filename, O_RDONLY);
                    // Dont skip if we cannot open this file, because we may need to be root to open it, and the flag we get is not that interesting...
                    if (fd < 0) {
                        isKernelThread = (psInfo.pr_ppid == 0); // default/good guess if we cannot open file
                    }
                    else {
                        Execution::Finally cleanup {[fd] ()
                        {
                            Verify (::close (fd) == 0);
                        }
                                                   };
                        int count = ::read (fd, &psStatus, sizeof (psStatus));
                        if (count != sizeof (psStatus)) {
                            DbgTrace ("Odd size read returned for '%s' sz=%d, (errno %d) - skipping", filename, count, errno);
                        }
                        else {
                            isKernelThread = static_cast<bool> (psStatus.pr_flags & PR_ISSYS);
                        }
                    }
                }

                psInfo.pr_psargs[NEltsOf(psInfo.pr_psargs) - 1] = '\0'; // make sure even if data from kernel bad, no crash
                // See http://www-01.ibm.com/support/knowledgecenter/ssw_aix_53/com.ibm.aix.files/doc/aixfiles/proc.htm%23files-proc
                String  cmdLineArgs = String::FromNarrowSDKString (psInfo.pr_psargs);

                // @todo when we fix DateTime to be hgiher precision, we could use that higher precision for startedAt!
                result.Add (pid, ProcFSInfo_ {cmdLineArgs, static_cast<pid_t> (psInfo.pr_ppid), isKernelThread, cvtStatusCharToStatus_ (psInfo.pr_lwp.pr_sname), DateTime (static_cast<time_t> (psInfo.pr_start.tv_sec)) });
            }
            return result;
        }
        ProcessMapType  capture_using_perfstat_process_t_  ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("{}::CapturerWithContext_AIX_::capture_using_perfstat_process_t_");
#endif
            ProcessMapType  results;

            size_t  procCount;
            {
                int proc_count = ::perfstat_process (nullptr, nullptr, sizeof (perfstat_process_t), 0);
                Execution::ThrowErrNoIfNegative (proc_count);
                procCount = static_cast<size_t> (proc_count);
            }

            Memory::SmallStackBuffer<perfstat_process_t> procBuf (procCount);

            perfstat_id_t id;
            strcpy(id.name, "");
            {
                int rc = ::perfstat_process (&id, procBuf.begin () , sizeof(perfstat_process_t), procCount);
                Execution::ThrowErrNoIfNegative (rc);
                Assert (rc <= procCount);   // cannot return more than we allocated space for
                procCount = rc;
            }

            Time::DurationSecondsType now  = Time::GetTickCount ();

            Mapping<pid_t, PerfStats_>  newContextStats;

            Set<pid_t>  pids2LookupStaticInfo;  // for now grab all but soon be smarter
            Set<pid_t>  pids2LookupCmdLine;
            for (size_t i = 0; i < procCount; ++i) {
                ProcessType processDetails;

                pid_t   pid = procBuf[i].pid;

                pids2LookupStaticInfo.Add (pid);
                pids2LookupCmdLine.Add (pid);
                processDetails.fEXEPath = String::FromNarrowSDKString (procBuf[i].proc_name);    // ok default
                processDetails.fTotalCPUTimeEverUsed = static_cast<double> (procBuf[i].ucpu_time + procBuf[i].scpu_time) / 1000.0;

                /*
                 *  Docs from /usr/include/libperfstat.h:
                 *          ...
                 *          proc_size;              --  Virtual Size of the Process in KB(Exclusive Usage, Leaving all Shared Library Text
                 *                                      & Shared File Pages, Shared Memory, Memory Mapped)
                 *
                 *          proc_real_mem_data;     --  Real Memory used for Data in KB
                 *          proc_real_mem_text;     --  Real Memory used for Text in KB
                 *          proc_virt_mem_data;     --  Virtual Memory used to Data in KB
                 *          proc_virt_mem_text;     --  Virtual Memory used for Text in KB
                 *          shared_lib_data_size    --  VData Size from Shared Library in KB
                 *          heap_size;              --  Heap Size in KB
                 *          real_inuse;             --  The Real memory in use(in KB) by the process including all kind of
                 *                                      segments (excluding system segments). This includes Text, Data,
                 *                                      Shared Library Text, Shared Library Data, File Pages,
                 *                                      Shared Memory & Memory Mapped
                 *          virt_inuse;             --  The Virtual memory in use(in KB) by the process including all kind of
                 *                                      segments (excluding system segments). This includes Text, Data,
                 *                                      Shared Library Text, Shared Library Data, File Pages, Shared Memory & Memory Mapped
                 *          ...
                 *          pgsp_inuse;             --  Paging Space used(in KB) inclusive of all segments
                 *          filepages;              --   File Pages used(in KB) including shared pages
                 *          real_inuse_map;         --  Real memory used(in KB) for Shared Memory and Memory Mapped regions
                 *          virt_inuse_map;             --  Virtual Memory used(in KB) for Shared Memory and Memory Mapped regions
                 *          ...
                 */

                /*
                 *  Just REAL mem in use for TEXT and DATA space.
                 *  I'm hoping/assuming that includes the REAM member for RAM allocated (malloc).
                 */
                processDetails.fResidentMemorySize = (procBuf[i].proc_real_mem_data + procBuf[i].proc_real_mem_text) * 1024;

                /*
                 *  Unclear if this should be procBuf[i].proc_virt_mem_data + procBuf[i].proc_virt_mem_text or proc_size
                 */
                processDetails.fPrivateVirtualMemorySize = (procBuf[i].proc_size) * 1024;

                processDetails.fTotalVirtualMemorySize = (procBuf[i].virt_inuse) * 1024;

                /*
                 *  not sure we should count proc_real_mem_text.
                 *
                 *  Also- maybe we should read /proc/5243076/mmap (see https://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.files/proc.htm)
                 *  and grab pr_mflags sections with MA_READ and  MA_WRITE but not MA_SHARED. amd subtrace out overlapping regions (a bit of work but not
                 *  too bad). Stroika DisjointRange may help with this?
                 */
                if (procBuf[i].proc_size >= procBuf[i].proc_virt_mem_text) {    // SB always true
                    processDetails.fPrivateBytes = (procBuf[i].proc_size - procBuf[i].proc_virt_mem_text) * 1024;
                }

                processDetails.fUserName = Execution::Platform::POSIX::uid_t2UserName (procBuf[i].proc_uid);
                processDetails.fThreadCount =  procBuf[i].num_threads;

                if (Optional<PerfStats_> p = fContextStats_.Lookup (pid)) {
                    if (p->fCombinedIOReadBytes) {
                        processDetails.fCombinedIOReadRate =   (procBuf[i].inBytes - *p->fCombinedIOReadBytes) / (now - p->fCapturedAt);
                    }
                    if (p->fCombinedIOWriteBytes) {
                        processDetails.fCombinedIOWriteRate =   (procBuf[i].outBytes - *p->fCombinedIOWriteBytes) / (now - p->fCapturedAt);
                    }
                    if (p->fTotalCPUTimeEverUsed) {
                        processDetails.fPercentCPUTime =   (*processDetails.fTotalCPUTimeEverUsed - *p->fTotalCPUTimeEverUsed) * 100.0 / (now - p->fCapturedAt);
                    }
                }
                else {
                    // if new process we also can capture this data here!
                    processDetails.fCombinedIOReadRate =  procBuf[i].inBytes;
                    processDetails.fCombinedIOWriteRate =  procBuf[i].outBytes;
                    // @todo but minor
                    //????? processDetails.fPercentCPUTime =  *processDetails.fTotalCPUTimeEverUsed but must divide by life of process
                }

                if (processDetails.fTotalCPUTimeEverUsed or processDetails.fCombinedIOReadBytes or processDetails.fCombinedIOWriteBytes) {
                    newContextStats.Add (pid, PerfStats_ { now, processDetails.fTotalCPUTimeEverUsed, processDetails.fCombinedIOReadBytes, processDetails.fCombinedIOWriteBytes });
                }

                // insanely slow, but opimizable
                processDetails.fEXEPath = String::FromSDKString (Execution::GetEXEPathWithHint (pid, procBuf[i].proc_name));

                results.Add (pid, processDetails);
            }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("Finished processing procCount=%d entries, and pids2LookupStaticInfo.size = %d", procCount, pids2LookupStaticInfo.size ());
#endif

            fContextStats_ = newContextStats;

            Mapping<pid_t, ProcFSInfo_>   procFSInfo    =   capture_Pid2ProcFSInfoMap_ (pids2LookupStaticInfo);

            // ProcFS (I THINK) is faster, but using ps for reasons I don't yet understand - appears to give a better answer for the
            // command line. We COULD do ProcFS and then look for max-len command-lines and look them up with ps???
            //          --LGP 2015-09-14
            constexpr   bool    kPreferUsingProcFSOrPSForCommandLines_  { true };
            Mapping<pid_t, String>   pid2CmdLineMap;        // could make this list just ones with 'short' command-line, but could better restrict based on need for EXE anme...
            if (not kPreferUsingProcFSOrPSForCommandLines_) {
                pid2CmdLineMap = capture_pid2CmdLineMapFromPS_ (pids2LookupCmdLine);
            }

            {
                ProcessMapType  updateResults;
                for (KeyValuePair<pid_t, ProcessType> i : results) {
                    pid_t pid = i.fKey;

                    ProcessType pi = i.fValue;

                    if (auto o = procFSInfo.Lookup (i.fKey)) {
                        pi.fKernelProcess = o->fIsKernelThread;
                        pi.fParentProcessID = o->fParentProcessID;
                        pi.fRunStatus = o->fRunStatus;
                        pi.fProcessStartedAt = o->fProcessStartedAt;

                        // Could do this for EXEPATH and fCommandLine
                        //&&&&&
                        {
                            String cmdLine = o->fCmdLine;
                            // if empty, assume we dont know real cmdline, cuz cannot be empty
                            if (not cmdLine.empty ()) {
#if 0
                                {
                                    // Fake but usable answer
                                    Sequence<String>    t    =  cmdLine.Tokenize ();
                                    if (not t.empty () and not t[0].empty () and t[0][0] == '/')
                                    {
                                        pi.fEXEPath = t[0];
                                    }
                                }
#endif
                                if (fOptions_.fCaptureCommandLine and fOptions_.fCaptureCommandLine (pid, pi.fEXEPath.Value ())) {
                                    pi.fCommandLine = cmdLine;
                                }
                            }
                        }
                    }

                    if (auto o = pid2CmdLineMap.Lookup (i.fKey)) {
                        String cmdLine = *o;
                        if (not cmdLine.empty ()) {
#if 0
                            {
                                // Fake but usable answer
                                Sequence<String>    t    =  cmdLine.Tokenize ();
                                if (not t.empty () and not t[0].empty () and t[0][0] == '/')
                                {
                                    pi.fEXEPath = t[0];
                                }
                            }
#endif
                            if (fOptions_.fCaptureCommandLine and fOptions_.fCaptureCommandLine (pid, pi.fEXEPath.Value ())) {
                                pi.fCommandLine = cmdLine;
                            }
                        }
                    }

                    updateResults.Add (pid, pi);
                }
                results = updateResults;
            }

            return results;
        }
        // consider using this as a backup if /procfs/ not present...
        ProcessMapType  capture_using_ps_ ()
        {
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::capture_using_ps_");
            ProcessMapType  result;
            using   Execution::ProcessRunner;
            // @TODO - much of this is wrong - elapsed not used, tdkskIO not used, TIME always zero, and %MEM reproted as RSS - so many bugs.. But testable...

            /*
             *  Thought about STIME but too hard to parse???
             *
             *  EXAMPLE OUTPUT:
             *          $ ps -A -o "pid,ppid,st,time,pmem,vsz,user,thcount,etime,tdiskio,args" | head
             *               PID     PPID S        TIME  %MEM   VSZ     USER THCNT     ELAPSED TDISKIO COMMAND
             *                 0        0 A    00:00:00   0.0   384     root     1  0-03:00:00       - swapper
             *                 1        0 A    00:00:00   0.0   712     root     1  0-03:00:00       - /etc/init
             *            131076        0 A    00:00:00   0.0   448     root     1  0-03:00:00       - wait
             *            196614        0 A    00:00:00   0.0   448     root     1  0-03:00:00       - sched
             *            262152        0 A    00:00:00   0.0   640     root     3  0-03:00:00       - lrud
             *            327690        0 A    00:00:00   0.0   448     root     1  0-03:00:00       - vmptacrt
             *            393228        0 A    00:00:00   0.0   640     root     3  0-03:00:00       - psmd
             *            458766        0 A    00:00:00   0.0   832     root     5  0-03:00:00       - vmmd
             *            524304        0 A    00:00:00   0.0   448     root     1  0-03:00:00       - pvlist
             */
            constexpr   size_t  kVSZ_Idx_               { 5 };
            constexpr   size_t  kUser_Idx_              { 6 };
            constexpr   size_t  kThreadCnt_Idx_         { 7 };
            constexpr   size_t  kColCountIncludingCmd_  { 11 };
            ProcessRunner   pr (L"ps -A -o \"pid,ppid,st,time,pmem,vsz,user,thcount,etime,tdiskio,args\"");
            Streams::MemoryStream<Byte>   useStdOut;
            pr.SetStdOut (useStdOut);
            pr.Run ();
            String out;
            Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
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
                pid_t       pid = Characters::String2Int<int> (l[0].Trim ());
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

                    // GROSS hack cuz time reported always zero above way...
                    if (*processDetails.fTotalCPUTimeEverUsed == 0) {
                        try {
                            ProcessRunner   ppr (Characters::Format (L"ps -p %d", pid));
                            Streams::MemoryStream<Byte>   useStdOut1;
                            ppr.SetStdOut (useStdOut1);
                            ppr.Run ();
                            Streams::TextReader   stdOut2  =   Streams::TextReader (useStdOut1);
                            String i = stdOut2.ReadLine ();
                            i = stdOut2.ReadLine ();
                            Sequence<String> tt = i.Tokenize ();
                            if (tt.size () >= 3) {
                                string  tmp =   tt[2].AsUTF8 ();
                                int minutes = 0;
                                int seconds = 0;
                                sscanf (tmp.c_str (), "%d:%d", &minutes, &seconds);
                                processDetails.fTotalCPUTimeEverUsed = minutes * 60 + seconds;
                            }
                        }
                        catch (...) {
                            // ignore for now
                        }
                    }
                }
                processDetails.fResidentMemorySize =  Characters::String2Int<int> (l[4].Trim ()) * 1024;    // RSS in /proc/xx/stat is * pagesize but this is *1024
                static  uint64_t    kTotalRAM_ = Stroika::Foundation::Configuration::GetSystemConfiguration_Memory ().fTotalPhysicalRAM;
                processDetails.fResidentMemorySize /= 1024;
                processDetails.fResidentMemorySize *= kTotalRAM_ / 100;
                processDetails.fPrivateVirtualMemorySize =  Characters::String2Int<int> (l[kVSZ_Idx_].Trim ()) * 1024;
                processDetails.fUserName = l[kUser_Idx_].Trim ();
                processDetails.fThreadCount =  Characters::String2Int<unsigned int> (l[kThreadCnt_Idx_].Trim ());
                String  cmdLine;
                {
                    // wrong - must grab EVERYHTING from i past a certain point
                    // Since our first line has headings, its length is our target, minus the 3 chars for CMD
                    const size_t kCmdNameStartsAt_ = headerLen - 7;
                    cmdLine = i.size () <= kCmdNameStartsAt_ ? String () : i.SubString (kCmdNameStartsAt_).RTrim ();
                }
                {
                    processDetails.fKernelProcess = pid != 1 and processDetails.fParentProcessID == 0;      // wag?
                    // Fake but usable answer
                    Sequence<String>    t    =  cmdLine.Tokenize ();
                    if (not t.empty () and not t[0].empty () and t[0][0] == '/') {
                        processDetails.fEXEPath = t[0];
                    }
                }
                if (fOptions_.fCaptureCommandLine and fOptions_.fCaptureCommandLine (pid, processDetails.fEXEPath.Value ())) {
                    processDetails.fCommandLine = cmdLine;
                }
                result.Add (pid, processDetails);
            }
            return result;
        }
        Optional<ProcessType::RunStatus>    cvtStatusCharToStatus_ (char state)
        {
            switch (state) {
                case PR_SNAME_TSRUN:
                    return ProcessType::RunStatus::eRunning;
                case PR_SNAME_TSSLEEP:
                    return ProcessType::RunStatus::eSleeping;
                case 'D':
                    return ProcessType::RunStatus::eWaitingOnDisk;
                case PR_SNAME_TSZOMB:
                    return ProcessType::RunStatus::eZombie;
                case PR_SNAME_TSSTOP:
                    return ProcessType::RunStatus::eSuspended;
                case PR_SNAME_TSSWAP:
                    return ProcessType::RunStatus::eWaitingOnPaging;
            }
            return Optional<ProcessType::RunStatus> ();
        }
    };
};
#endif






#if     qPlatform_Linux
namespace {
    /*
     *  Missing items we don't currently capture:
     *      >   fWorkingSetSize
     *
     *  Fragile or broken:
     *      >   fPrivateBytes   doesnt work on RedHat5 - must use /proc/PID/map (see http://stackoverflow.com/questions/1401359/understanding-linux-proc-id-maps)
     */
    struct  CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
        struct PerfStats_ {
            DurationSecondsType     fCapturedAt;
            Optional<double>        fTotalCPUTimeEverUsed;
            Optional<double>        fCombinedIOReadBytes;
            Optional<double>        fCombinedIOWriteBytes;
        };
        Mapping<pid_t, PerfStats_>  fContextStats_;

        CapturerWithContext_Linux_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
            fStaticSuppressedAgain.clear ();    // cuz we never returned these
        }

        ProcessMapType  capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        ProcessMapType  capture_ ()
        {
            ProcessMapType  result {};
            if (fOptions_.fAllowUse_ProcFS) {
                result = ExtractFromProcFS_ ();
            }
            else if (fOptions_.fAllowUse_PS) {
                result = capture_using_ps_ ();
            }
            fLastCapturedAt = Time::GetTickCount ();
            fPostponeCaptureUntil_ = fLastCapturedAt + fMinimumAveragingInterval_;
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
            //
            /// Most status - like time - come from http://linux.die.net/man/5/proc
            ///proc/[pid]/stat
            //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
            //
            static  const   String_Constant kCWDFilename_       { L"cwd" };
            static  const   String_Constant kEXEFilename_       { L"exe" };
            static  const   String_Constant kEnvironFilename_   { L"environ" };
            static  const   String_Constant kRootFilename_      { L"root" };
            static  const   String_Constant kCmdLineFilename_   { L"cmdline" };
            static  const   String_Constant kStatFilename_      { L"stat" };
            static  const   String_Constant kStatusFilename_    { L"status" };
            static  const   String_Constant kIOFilename_        { L"io" };

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
            for (String dir : IO::FileSystem::DirectoryIterable (String_Constant { L"/proc" })) {
                bool isAllNumeric = not dir.FindFirstThat ([] (Character c) -> bool { return not c.IsDigit (); });
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ExtractFromProcFS_::reading proc files");
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

                    if (grabStaticData) {
                        processDetails.fEXEPath = OptionallyResolveShortcut_ (processDirPath + kEXEFilename_);
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
                        processDetails.fKernelProcess = processDetails.fEXEPath.IsMissing ();
                        // Note - many kernel processes have commandline, so dont filter here based on that
                        if (fOptions_.fCaptureCommandLine and fOptions_.fCaptureCommandLine (pid, processDetails.fEXEPath.Value ())) {
                            processDetails.fCommandLine = ReadCmdLineString_ (processDirPath + kCmdLineFilename_);
                        }
                        // kernel process cannot chroot (as far as I know) --LGP 2015-05-21
                        if (fOptions_.fCaptureRoot and processDetails.fKernelProcess == false) {
                            processDetails.fRoot = OptionallyResolveShortcut_ (processDirPath + kRootFilename_);
                        }
                        // kernel process cannot have environment variables (as far as I know) --LGP 2015-05-21
                        if (fOptions_.fCaptureEnvironmentVariables and processDetails.fKernelProcess == false) {
                            processDetails.fEnvironmentVariables = OptionallyReadFileStringsMap_ (processDirPath + kEnvironFilename_);
                        }
                    }

                    // kernel process cannot have current directory (as far as I know) --LGP 2015-05-21
                    if (fOptions_.fCaptureCurrentWorkingDirectory and processDetails.fKernelProcess == false) {
                        processDetails.fCurrentWorkingDirectory = OptionallyResolveShortcut_ (processDirPath + kCWDFilename_);
                    }

                    static  const   double  kClockTick_ = ::sysconf (_SC_CLK_TCK);

                    try {
                        StatFileInfo_   stats    =  ReadStatFile_ (processDirPath + kStatFilename_);

                        processDetails.fRunStatus = cvtStatusCharToStatus_ (stats.state);

                        static  const   size_t  kPageSizeInBytes_ = ::sysconf (_SC_PAGESIZE);

                        if (grabStaticData) {
                            static  const time_t    kUNIXEpochTimeOfBoot_ = [] () {
                                struct sysinfo info;
                                ::sysinfo (&info);
                                return ::time (NULL) - info.uptime;
                            } ();
                            //starttime %llu (was %lu before Linux 2.6)
                            //(22) The time the process started after system boot. In kernels before Linux 2.6,
                            // this value was expressed in jiffies. Since Linux 2.6,
                            // the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
                            processDetails.fProcessStartedAt = DateTime (static_cast<time_t> (stats.start_time / kClockTick_ + kUNIXEpochTimeOfBoot_));
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

                        processDetails.fPrivateVirtualMemorySize = stats.vsize;

                        // Dont know how to easily compute, but I'm sure not hard (add in shared memory of various sorts at worst) - or look at memory map, but
                        // very low priority (like smaps file below)
                        //processDetails.fTotalVirtualMemorySize = stats.vsize;

                        processDetails.fResidentMemorySize = stats.rss * kPageSizeInBytes_;

                        processDetails.fPageFaultCount  = stats.minflt + stats.majflt;
                        processDetails.fMajorPageFaultCount = stats.majflt;

                        /*
                         * Probably best to compute fPrivateBytes from:
                         *       grep  Private /proc/1912/smaps
                         */
                        processDetails.fPrivateBytes = ReadPrivateBytes_ (processDirPath + L"smaps");

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"loaded processDetails.fProcessStartedAt=%s wuit stats.start_time = %lld", processDetails.fProcessStartedAt.Value ().Format ().c_str (), stats.start_time);
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
                                proc_status_data_   stats    =  Readproc_proc_status_data_ (processDirPath + kStatusFilename_);
                                processDetails.fUserName = Execution::Platform::POSIX::uid_t2UserName (stats.ruid);
                            }
                        }
                        catch (...) {
                        }
                    }

                    try {
                        // @todo maybe able to optimize and not check this if processDetails.fKernelProcess == true
                        Optional<proc_io_data_>   stats    =  Readproc_io_data_ (processDirPath + kIOFilename_);
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
        Optional<T> OptionallyReadIfFileExists_ (const String& fullPath, const function<T(const Streams::InputStream<Byte>&)>& reader)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return reader (FileInputStream::mk (fullPath, FileInputStream::eNotSeekable)));
            }
            return Optional<T> ();
        }

        Sequence<String>  ReadFileStrings_(const String& fullPath)
        {
            Sequence<String>            results;
            Streams::InputStream<Byte>  in = FileInputStream::mk (fullPath, FileInputStream::eNotSeekable);
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
        Optional<String>    ReadCmdLineString_(const String& fullPath2CmdLineFile)
        {
            // this reads /proc format files - meaning that a trialing nul-byte is the EOS
            auto ReadFileString_ = []   (const Streams::InputStream<Byte>& in) ->String {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadCmdLineString_");
#endif
                StringBuilder   sb;
                bool            lastCharNullRemappedToSpace = false;
                for (Memory::Optional<Memory::Byte> b; (b = in.Read ()).IsPresent ();)
                {
                    if (*b == '\0') {
                        sb.Append (' ');    // frequently - especially for kernel processes - we see nul bytes that really SB spaces
                        lastCharNullRemappedToSpace = true;
                    }
                    else {
                        sb.Append ((char) (*b));    // for now assume no charset
                        lastCharNullRemappedToSpace = false;
                    }
                }
                if (lastCharNullRemappedToSpace)
                {
                    Assert (sb.length () > 0 and sb.GetAt (sb.length () - 1) == ' ');
                    return String (sb.begin (), sb.end () - 1);
                }
                else {
                    return sb.As<String> ();
                }
            };
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath2CmdLineFile)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileString_ (FileInputStream::mk (fullPath2CmdLineFile, FileInputStream::eNotSeekable)));
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
            /*
             *  From  http://linux.die.net/man/5/proc,  search for '/proc/[pid]/stat'
             *
             *  (1)     pid %d
             *          The process ID.
             *
             *  (2)     comm %s
             *          The filename of the executable, in parentheses. This is visible whether or not the executable is swapped out.
             *
             *  (3)     state %c
             *          One character from the string "RSDZTW" where R is running, S is sleeping in an interruptible wait, D is waiting in uninterruptible disk sleep, Z is zombie, T is traced or stopped (on a signal), and W is paging.
             *
             *  (4)     ppid %d
             *          The PID of the parent.
             *
             *  (5)     pgrp %d
             *          The process group ID of the process.
             *
             *  (6)     session %d
             *          The session ID of the process.
             *
             *  (7)     tty_nr %d
             *          The controlling terminal of the process. (The minor device number is contained in the combination of bits 31 to 20 and 7 to 0; the major device number is in bits 15 to 8.)
             *
             *  (8)     tpgid %d
             *          The ID of the foreground process group of the controlling terminal of the process.
             *
             *  (9)     flags %u (%lu before Linux 2.6.22)
             *          The kernel flags word of the process. For bit meanings, see the PF_* defines in the Linux kernel source file include/linux/sched.h. Details depend on the kernel version.
             *
             *  (10)    minflt %lu
             *          The number of minor faults the process has made which have not required loading a memory page from disk.
             *
             *  (11)    cminflt %lu
             *          The number of minor faults that the process's waited-for children have made.
             *
             *  (12)    majflt %lu
             *          The number of major faults the process has made which have required loading a memory page from disk.
             *
             *  (13)    cmajflt %lu
             *          The number of major faults that the process's waited-for children have made.
             *
             *  (14)    utime %lu
             *          Amount of time that this process has been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)). This includes guest time, guest_time (time spent running a virtual CPU, see below), so that applications that are not aware of the guest time field do not lose that time from their calculations.
             *
             *  (15)    stime %lu
             *          Amount of time that this process has been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (16)    cutime %ld
             *          Amount of time that this process's waited-for children have been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)). (See also times(2).) This includes guest time, cguest_time (time spent running a virtual CPU, see below).
             *
             *  (17)    cstime %ld
             *          Amount of time that this process's waited-for children have been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (18)    priority %ld
             *          (Explanation for Linux 2.6) For processes running a real-time scheduling policy (policy below; see sched_setscheduler(2)), this is the negated scheduling priority, minus one; that is, a number in the range -2 to -100, corresponding to real-time priorities 1 to 99. For processes running under a non-real-time scheduling policy, this is the raw nice value (setpriority(2)) as represented in the kernel. The kernel stores nice values as numbers in the range 0 (high) to 39 (low), corresponding to the user-visible nice range of -20 to 19.
             *          Before Linux 2.6, this was a scaled value based on the scheduler weighting given to this process.
             *
             *  (19)    nice %ld
             *          The nice value (see setpriority(2)), a value in the range 19 (low priority) to -20 (high priority).
             *
             *  (20)    num_threads %ld
             *          Number of threads in this process (since Linux 2.6). Before kernel 2.6, this field was hard coded to 0 as a placeholder for an earlier removed field.
             *
             *  (21)    itrealvalue %ld
             *          The time in jiffies before the next SIGALRM is sent to the process due to an interval timer. Since kernel 2.6.17, this field is no longer maintained, and is hard coded as 0.
             *
             *  (22)    starttime %llu (was %lu before Linux 2.6)
             *          The time the process started after system boot. In kernels before Linux 2.6, this value was expressed in jiffies. Since Linux 2.6, the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (23)    vsize %lu
             *          Virtual memory size in bytes.
             *
             *          from ps docs:
             *              size in physical pages of the core image of the process.  This includes text, data, and stack space.  Device mappings are currently excluded;
             *          Empirically thats waht this appears to be.
             *
             *  (24)    rss %ld
             *          Resident Set Size: number of pages the process has in real memory. This is just the pages which count toward text, data, or stack space. This does not include pages which have not been demand-loaded in, or which are swapped out.
             *
             *  (25)    rsslim %lu
             *          Current soft limit in bytes on the rss of the process; see the description of RLIMIT_RSS in getrlimit(2).
             *
             *  (26)    startcode %lu
             *          The address above which program text can run.
             *
             *  (27)    endcode %lu
             *          The address below which program text can run.
             *
             *  (28)    startstack %lu
             *          The address of the start (i.e., bottom) of the stack.
             *
             *  (29)    kstkesp %lu
             *          The current value of ESP (stack pointer), as found in the kernel stack page for the process.
             *
             *  (30)    kstkeip %lu
             *          The current EIP (instruction pointer).
             *
             *  (31)    signal %lu
             *          The bitmap of pending signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (32)    blocked %lu
             *          The bitmap of blocked signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (33)    sigignore %lu
             *          The bitmap of ignored signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (34)    sigcatch %lu
             *          The bitmap of caught signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (35)    wchan %lu
             *          This is the "channel" in which the process is waiting. It is the address of a system call, and can be looked up in a namelist if you need a textual name. (If you have an up-to-date /etc/psdatabase, then try ps -l to see the WCHAN field in action.)
             *
             *  (36)    nswap %lu
             *          Number of pages swapped (not maintained).
             *
             *  (37)    cnswap %lu
             *          Cumulative nswap for child processes (not maintained).
             *
             *  (38)    exit_signal %d (since Linux 2.1.22)
             *          Signal to be sent to parent when we die.
             *
             *  (39)    processor %d (since Linux 2.2.8)
             *          CPU number last executed on.
             *
             *  (40)    rt_priority %u (since Linux 2.5.19; was %lu before Linux 2.6.22)
             *          Real-time scheduling priority, a number in the range 1 to 99 for processes scheduled under a real-time policy, or 0, for non-real-time processes (see sched_setscheduler(2)).
             *
             *  (41)    policy %u (since Linux 2.5.19; was %lu before Linux 2.6.22)
             *          Scheduling policy (see sched_setscheduler(2)). Decode using the SCHED_* constants in linux/sched.h.
             *
             *  (42)    delayacct_blkio_ticks %llu (since Linux 2.6.18)
             *          Aggregated block I/O delays, measured in clock ticks (centiseconds).
             *
             *  (43)    guest_time %lu (since Linux 2.6.24)
             *          Guest time of the process (time spent running a virtual CPU for a guest operating system), measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (44)    cguest_time %ld (since Linux 2.6.24)
             *          Guest time of the process's children, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             */
            char                state;          //  (3)
            int                 ppid;           //  (4)
            unsigned long long  utime;          //  (14)
            unsigned long long  stime;          //  (15)
            int                 nlwp;           //  (20)
            unsigned long long  start_time;     //  (22)
            unsigned long long  vsize;          //  (23)
            unsigned long long  rss;            //  (24)
            unsigned  long  minflt;
            unsigned  long  majflt;
        };
        StatFileInfo_   ReadStatFile_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadStatFile_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif
            StatFileInfo_    result {};
            Streams::InputStream<Byte>   in = FileInputStream::mk (fullPath, FileInputStream::eNotSeekable);
            Byte    data[10 * 1024];
            size_t nBytes = in.Read (begin (data), end (data));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("nBytes read = %d", nBytes);
#endif

            const char* S = reinterpret_cast<const char*> (data);
            {
                ///@TODO - FIX - THIS CODE UNSAFE - CAN CRASH! what if S not nul-terminated!
                S = ::strchr (S, '(') + 1;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S = %x", S);
#endif
                const char* tmp = ::strrchr (S, ')');
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S(tmp) = %x", tmp);
#endif
                S = tmp + 2;                 // skip ") "
            }

            // MSVC SILLY WARNING ABOUT USING swscanf_s
            // (warning doesnt appear to check if we have mismatch between types and format args provided.
            //      --LGP 2015-01-07
            DISABLE_COMPILER_MSC_WARNING_START(4996)
            int                 ignoredInt = 0;
            long                ignoredLong = 0;
            unsigned long       ignoredUnsignedLong = 0;
            unsigned long long  ignoredUnsignedLongLong = 0;
            unsigned long int   ignored_unsigned_long {};
            int num = ::sscanf (
                          S,

                          // (3 - char state)...
                          "%c "

                          // (4 - 'int' - ppid,pgrp,session,tty_nr,tpgid)...
                          "%d %d %d %d %d "

                          // (9 unsigned long  - flags,minflt,cminflt,majflt,cmajflt - NB: flags now unsigned int not unsigned long but unsigned long fits new and old)...
                          "%lu %lu %lu %lu %lu "

                          // (14 - unint but use ulonglong for safety - utime stime)...
                          "%llu %llu "

                          // (16 - unint but use ulonglong for safety- cutime cstime - docs say signed int but thats crazy--LGP2015-09-16)...
                          "%llu %llu "

                          // (18 long priority, nice)...
                          "%ld %ld "

                          // (20  docs say long but thats nuts %ld   num_threads)...
                          "%d "

                          // (21  %ld - itrealvalue)...
                          "%d "

                          // (22 llu -   starttime %llu)...
                          "%llu "

                          // (23 unsigned long by docs but use ull   vsize, rss)...
                          "%llu %llu "
                          ,

                          // (3 - char state)...
                          &result.state,

                          // (4 - 'int' - ppid,pgrp,session,tty_nr,tpgid)...
                          &result.ppid, &ignoredInt, &ignoredInt, &ignoredInt, &ignoredInt,

                          // (9 unsigned long - flags,minflt,cminflt,majflt,cmajflt - NB: flags now unsigned int not unsigned long but unsigned long fits new and old)...
                          &ignoredUnsignedLong, &result.minflt, &ignoredUnsignedLong, &result.majflt, &ignoredUnsignedLong,

                          // (14 - unint but use ulonglong for safety - utime stime)...
                          &result.utime, &result.stime,

                          // (16 - unint but use ulonglong for safety- cutime cstime - docs say signed int but thats crazy--LGP2015-09-16)...
                          &ignoredUnsignedLongLong, &ignoredUnsignedLongLong,

                          // (18 long priority, nice)
                          &ignoredLong, &ignoredLong,

                          // (20  docs say long but thats nuts %ld   num_threads)
                          &result.nlwp,

                          // (21  %ld - itrealvalue)
                          &ignoredInt,

                          // (22 llu -   starttime %llu)...
                          &result.start_time,

                          // (23 unsigned long by docs but use ull   vsize, rss)...
                          &result.vsize,  &result.rss
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
        struct  proc_io_data_ {
            uint64_t    read_bytes;
            uint64_t    write_bytes;
        };
        Optional<proc_io_data_>   Readproc_io_data_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::Readproc_io_data_");
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

        Optional<MemorySizeType>   ReadPrivateBytes_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadPrivateBytes_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif

            if (not IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Skipping read cuz no access");
#endif
                return Optional<MemorySizeType> ();
            }
            MemorySizeType    result {};
            ifstream r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf [0] = '\0';
                if (r.getline (buf, sizeof(buf))) {
                    // I think always in KB
                    const char kPrivate1Lbl_ [] = "Private_Clean:";
                    const char kPrivate2Lbl_ [] = "Private_Dirty:";
                    // @todo - SHOULD pay attention to the labelm after the number. It may not always be kB? BUt not sure what it can be
                    if (strncmp (buf, kPrivate1Lbl_, strlen (kPrivate1Lbl_)) == 0) {
                        result += Characters::CString::String2Int<MemorySizeType> (buf + strlen (kPrivate1Lbl_)) * 1024;
                    }
                    else if (strncmp (buf, kPrivate2Lbl_, strlen (kPrivate2Lbl_)) == 0) {
                        result += Characters::CString::String2Int<MemorySizeType> (buf + strlen (kPrivate2Lbl_)) * 1024;
                    }
                }
            }
            return result;
        }

        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/PID/status'
        struct  proc_status_data_ {
            uid_t ruid;
        };
        proc_status_data_   Readproc_proc_status_data_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::Readproc_proc_status_data_");
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
                        int ruid = ::strtol (S, &S, 10);
                        int euid = ::strtol (S, &S, 10);
                        int suid = ::strtol (S, &S, 10);
                        int fuid = ::strtol (S, &S, 10);
                        result.ruid = ruid;
                    }
                }
            }
            return result;
        }
        // consider using this as a backup if /procfs/ not present...
        ProcessMapType  capture_using_ps_ ()
        {
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::capture_using_ps_");
            ProcessMapType  result;
            using   Execution::ProcessRunner;
            /*
             *  Thought about STIME but too hard to parse???
             *
             *  EXAMPLE OUTPUT:
             *          $ ps -e -o "pid,ppid,s,time,rss,vsz,user,nlwp,cmd" | head
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
            constexpr   size_t  kVSZ_Idx_               { 5 };
            constexpr   size_t  kUser_Idx_              { 6 };
            constexpr   size_t  kThreadCnt_Idx_         { 7 };
            constexpr   size_t  kColCountIncludingCmd_  { 9 };
            ProcessRunner   pr (L"ps -A -o \"pid,ppid,s,time,rss,vsz,user,nlwp,cmd\"");
            Streams::MemoryStream<Byte>   useStdOut;
            pr.SetStdOut (useStdOut);
            pr.Run ();
            String out;
            Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
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
                processDetails.fPrivateVirtualMemorySize =  Characters::String2Int<int> (l[kVSZ_Idx_].Trim ()) * 1024;
                processDetails.fUserName = l[kUser_Idx_].Trim ();
                processDetails.fThreadCount =  Characters::String2Int<unsigned int> (l[kThreadCnt_Idx_].Trim ());
                String  cmdLine;
                {
                    // wrong - must grab EVERYHTING from i past a certain point
                    // Since our first line has headings, its length is our target, minus the 3 chars for CMD
                    const size_t kCmdNameStartsAt_ = headerLen - 3;
                    cmdLine = i.size () <= kCmdNameStartsAt_ ? String () : i.SubString (kCmdNameStartsAt_).RTrim ();
                }
                {
                    processDetails.fKernelProcess = not cmdLine.empty () and cmdLine[0] == '[';
                    // Fake but usable answer
                    Sequence<String>    t    =  cmdLine.Tokenize ();
                    if (not t.empty () and not t[0].empty () and t[0][0] == '/') {
                        processDetails.fEXEPath = t[0];
                    }
                }
                if (fOptions_.fCaptureCommandLine and fOptions_.fCaptureCommandLine (pid, processDetails.fEXEPath.Value ())) {
                    processDetails.fCommandLine = cmdLine;
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
            fLastCapturedAt = Time::GetTickCount ();
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
            fLastCapturedAt = Time::GetTickCount ();
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

            SetPrivilegeInContext s (SE_DEBUG_NAME, SetPrivilegeInContext::eIgnoreError);
            ProcessMapType  results;

#if   qUseWMICollectionSupport_
            Mapping<String, double> threadCounts_ByPID          =   fProcessWMICollector_.GetCurrentValues (kThreadCount_);
            Mapping<String, double> ioReadBytesPerSecond_ByPID  =   fProcessWMICollector_.GetCurrentValues (kIOReadBytesPerSecond_);
            Mapping<String, double> ioWriteBytesPerSecond_ByPID =   fProcessWMICollector_.GetCurrentValues (kIOWriteBytesPerSecond_);
            Mapping<String, double> pctProcessorTime_ByPID      =   fProcessWMICollector_.GetCurrentValues (kPercentProcessorTime_);
            Mapping<String, double> processStartAt_ByPID        =   fProcessWMICollector_.GetCurrentValues (kElapsedTime_);
#endif

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
                            LookupProcessPath_ (pid, hProcess,  &processEXEPath, &parentProcessID, fOptions_.fCaptureCommandLine ? &cmdLine : nullptr, &userName);
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
                                processInfo.fPageFaultCount = memInfo.PageFaultCount;   // docs not 100% clear but I think this is total # pagefaults
                            }
                        }
                    }
                }
#if   qUseWMICollectionSupport_
                {
                    String instanceVal = pid2InstanceMap.LookupValue (pid);
                    if (auto o = threadCounts_ByPID.Lookup (instanceVal)) {
                        processInfo.fThreadCount = static_cast<unsigned int> (*o);
                    }
                    if (auto o = ioReadBytesPerSecond_ByPID.Lookup (instanceVal)) {
                        processInfo.fCombinedIOReadRate = *o;
                    }
                    if (auto o = ioWriteBytesPerSecond_ByPID.Lookup (instanceVal)) {
                        processInfo.fCombinedIOWriteRate = *o;
                    }
                    if (auto o = pctProcessorTime_ByPID.Lookup (instanceVal)) {
                        processInfo.fPercentCPUTime = *o;
                    }
                    if (grabStaticData) {
                        if (auto o = processStartAt_ByPID.Lookup (instanceVal)) {
                            processInfo.fProcessStartedAt = DateTime::Now ().AddSeconds (-static_cast<time_t> (*o));
                        }
                    }
                }
#endif
                results.Add (pid, processInfo);
            }
            fLastCapturedAt = Time::GetTickCount ();
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
        void    LookupProcessPath_ (pid_t pid, HANDLE hProcess, Optional<String>* processEXEPath, Optional<pid_t>* parentProcessID, Optional<String>* cmdLine, Optional<String>* userName)
        {
            RequireNotNull (hProcess);
            RequireNotNull (processEXEPath);
            RequireNotNull (parentProcessID);
            //CANBENULL (cmdLine);
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
            if (cmdLine != nullptr) {
                if (fOptions_.fCaptureCommandLine == nullptr or not fOptions_.fCaptureCommandLine (pid, processEXEPath->Value ())) {
                    cmdLine = nullptr;
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

                        if (cmdLine != nullptr) {
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


const   MeasurementType SystemPerformance::Instruments::Process::kProcessMapMeasurement = MeasurementType { String_Constant { L"Process-Details" } };






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
            Measurement     m { kProcessMapMeasurement, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info  Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType    before = fCaptureContext.fLastCapturedAt;
            Info rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.fLastCapturedAt);
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
 ******************* Instruments::Process::GetInstrument ************************
 ********************************************************************************
 */
Instrument          SystemPerformance::Instruments::Process::GetInstrument (const Options& options)
{
    return Instrument (
               InstrumentNameType (String_Constant { L"Process" }),
#if     qCompilerAndStdLib_make_unique_Buggy
               Instrument::SharedByValueCaptureRepType (unique_ptr<MyCapturer_> (new MyCapturer_ (CapturerWithContext_ { options }))),
#else
               Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_ { options })),
#endif
    {kProcessMapMeasurement},
    GetObjectVariantMapper ()
           );
}




/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template    <>
Instruments::Process::Info   SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
