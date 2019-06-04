/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Process_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Process_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Configuration/Enumeration.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../../Foundation/Execution/Process.h"

#include "../Instrument.h"

/*
 *
 *  TODO:
 *      @todo   In POSIX/fAllowUse_PS mode add or fix support for Command-Line (done badly), EXE-Path, Process-Started-At,
 *              Percent-CPUTime-Used.
 *
 *      @todo   Consider replacing fIncluidePIDs and fOMitPIDs with FilterFunctionType...
 */

namespace Stroika::Frameworks::SystemPerformance::Instruments::Process {

    using Foundation::Containers::Mapping;
    using Foundation::DataExchange::ObjectVariantMapper;
    using Foundation::Execution::pid_t;
    using Foundation::Time::DurationSecondsType;

    using MemorySizeType = uint64_t;

    /**
     *  Based closely on http://en.wikipedia.org/wiki/Procfs
     */
    struct ProcessType {
        /**
         *  If true (unix only) - this process is really a thread builtin to the kernel. If missing, unknown.
         */
        optional<bool> fKernelProcess;

        /**
         *  This will be omitted if unknown, but can be the special value zero, whose meaning varies from OS to OS, but
         *  typically means created by the system sponaneously (like the init process on UNIX has parent process id 0).
         *
         *  Kernel processes on some operating systems (e.g. AIX) also tend to have parent PID 0)
         */
        optional<pid_t> fParentProcessID;

        /*
         *  On some systems, in some cases, we cannot find the EXE name, but still have some sort of process
         *  name we can retrieve.
         */
        optional<String> fProcessName;

        /*
         *   on windows, this is in the form of username@domain if a domain is present
         */
        optional<String>                  fUserName;
        optional<String>                  fCommandLine;
        optional<String>                  fCurrentWorkingDirectory;
        optional<Mapping<String, String>> fEnvironmentVariables;
        optional<String>                  fEXEPath;
        optional<String>                  fRoot; // chroot
        optional<Time::DateTime>          fProcessStartedAt;

        /**
         *  Based on
         *      http://linux.die.net/man/5/proc (search for /proc/[pid]/stat)
         *          One character from the string "RSDZTW" where R is running, S is sleeping in
         *          an interruptible wait, D is waiting in uninterruptible disk sleep, Z is zombie,
         *          T is traced or stopped (on a signal), and W is paging.
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class RunStatus {
            eRunning,
            eSleeping,
            eWaitingOnDisk,
            eWaitingOnPaging,
            eZombie,
            eSuspended, //  T is traced or stopped (on a signal)

            Stroika_Define_Enum_Bounds (eRunning, eSuspended)
        };

        /**
         */
        optional<RunStatus> fRunStatus;

        /**
         *  This is the total VM allocated solely for the purpose of this process.
         *  This includes data space, SHOULD (but may not include)stack space for threads etc, and
         *  heap.
         *
         *  On UNIX, this corresponds to VSZ in ps, and top.
         *  For now, this is not supported in Windows.
         *
         *  @see fPrivateBytes
         */
        optional<MemorySizeType> fPrivateVirtualMemorySize;

        /**
         *  This is the total VM size for the process, including all mapped shared data. This value will
         *  often grossly over-state the amount of 'virtual memory' in use for a process, because
         *  it can include things like memory mapped files, etc.
         */
        optional<MemorySizeType> fTotalVirtualMemorySize;

        /**
         *  Resident Set Size (RSS): number of [BYTES] the process has in real memory. This is just the
         *  pages which count toward text, data, or stack space. This does not include pages which have not
         *  been demand-loaded in, or which are swapped out.
         *
         *  @todo   DECIDE IF
         *          This  does NOT include 'shared' memory (e.g. for mapped .so files)
         *          SEEN CONTRADICTORY INFO.
         *          RATIONALIZE AND PROIVIDE BOTH (optionally)
         *
         *  This value is commonly used/available in UNIX.
         *
         *  \note   AIX lipperf:  proc_real_mem_data + proc_real_mem_text
         */
        optional<MemorySizeType> fResidentMemorySize;

        /*
         *  From http://superuser.com/questions/618686/private-bytes-vs-working-set-in-process-explorer:
         *  Private Bytes refers to the amount of Page file space that is allocated to the process
         *  (not necessarily used) in the event that the process's private memory footprint is completely
         *  paged out to swap. most of the time, the process is not entirely (or at all) page-file resident,
         *  so that's why private bytes appears to have "room" for further allocation. It is not however the case.
         *
         *  Private bytes however only refers to the processes private memory, so this value may not reflect
         *  shared resources (even if the shared resource is only used by this process at present).
         *
         *  This value is commonly used/available in Windows
         *
         *  For Linux:
         *      This is all the private area from  /proc/<PID>/smaps with the label Private_Clean: or Private_Dirty
         *
         *  For AIX:
         *      This is lipperf psinfo proc_size minus proc_virt_mem_text (so all VM that is process specific, less text space VM).
         *
         *  @see fPrivateVirtualMemorySize
         */
        optional<MemorySizeType> fPrivateBytes;

        /**
         *  Total number of page major (causing a block/disk read)
         */
        optional<unsigned int> fMajorPageFaultCount;

        /**
         *  Total number of page faults (read or write) ever for this process.
         */
        optional<unsigned int> fPageFaultCount;

        /**
         *  @todo better document, and only implemented for windows - but need todo for unix
         */
        optional<MemorySizeType> fWorkingSetSize;

        /**
         *  @todo support in Windows AND UNIX - WIndows must use WMI - I think - and
         *  @todo DOCUMENT
         */
        optional<MemorySizeType> fPrivateWorkingSetSize;

        /**
         *  Average CPU time used / second over this collection interval. This when available - is logically
         *  fTotalCPUTimeEverUsed-PREV.fTotalCPUTimeEverUsed)/measurement_time;
         *
         *  So - if you have two cores running constantly, this returns 2.0;
         */
        optional<DurationSecondsType> fAverageCPUTimeUsed;

        /**
         *     In seconds - combines system and user time, and is NOT a time over the interval, but rather is
         *     the total (user + system) usage of the process since it started.
         *
         *     This is in units of a single CPU, so if you have a 2 CPU system running flat out for 3 seconds,
         *     this number would be 6 (2 * 3).
         */
        optional<DurationSecondsType> fTotalCPUTimeEverUsed;

        /**
         */
        optional<unsigned int> fThreadCount;

        /**
         *  Rate in bytes per second.
         *  This is summed accross all IO devices, including disk and network.
         */
        optional<double> fCombinedIOReadRate;

        /*
         *  Rate in bytes per second
         *  This is summed accross all IO devices, including disk and network.
         */
        optional<double> fCombinedIOWriteRate;

        /**
         *  See https://www.kernel.org/doc/Documentation/filesystems/proc.txt
         *  search for 'read_bytes'
         *
         *  This reflects the total number of bytes read over the process lifetime.
         *
         *  This (I believe) - includes paging reads/writes.
         *
         *  @todo make sure This is summed accross all IO devices, including disk and network
         */
        optional<double> fCombinedIOReadBytes;

        /**
         *  See https://www.kernel.org/doc/Documentation/filesystems/proc.txt
         *  search for 'write_bytes'
         *
         *  This reflects the total number of bytes written over the process lifetime.
         *
         *  This (I believe) - includes paging reads/writes.
         *
         *  @todo make sure This is summed accross all IO devices, including disk and network
         */
        optional<double> fCombinedIOWriteBytes;

        /**
         *  Where available, check the number of TCP streams associated with this process - and
         *  separately count established, listening, and other (usually shutting down)
         *  TCP streams.
         *
         *  \note   @todo https://stroika.atlassian.net/browse/STK-478  - fTCPStats for whole computer not process
         */
        struct TCPStats {
            unsigned int fEstablished{};
            unsigned int fListening{};
            unsigned int fOther{};
        };
        optional<TCPStats> fTCPStats;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     */
    using ProcessMapType = Mapping<pid_t, ProcessType>;

    /**
     */
    using Info = ProcessMapType;

    /**
     *  This instrument produce a measurement of type kProcessMapMeasurement, whcih can be converted to ProcessMapType.
     *
     *  \par Example Usage
     *      \code
     *       for (Measurement m : ms.fMeasurements) {
     *          if (m.fType == SystemPerformance::Instruments::Process::kProcessMapMeasurement) {
     *              AccumulateMeasurement_Process_ (m);
     *          }
     *      \endcode
     */
    extern const MeasurementType kProcessMapMeasurement;

    /**
     *  For ProcessType and ProcessMapType types.
     */
    ObjectVariantMapper GetObjectVariantMapper ();

    /**
     */
    enum class CachePolicy {
        eOmitUnchangedValues,
        eIncludeAllRequestedValues,
    };

    /**
     */
    struct Options {
        /**
         *  If FilterFunctionType is nullptr, then treat this as false.
         */
        using FilterFunctionType = function<bool (pid_t pid, const String& processPath)>;

        /**
         *  \req fMinimumAveragingInterval >= 0
         */
        Time::DurationSecondsType fMinimumAveragingInterval{1.0};

        /*
         * Assign nullptr to disable commandline capture.
         */
        FilterFunctionType fCaptureCommandLine{[] (pid_t /*pid*/, const String & /*processPath*/) -> bool { return true; }};

        bool                 fCaptureEnvironmentVariables{true};
        bool                 fCaptureCurrentWorkingDirectory{true};
        bool                 fCaptureRoot{true};
        bool                 fCaptureTCPStatistics{false};
        optional<Set<pid_t>> fRestrictToPIDs;
        optional<Set<pid_t>> fOmitPIDs;
        CachePolicy          fCachePolicy{CachePolicy::eIncludeAllRequestedValues};

        enum ProcessNameReadPolicy {
            eNever,
            eOnlyIfEXENotRead,
            eAlways
        };
        ProcessNameReadPolicy fProcessNameReadPolicy{eOnlyIfEXENotRead};
#if qPlatform_POSIX
        bool fAllowUse_ProcFS{true};
        bool fAllowUse_PS{true};
#elif qPlatform_Windows
//PERHAPS SUPPORT IN FUTURE?
//bool                          fAllowUse_WMI                   { true };
#endif
    };

    /**
     *  Instrument returning ProcessMapType measurements.
     */
    Instrument GetInstrument (const Options& options = Options ());

}

namespace Stroika::Frameworks::SystemPerformance {

    /*
     *  Specialization to improve performance
     */
    template <>
    Instruments::Process::Info Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus> DefaultNames<Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus>::k{
        EnumNames<Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus>::BasicArrayInitializer{
            {
                {Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus::eRunning, L"Running"},
                {Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus::eSleeping, L"Sleeping"},
                {Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus::eWaitingOnDisk, L"WaitingOnDisk"},
                {Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus::eWaitingOnPaging, L"WaitingOnPaging"},
                {Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus::eZombie, L"Zombie"},
                {Frameworks::SystemPerformance::Instruments::Process::ProcessType::RunStatus::eSuspended, L"Suspended"},
            }}};
}
#endif /*_Stroika_Framework_SystemPerformance_Instruments_Process_h_*/
