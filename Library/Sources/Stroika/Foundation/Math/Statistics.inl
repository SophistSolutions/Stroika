/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Statistics_inl_
#define _Stroika_Foundation_Math_Statistics_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <algorithm>

#include    "../Debug/Assertions.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /*
             ********************************************************************************
             ************************************ Mean **************************************
             ********************************************************************************
             */
            template    <typename   ITERATOR_OF_T, typename RESULT_TYPE>
            RESULT_TYPE Mean (ITERATOR_OF_T start, ITERATOR_OF_T end)
            {
                Require (start != end);
                unsigned int        cnt     {};
                RESULT_TYPE         result  {};
                for (ITERATOR_OF_T i = start; i != end; ++i) {
                    result += *i;
                    cnt++;
                }
                return result / cnt;
            }
            template    <typename   CONTAINER_OF_T, typename RESULT_TYPE>
            inline  RESULT_TYPE     Mean (const CONTAINER_OF_T& container)
            {
                Require (not container.empty ());
                using   ITERATOR_TYPE   =   decltype (begin (container));
                return Mean<ITERATOR_TYPE, RESULT_TYPE> (begin (container), end (container));
            }


            /*
             ********************************************************************************
             ********************************** Median **************************************
             ********************************************************************************
             */
            template    <typename   ITERATOR_OF_T, typename RESULT_TYPE>
            RESULT_TYPE Median (ITERATOR_OF_T start, ITERATOR_OF_T end)
            {
                Require (start != end);
                Memory::SmallStackBuffer<RESULT_TYPE>   tmp     { start, end };      // copy cuz data modified
                size_t                                  size    =   distance (start, end);
                nth_element (tmp.begin (), tmp.begin () + size / 2, tmp.end ());
                RESULT_TYPE     result { tmp[size / 2] };
                if ((size % 2) == 0) {
                    Assert (size >= 2); // cuz require at start >=1 and since even
                    // NB: Could use sort instead of nth_element, and some on the web suggest faster, but sort is nlogn, and nth_elemnet is O(n) (even
                    // when you do it twice.
                    nth_element (tmp.begin (), tmp.begin () + size / 2 - 1, tmp.end ());
                    result += tmp[size / 2 - 1];
                    result /= static_cast<RESULT_TYPE> (2);
                }
                return result;
            }
            template    <typename   CONTAINER_OF_T, typename RESULT_TYPE>
            inline  RESULT_TYPE     Median (const CONTAINER_OF_T& container)
            {
                Require (not container.empty ());
                using   ITERATOR_TYPE   =   decltype (begin (container));
                return Median<ITERATOR_TYPE, RESULT_TYPE> (begin (container), end (container));
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_Statistics_inl_*/
