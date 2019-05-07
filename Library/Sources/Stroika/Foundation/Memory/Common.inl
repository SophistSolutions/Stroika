/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_inl_
#define _Stroika_Foundation_Memory_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ********************* Memory::GlobalAllocationStatistics ***********************
     ********************************************************************************
     */
    inline GlobalAllocationStatistics::GlobalAllocationStatistics ()
        : fTotalOutstandingAllocations (0)
        , fTotalOutstandingBytesAllocated (0)
        , fPageFaultCount (0)
        , fWorkingSetSize (0)
        , fPagefileUsage (0)
    {
    }

    /*
     ********************************************************************************
     ********************************* Memory::MemCmp *******************************
     ********************************************************************************
     */
    template <>
    constexpr int MemCmp (const uint8_t* lhs, const uint8_t* rhs, std::size_t count)
    {
        //Require (count == 0 or lhs != nullptr);
        //Require (count == 0 or rhs != nullptr);
        const uint8_t* li = lhs;
        const uint8_t* ri = rhs;
        for (; count--; li++, ri++) {
            uint8_t lv = *li;
            uint8_t rv = *ri;
            if (lv != rv) {
                return (lv - rv);
            }
        }
        return 0;
    }
    template <typename T>
    constexpr int MemCmp (const T* lhs, const T* rhs, size_t count)
    {
        return MemCmp (reinterpret_cast<const uint8_t*> (lhs), reinterpret_cast<const uint8_t*> (rhs), n * sizeof (T));
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
