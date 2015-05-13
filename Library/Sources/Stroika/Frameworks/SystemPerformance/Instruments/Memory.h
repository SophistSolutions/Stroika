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
                         *      The total amount of allocated virtual address space (in bytes).
                         *
                         *      /proc/meminfo::VMallocTotal
                         */
                        Optional<uint64_t>  fTotalVirtualMemory {};

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
                         */
                        Optional<uint64_t>  fCommitLimit {};

                        /**
                         *      The total amount of used virtual address space (in bytes).
                         *
                         *      /proc/meminfo::VMallocUsed
                         */
                        Optional<uint64_t>  fUsedVirtualMemory {};

                        /**
                         *      The largest contiguous block of available virtual memory (in bytes).
                         *
                         *      /proc/meminfo::VMallocChunk
                         */
                        Optional<uint64_t>  fLargestAvailableVirtualChunk {};

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
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_Memory_h_*/
