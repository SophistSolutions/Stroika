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
            template    <typename   ITERATOR_OF_T>
            auto    Mean (ITERATOR_OF_T start, ITERATOR_OF_T end) -> typename remove_cv<typename remove_reference<decltype (*start)>::type>::type {
                Require (start != end);
                using T =   typename    remove_reference<typename remove_cv<decltype (*start)>::type>::type;
                unsigned int        cnt     =   1;
                auto                result  =   *start++;
                for (ITERATOR_OF_T i = start; i != end; ++i)
                {
                    result += *i;
                    cnt++;
                }
                return result / cnt;
            }
            template    <typename   CONTAINER_OF_T>
            auto    Mean (const CONTAINER_OF_T& container) -> typename remove_cv<typename remove_reference<decltype (*container.begin ())>::type>::type {
                Require (not container.empty ());
                return Mean (begin (container), end (container));
            }


            /*
             ********************************************************************************
             ********************************** Median **************************************
             ********************************************************************************
             */
            template    <typename   ITERATOR_OF_T>
            auto    Median (ITERATOR_OF_T start, ITERATOR_OF_T end) -> typename remove_cv<typename remove_reference<decltype (*start)>::type>::type {
                Require (start != end);
                // sloppy impl, but workable
                using T =   typename remove_cv<typename remove_reference<decltype (*start)>::type>::type;
                size_t  size    =   distance (start, end);
                Memory::SmallStackBuffer<T>   tmp (0);      // copy cuz data modified
                for (ITERATOR_OF_T i = start; i != end; ++i)
                {
                    tmp.push_back (*i);
                }
                if ((size % 2) == 0)
                {
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
            template    <typename   CONTAINER_OF_T>
            auto    Median (const CONTAINER_OF_T& container) -> typename remove_cv<typename remove_reference<decltype (*container.begin ())>::type>::type {
                Require (not container.empty ());
                return Median (begin (container), end (container));
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_Statistics_inl_*/
