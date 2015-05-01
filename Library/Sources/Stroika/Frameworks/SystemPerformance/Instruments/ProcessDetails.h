/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_ProcessDetails_h_
#define _Stroika_Framework_SystemPerformance_Instruments_ProcessDetails_h_ 1

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
 *      @todo   Eventually allow configuration of whether to use procfs or ps process grep, but default to
 *              procfs is available.
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace ProcessDetails {


                    using   Foundation::Containers::Mapping;
                    using   Foundation::DataExchange::ObjectVariantMapper;
                    using   Foundation::Execution::pid_t;
                    using   Foundation::Memory::Optional;
                    using   Foundation::Time::DurationSecondsType;


                    using   MemorySizeType = uint64_t;


                    /*
                     *  Based closely on http://en.wikipedia.org/wiki/Procfs
                     */
                    struct  ProcessType {
                        Optional<pid_t>                     fParentProcessID;
                        Optional<String>                    fUserName;          // on windows, this is in the form of username@domain if a domain is present
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

                        Optional<RunStatus>                 fRunStatus;
                        Optional<MemorySizeType>            fVirtualMemorySize;

                        /**
                         *  Resident Set Size (RSS): number of [BYTES] the process has in real memory. This is just the
                         *  pages which count toward text, data, or stack space. This does not include pages which have not
                         *  been demand-loaded in, or which are swapped out.
                         *
                         *  This  does NOT include 'shared' memory (e.g. for mapped .so files)
                         *
                         *  This value is commonly used/available in UNIX
                         */
                        Optional<MemorySizeType>            fResidentMemorySize;

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
                         *  @todo This MAYBE windows only(?)
                         */
                        Optional<MemorySizeType>           fPrivateBytes;

                        /**
                         */
                        Optional<unsigned int> fPageFaultCount;

                        /**
                         */
                        Optional<MemorySizeType>   fWorkingSetSize;

                        /**
                         *    Percent (of a single???) CPU time used (summed over user / system)
                         */
                        Optional<DurationSecondsType>       fPercentCPUTime;

                        /**
                          *    in seconds - combines system and user time
                          */
                        Optional<DurationSecondsType>       fTotalCPUTimeUsed;

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


                    enum    class   Fields2Capture {
                        eCommandLine,
                        eCurrentWorkingDirectory,
                        eEnvironmentVariables,
                        /// many more TBD
                    };

                    /**
                     */
                    using ProcessMapType = Mapping<pid_t, ProcessType>;

                    /**
                     *  This instrument produce a measurement of type kProcessMapMeasurement, whcih can be converted to ProcessMapType.
                     *
                     *  EXAMPLE USAGE:
                     *       for (Measurement m : ms.fMeasurements) {
                     *          if (m.fType == SystemPerformance::Instruments::ProcessDetails::kProcessMapMeasurement) {
                     *              AccumulateMeasurement_ProcessDetails_ (m);
                     *          }
                     */
                    extern  const   MeasurementType kProcessMapMeasurement;

                    /**
                     *  For ProcessType and ProcessMapType types.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    enum    class   CachePolicy {
                        eOmitUnchangedValues,
                        eIncludeAllRequestedValues,
                    };


                    /**
                     */
                    struct  Options {
                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType       fMinimumAveragingInterval { 1.0 };

                        Optional<Set<Fields2Capture>>   fOnlyCaptureFields;
                        Optional<Set<pid_t>>            fRestrictToPIDs;
                        Optional<Set<pid_t>>            fOmitPIDs;
                        CachePolicy                     fCachePolicy { CachePolicy::eIncludeAllRequestedValues };

                        enum    Strategy {
#if     qPlatform_POSIX
                            eProcFS,
                            ePS,
#endif
#if     qPlatform_Windows
                            eWindows_EnumProcesses,
#endif
                        };

#if 0
                        //NYI on windows - init...
                        Containers::Sequence<Strategy>  fStrategiesToTry {
#if     qPlatform_POSIX
                            eProcFS,
                            ePS,
#endif
#if     qPlatform_Windows
                            eWindows_EnumProcesses
#endif
                        };
#endif
                    };


                    /**
                     *  Instrument returning ProcessMapType measurements.
                     */
                    Instrument  GetInstrument (const Options& options = Options ());


                }

            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_ProcessDetails_h_*/
