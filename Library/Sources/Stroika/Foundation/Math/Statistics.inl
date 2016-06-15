/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
                unsigned int        cnt     =   1;
                RESULT_TYPE         result  =   *start++;
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
                // sloppy impl, but workable
                size_t  size    =   distance (start, end);
                Memory::SmallStackBuffer<RESULT_TYPE>   tmp (0);      // copy cuz data modified
                for (ITERATOR_OF_T i = start; i != end; ++i) {
                    tmp.push_back (*i);
                }
                if ((size % 2) == 0) {
                    Assert (size >= 2); // cuz require at start >=1 and since even
                    // @todo MAYBE faster to just use std::sort () - or two partial sorts? TEST
                    sort (tmp.begin (), tmp.end ());
                    return (tmp[size / 2] + tmp[size / 2 - 1]) / static_cast<T> (2);
                }
                else {
                    nth_element (tmp.begin (), tmp.begin () + size / 2, tmp.end ());
                    return tmp[size / 2];
                }
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
