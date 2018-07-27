/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_inl_
#define _Stroika_Foundation_Memory_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Memory {

    //  class   Memory::GlobalAllocationStatistics
    inline GlobalAllocationStatistics::GlobalAllocationStatistics ()
        : fTotalOutstandingAllocations (0)
        , fTotalOutstandingBytesAllocated (0)
        , fPageFaultCount (0)
        , fWorkingSetSize (0)
        , fPagefileUsage (0)
    {
    }

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
    namespace Private {
        template <class _InIt, class _OutIt>
        inline void VC_BWA_std_copy (_InIt _First, _InIt _Last, _OutIt _Dest)
        {
            auto o = _Dest;
            for (auto i = _First; i != _Last; ++i, ++o) {
                *o = *i;
            }
        }
    }
#endif

}
#endif /*_Stroika_Foundation_Memory_Common_inl_*/
