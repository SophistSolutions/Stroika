/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Memory_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Memory_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace   Memory {


                    using   DataExchange::ObjectVariantMapper;
                    using   Foundation::Memory::Optional;


                    /**
                     *      @see https://www.centos.org/docs/5/html/5.1/Deployment_Guide/s2-proc-meminfo.html
                     */
                    struct  Info {
                        /**
                         *  The amount of physical RAM, left unused by the system (in bytes).
                         *
                         *  From Linux:
                         *      /proc/meminfo::MemFree
                         *
                         *  From Windows:
                         *      ::GlobalMemoryStatusEx (&statex), statex.ullAvailPhys
                         *
                         */
                        Optional<uint64_t>  fFreePhysicalMemory {};

                        /**
                         *     DEFINITION UNCLEAR (cross-platform):
                         *          But roughtly - this is the number of bytes of pagefile allocated + number of bytes of physical memory.
                         *
                         *  We use as our definition of this "Limit" from http://en.wikipedia.org/wiki/Commit_charge
                         *      Limit is the maximum possible value for Total; it is the sum of the current pagefile size plus the physical memory
                         *      available for pageable contents (this excludes RAM that is assigned to non-pageable areas).
                         *      The corresponding performance counter is called "Commit Limit".
                         *
                         *  From Linux:
                         *      /proc/meminfo::CommitLimit
                         *
                         *      Note - on Linux - this value is controlled BOTH by the amount of RAM, and overcommit ratio.
                         *      @see  https://www.kernel.org/doc/Documentation/vm/overcommit-accounting about
                         *      sysctl vm.overcommit_memory vm.overcommit_ratio
                         *
                         *      So from: http://liyiadam.blogspot.com/2008/01/procmeminfo.html
                         *          CommitLimit: Based on the overcommit ratio ('vm.overcommit_ratio'),
                         *          this is the total amount of memory currently available to
                         *          be allocated on the system. This limit is only adhered to
                         *          if strict overcommit accounting is enabled (mode 2 in
                         *          'vm.overcommit_memory').
                         *
                         *          The CommitLimit is calculated with the following formula:
                         *              CommitLimit = ('vm.overcommit_ratio' * Physical RAM) + Swap
                         *
                         *          For example, on a system with 1G of physical RAM and 7G
                         *          of swap with a `vm.overcommit_ratio` of 30 it would
                         *          yield a CommitLimit of 7.3G
                         *
                         *          (note that overcommit_ratio is a PERCENTAGE).
                         *
                         *  From Windows:
                         *      WMI:    "Memory(_Total)/Commit Limit"
                         *
                         *          Commit Limit is the amount of virtual memory that can be committed without having to extend
                         *          the paging file(s).  It is measured in bytes.
                         *          Committed memory is the physical memory which has space reserved on the disk paging files.
                         *          There can be one paging file on each logical drive). If the paging file(s) are be expanded,
                         *          this limit increases accordingly.
                         */
                        Optional<uint64_t>  fCommitLimit {};

                        /**
                         *  From Windows:
                         *      WMI:    "Memory(_Total)/Committed Bytes"
                         *
                         *              Committed Bytes is the amount of committed virtual memory, in bytes.
                         *              Committed memory is the physical memory which has space reserved on the disk paging file(s).
                         *              There can be one or more paging files on each physical drive.
                         */
                        Optional<uint64_t>  fCommittedBytes {};

                        /**
                         *      The largest contiguous block of available virtual memory (in bytes).
                         *
                         *      /proc/meminfo::VMallocChunk
                         */
                        Optional<uint64_t>  fLargestAvailableVirtualChunk {};

                        /**
                         *      Total size of all loaded swapfiles (or on windows pagefiles).
                         *
                         *      /proc/meminfo::SwapTotal
                         */
                        Optional<uint64_t>  fPagefileTotalSize {};

                        /**
                         *  Windows:
                         *      Same as @see fCommittedBytes
                         *
                         *  UNIX:
                         *      /proc/meminfo:: (SwapCached + (MemTotal-MemFree))
                         **         ((RETHINK - WAG FOR NOW))
                         */
                        Optional<uint64_t>  fTotalVMInUse {};

                        /**
                         *      From http://en.wikipedia.org/wiki/Commit_charge
                         *          Total is the amount of pagefile-backed virtual address space in use, i.e., the
                         *          current commit charge. This is composed of main memory (RAM) and disk (pagefiles).
                         *          The corresponding performance counter is called "Committed Bytes".
                         *
                         *  On Windows this is "CommitLimit"
                         *
                         *  On Linux this is TotalRAM + TotalSwapSize
                         */
                        Optional<uint64_t>  fTotalPagefileBackedVirtualMemory {};

                        /**
                         *  @see http://en.wikipedia.org/wiki/Page_fault
                         *  @see http://www.linuxinsight.com/proc_vmstat.html
                         *
                         *  fMajorPageFaultsSinceBoot   is the number of major (requiring a disk read)
                         *  faults since the system booted.
                         *
                         *  @see /proc/vmstat::pgmajfault
                         *  @see /proc/vmstat::pgfault
                         */
                        Optional<uint64_t>    fMajorPageFaultsSinceBoot {};

                        /**
                         *  @see http://en.wikipedia.org/wiki/Page_fault
                         *  @see http://www.linuxinsight.com/proc_vmstat.html
                         *
                         *  fMinorPageFaultsSinceBoot   is the number of minor (not requiring a disk read)
                         *  faults since the system booted.
                         *
                         *  @see /proc/vmstat::pgfault and proc/vmstat::pgmajfault (this is pgfault-pgmajfault)
                         */
                        Optional<uint64_t>    fMinorPageFaultsSinceBoot {};

                        /**
                         *  @see fMajorPageFaultsSinceBoot.
                         *
                         *  This is not computed in the first call to the intstrument, but based on successive calls
                         */
                        Optional<double>    fMajorPageFaultsPerSecond {};

                        /**
                         *  @see fMinorPageFaultsSinceBoot.
                         *
                         *  This is not computed in the first call to the intstrument, but based on successive calls
                         */
                        Optional<double>    fMinorPageFaultsPerSecond {};
                    };


                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  To control the behavior of the instrument.
                     */
                    struct  Options {
                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType   fMinimumAveragingInterval { 1.0 };
                    };


                    /**
                     *  Instrument returning Info measurements.
                     */
                    Instrument          GetInstrument (Options options = Options ());


                }
            }


            /*
             *  Specialization to improve performance
             */
            template    <>
            Instruments::Memory::Info   Instrument::CaptureOneMeasurement (DateTimeRange* measurementTimeOut);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_Memory_h_*/
