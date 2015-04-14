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
 *  TODO:
 *      @todo   Add options to GetInstrument (), for things like what to include, and maybe the time
 *              threshold for initial seeding of delta counters.
 *
 *      @todo   Find a way to restructure use of Debug::AssertExternallySynchronizedLock so it
 *              handles copy construction
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
                         *  /proc/meminfo::MemFree
                         */
                        Optional<uint64_t>  fFreePhysicalMemory {};

                        /**
                         *      The total amount of allocated virtual address space (in bytes).
                         *
                         *      /proc/meminfo::VMallocTotal
                         */
                        Optional<uint64_t>  fTotalVirtualMemory {};

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
                    extern  const   MeasurementType kSystemMemoryMeasurement;

                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  Instrument returning Info measurements.
                     */
                    Instrument          GetInstrument ();


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
