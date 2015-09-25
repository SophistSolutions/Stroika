/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Process_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Process_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Configuration/Enumeration.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Execution/Process.h"
#include    "../../../Foundation/Memory/Optional.h"

#include    "../Instrument.h"


/*
 *
 *  TODO:
 *      @todo   In POSIX/fAllowUse_PS mode add or fix support for Command-Line (done badly), EXE-Path, Process-Started-At,
 *              Percent-CPUTime-Used.
 *
 *      @todo   Consider replacing fIncluidePIDs and fOMitPIDs with FilterFunctionType...
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace Process {


                    using   Foundation::Containers::Mapping;
                    using   Foundation::DataExchange::ObjectVariantMapper;
                    using   Foundation::Execution::pid_t;
                    using   Foundation::Memory::Optional;
                    using   Foundation::Time::DurationSecondsType;


                    using   MemorySizeType = uint64_t;


                    /**
                     *  Based closely on http://en.wikipedia.org/wiki/Procfs
                     */
                    struct  ProcessType {
                        Optional<bool>                      fKernelProcess;     // if true (unix only) - this process is really a thread builtin to the kernel
                        Optional<pid_t>                     fParentProcessID;

                        /*
                         *  On some systems, in some cases, we cannot find the EXE name, but still have some sort of process
                         *  name we can retrieve.
                         */
                        Optional<String>                    fProcessName;

                        /*
                         *   on windows, this is in the form of username@domain if a domain is present
                         */
                        Optional<String>                    fUserName;
                        Optional<String>                    fCommandLine;
                        Optional<String>                    fCurrentWorkingDirectory;
                        Optional<Mapping<String, String>>   fEnvironmentVariables;
                        Optional<String>                    fEXEPath;
                        Optional<String>                    fRoot;  // chroot
                        Optional<Time::DateTime>            fProcessStartedAt;

                        /*
                         *  Based on
                         *      http://linux.die.net/man/5/proc (search for /proc/[pid]/stat)
                         *          One character from the string "RSDZTW" where R is running, S is sleeping in
                         *          an interruptible wait, D is waiting in uninterruptible disk sleep, Z is zombie,
                         *          T is traced or stopped (on a signal), and W is paging.
                         */
                        enum    class   RunStatus {
                            eRunning,
                            eSleeping,
                            eWaitingOnDisk,
                            eWaitingOnPaging,
                            eZombie,
                            eSuspended,         //  T is traced or stopped (on a signal)
                            Stroika_Define_Enum_Bounds(eRunning, eSuspended)
                        };
                        static  const Configuration::EnumNames<RunStatus>   Stroika_Enum_Names(RunStatus);

                        Optional<RunStatus>         fRunStatus;

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
                        Optional<MemorySizeType>    fPrivateVirtualMemorySize;

                        /**
                         *  This is the total VM size for the process, including all mapped shared data. This value will
                         *  often grossly over-state the amount of 'virtual memory' in use for a process, because
                         *  it can include things like memory mapped files, etc.
                         */
                        Optional<MemorySizeType>    fTotalVirtualMemorySize;

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
                        Optional<MemorySizeType>    fResidentMemorySize;

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
                        Optional<MemorySizeType>    fPrivateBytes;

                        /**
                         *  Total number of page major (causing a block/disk read)
                         */
                        Optional<unsigned int>      fMajorPageFaultCount;

                        /**
                         *  Total number of page faults (read or write) ever for this process.
                         */
                        Optional<unsigned int>      fPageFaultCount;

                        /**
                         *  @todo better document, and only implemented for windows - but need todo for unix
                         */
                        Optional<MemorySizeType>    fWorkingSetSize;

                        /**
                         *  @todo support in Windows AND UNIX - WIndows must use WMI - I think - and
                         *  @todo DOCUMENT
                         */
                        Optional<MemorySizeType>    fPrivateWorkingSetSize;

                        /**
                         *      Percent of a single CPU time used (summed over user / system) and average over this
                         *      collection interval.
                         */
                        Optional<DurationSecondsType>       fPercentCPUTime;

                        /**
                          *     In seconds - combines system and user time, and is NOT a time over the interval, but rather is
                          *     the total (user + system) usage of the process since it started.
                          *
                          *     This is in units of a single CPU, so if you have a 2 CPU system running flat out for 3 seconds,
                          *     this number would be 6 (2 * 3).
                          */
                        Optional<DurationSecondsType>       fTotalCPUTimeEverUsed;

                        /**
                         */
                        Optional<unsigned int>              fThreadCount;

                        /**
                         *  Rate in bytes per second.
                         *  This is summed accross all IO devices, including disk and network.
                         */
                        Optional<double>        fCombinedIOReadRate;

                        /*
                         *  Rate in bytes per second
                         *  This is summed accross all IO devices, including disk and network.
                         */
                        Optional<double>        fCombinedIOWriteRate;

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
                        Optional<double>        fCombinedIOReadBytes;

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
                        Optional<double>        fCombinedIOWriteBytes;
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
                    extern  const   MeasurementType kProcessMapMeasurement;

                    /**
                     *  For ProcessType and ProcessMapType types.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     */
                    enum    class   CachePolicy {
                        eOmitUnchangedValues,
                        eIncludeAllRequestedValues,
                    };


                    /**
                     */
                    struct  Options {
                        /**
                         *  If FilterFunctionType is nullptr, then treat this as false.
                         */
                        using   FilterFunctionType = function<bool(pid_t pid, const String& processPath)>;

                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType       fMinimumAveragingInterval       { 1.0 };

                        /*
                         * Assign nullptr to disable commandline capture.
                         */
                        FilterFunctionType              fCaptureCommandLine             { [] (pid_t pid, const String & processPath) -> bool { return true; } };

                        bool                            fCaptureEnvironmentVariables    { true };
                        bool                            fCaptureCurrentWorkingDirectory { true };
                        bool                            fCaptureRoot                    { true };
                        Optional<Set<pid_t>>            fRestrictToPIDs;
                        Optional<Set<pid_t>>            fOmitPIDs;
                        CachePolicy                     fCachePolicy                    { CachePolicy::eIncludeAllRequestedValues };

                        enum    ProcessNameReadPolicy {
                            eNever,
                            eOnlyIfEXENotRead,
                            eAlways
                        };
                        ProcessNameReadPolicy           fProcessNameReadPolicy          { eOnlyIfEXENotRead };
#if     qPlatform_POSIX
                        bool                            fAllowUse_ProcFS                { true };
                        bool                            fAllowUse_PS                    { true };
#elif   qPlatform_Windows
                        //PERHAPS SUPPORT IN FUTURE?
                        //bool                          fAllowUse_WMI                   { true };
#endif
                    };


                    /**
                     *  Instrument returning ProcessMapType measurements.
                     */
                    Instrument  GetInstrument (const Options& options = Options ());


                }
            }


            /*
             *  Specialization to improve performance
             */
            template    <>
            Instruments::Process::Info   Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_Process_h_*/
