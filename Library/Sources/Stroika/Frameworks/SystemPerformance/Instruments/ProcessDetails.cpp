/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Characters/StringBuilder.h"
#include    "../../../Foundation/Characters/Tokenize.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/Execution/ThreadAbortException.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "../../../Foundation/IO/FileSystem/FileSystem.h"
#include    "../../../Foundation/Memory/BLOB.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Streams/BufferedBinaryInputStream.h"

#include    "../CommonMeasurementTypes.h"

#include    "ProcessDetails.h"


// Comment this in to turn on aggressive noisy DbgTrace in this module
#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



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




const   MeasurementType Instruments::ProcessDetails::kProcessMapMeasurement = MeasurementType (String_Constant (L"Process-Details"));

//tmphack to test
//#define qUseProcFS_ 1

#ifndef     qUseProcFS_
#define     qUseProcFS_ qPlatform_POSIX
#endif



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
        mapper.AddCommonType<Optional<String>> ();
        mapper.AddCommonType<Optional<Time::DateTime>> ();
        mapper.AddCommonType<Optional<Time::DurationSecondsType>> ();
        mapper.AddCommonType<Optional<Mapping<String, String>>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<ProcessType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCommandLine), String_Constant (L"Command-Line"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCurrentWorkingDirectory), String_Constant (L"Current-Working-Directory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEnvironmentVariables), String_Constant (L"Environment-Variables"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEXEPath), String_Constant (L"EXE-Path"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fRoot), String_Constant (L"Root"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fProcessStartedAt), String_Constant (L"Process-Started-At"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fTotalTimeUsed), String_Constant (L"Total-Time-Used"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<ProcessMapType> ();
        return mapper;
    } ();
    return sMapper_;
}



namespace {
    // this reads /proc format files - meaning that a trialing nul-byte is the EOS
    String  ReadFileString_(const String& fullPath)
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (SDKSTR ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ReadFileString_"));
#endif
        Streams::BinaryInputStream   in = Streams::BufferedBinaryInputStream (IO::FileSystem::BinaryFileInputStream (fullPath));
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
    Sequence<String>  ReadFileStrings_(const String& fullPath)
    {
        Sequence<String>    results;
        Streams::BinaryInputStream   in = Streams::BufferedBinaryInputStream (IO::FileSystem::BinaryFileInputStream (fullPath));
        StringBuilder sb;
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
        for (auto i : ReadFileStrings_ (fullPath)) {
            auto tokens = Tokenize<String> (i, L"=");
            if (tokens.size () == 2) {
                results.Add (tokens[0], tokens[1]);
            }
        }
        return results;
    }
    struct StatFileInfo_ {
        //@todo REDO BASED on http://linux.die.net/man/5/proc,  search for '/proc/[pid]/stat'


        // trim down and find better source - but for now use 'procps-3.2.8\proc\'
        int ppid;
        char state;     // stat,status     single-char code for process state (S=sleeping)
        unsigned long long    utime;      // stat            user-mode CPU time accumulated by process
        unsigned long long stime;     // stat            kernel-mode CPU time accumulated by process
        unsigned long long    cutime;     // stat            cumulative utime of process and reaped children
        unsigned long long cstime;        // stat            cumulative stime of process and reaped children
        unsigned long  start_time;    // stat            start time of process -- seconds since 1-1-70

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
        Debug::TraceContextBumper ctx (SDKSTR ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ReadStatFile_"));
        DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif
        StatFileInfo_    result {};
        Streams::BinaryInputStream   in = Streams::BufferedBinaryInputStream (IO::FileSystem::BinaryFileInputStream (fullPath));
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
            DbgTrace ("S = %x", S);
#endif
            S = tmp + 2;                 // skip ") "
        }

        int num = sscanf(S,
                         "%c "
                         "%d %d %d %d %d "
                         "%lu %lu %lu %lu %lu "
                         "%llu %llu %llu %llu "  /* utime stime cutime cstime */
                         "%ld %ld "
                         "%d "
                         "%ld "
                         "%llu "  /* start_time */
#if 0
                         /*
                         "%lu "
                         "%ld "
                         "%lu %"KLF"u %"KLF"u %"KLF"u %"KLF"u %"KLF"u "
                         "%*s %*s %*s %*s " /* discard, no RT signals & Linux 2.1 used hex */
                         "%"KLF"u %*lu %*lu "
                         "%d %d "
                         "%lu %lu"
                         * /
#endif
                         ,
                         &result.state,
                         &result.ppid, &result.pgrp, &result.session, &result.tty, &result.tpgid,
                         &result.flags, &result.min_flt, &result.cmin_flt, &result.maj_flt, &result.cmaj_flt,
                         &result.utime, &result.stime, &result.cutime, &result.cstime,
                         &result.priority, &result.nice,
                         &result.nlwp,
                         &result.alarm,
                         &result.start_time
#if 0
                         & result.vsize,
                         &result.rss,
                         &result.rss_rlim, &result.start_code, &result.end_code, &result.start_stack, &result.kstk_esp, &result.kstk_eip,
                         /*     P->signal, P->blocked, P->sigignore, P->sigcatch,   */ /* can't use */
                         &result.wchan, /* &P->nswap, &P->cnswap, */  /* nswap and cnswap dead for 2.4.xx and up */
                         /* -- Linux 2.0.35 ends here -- */
                         &result.exit_signal, &result.processor,  /* 2.2.1 ends with "exit_signal" */
                         /* -- Linux 2.2.8 to 2.5.17 end here -- */
                         &result.rtprio, &result.sched  /* both added to 2.5.18 */
#endif
                        );


        return result;
    }

    ProcessMapType  ExtractFromProcFS_ ()
    {
        /// Most status - like time - come from http://linux.die.net/man/5/proc
        ///proc/[pid]/stat
        //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
        //
        ProcessMapType  results;

#if     qUseProcFS_
        for (String dir : IO::FileSystem::DirectoryIterable (String_Constant (L"/proc"))) {
            bool isAllNumeric = not dir.FindFirstThat ([] (Character c) -> bool { return not c.IsDigit (); });
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx (SDKSTR ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ExtractFromProcFS_::reading proc files"));
            DbgTrace (L"isAllNumeric=%d, dir= %s", isAllNumeric, dir.c_str ());
#endif
            if (isAllNumeric) {
                pid_t pid = String2Int<pid_t> (dir);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("reading for pid = %d", pid);
#endif
                String  processDirPath = String_Constant (L"/proc/") + dir + String_Constant (L"/");
                ProcessType processDetails;
                IgnoreExceptionsExceptThreadAbortForCall (processDetails.fCommandLine = ReadFileString_ (processDirPath + String_Constant (L"cmdline")));
                IgnoreExceptionsExceptThreadAbortForCall (processDetails.fCurrentWorkingDirectory = IO::FileSystem::FileSystem::Default ().ResolveShortcut (processDirPath + String_Constant (L"cwd")));
                IgnoreExceptionsExceptThreadAbortForCall (processDetails.fEnvironmentVariables = ReadFileStringsMap_ (processDirPath + String_Constant (L"environ")));
                IgnoreExceptionsExceptThreadAbortForCall (processDetails.fEXEPath = IO::FileSystem::FileSystem::Default ().ResolveShortcut (processDirPath + String_Constant (L"exe")));
                IgnoreExceptionsExceptThreadAbortForCall (processDetails.fRoot = IO::FileSystem::FileSystem::Default ().ResolveShortcut (processDirPath + String_Constant (L"root")));

                try {
                    static  const   double  kClockTick_ = sysconf(_SC_CLK_TCK);
                    StatFileInfo_   stats    =  ReadStatFile_ (processDirPath + String_Constant (L"stat"));
                    processDetails.fProcessStartedAt = DateTime (static_cast<time_t> (stats.start_time));
                    processDetails.fTotalTimeUsed = double (stats.utime) + double (stats.stime ) / kClockTick_;
                }
                catch (...) {
                }
                results.Add (pid, processDetails);
            }
        }
#else
        ProcessType test;
        test.fCommandLine = L"Hi mom comamndline";
        Mapping<String, String> env;
        env.Add (L"Home", L"/home/lewis");
        test.fEnvironmentVariables = env;
        results.Add (101, test);
#endif
        return results;
    }
}





/*
 ********************************************************************************
 ************* Instruments::ProcessDetails::GetInstrument **********************
 ********************************************************************************
 */
Instrument          SystemPerformance::Instruments::ProcessDetails::GetInstrument (
    const Optional<Set<Fields2Capture>>& onlyCaptureFields,
    const Optional<Set<pid_t>>& restrictToPIDs,
    const Optional<Set<pid_t>>& omitPIDs,
    CachePolicy cachePolicy
)
{
    // @todo can only use static one if right options passed in...
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"ProcessDetails")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        auto rawMeasurement = ExtractFromProcFS_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kProcessMapMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kProcessMapMeasurement}
                                          );
    return kInstrument_;
}
