/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_h_
#define _Stroika_Foundation_Memory_Common_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"

/**
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            /**
             *  \brief Byte is same as defined to be a uint8_t, but more readable and clear its like 'void' for pointer types
             *
             *  Byte is same as defined to be a uint8_t. It can be used to make much code which used void* a
             *  bit more readable.
             *
             *  For example, if you have some typeless data, you can declare
             *      std::vector<Byte> d;    // cannot say vector<void> d;
             *
             *  and the stride is the usual stride one would expect and manually enforce with void* data.
             *
             *  Typically - programmers just use 'char' or 'unsigned char' for this, but this makes that
             *  intent more clear.
             */
            using Byte = uint8_t;

            /**
             *  \brief NEltsOf(X) returns the number of elements in array X
             *
             *      @todo   Found std::begin() could be used to replace old StartOfArray() macro -
             *              see if this too can be replaced with something in C++11?
             */
            template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
            inline constexpr size_t NEltsOf_REAL_ (const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY])
            {
                return SIZE_OF_ARRAY;
            }
// after testing - do this - cuz otherwise use in constant array arg wont work
#define NEltsOf(X) Stroika::Foundation::Memory::NEltsOf_REAL_ (X)

            /**
             *  API to return memory allocation statistics. Generally - these will be inaccurate,
             *  unless certain defines are set in Memory.cpp - but at least some stats can be
             *  returned in either case.
             *
             *  Note - this currently only produces useful answers on windows, but can easily pull
             *  similar values out of /proc fs stuff with linux (nyi).
             *
             *  @todo   Does this belong in "Execution" module"
             */
            struct GlobalAllocationStatistics {
                GlobalAllocationStatistics ();

                size_t fTotalOutstandingAllocations;
                size_t fTotalOutstandingBytesAllocated;
                size_t fPageFaultCount;
                size_t fWorkingSetSize;
                size_t fPagefileUsage;
            };
            GlobalAllocationStatistics GetGlobalAllocationStatistics ();

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
            namespace Private {
                /**
                 *  Sadly, using pragmas to disable warnings didn't work with VC11. VC_BWA_std_copy
                 *  provides a little shared code for a worakround - implemenation of std::copy that - while probably
                 *  inferior to the default - at least isn't much inferior, and doesn't give those
                 *  annoying warnings.
                 */
                template <class _InIt, class _OutIt>
                inline void VC_BWA_std_copy (_InIt _First, _InIt _Last, _OutIt _Dest);
            }
#endif
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Memory_Common_h_*/
