/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_ElementTraits_h_
#define _Stroika_Foundation_Containers_ElementTraits_h_   1


/*
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 *      (o)     Must add more types - for other sorts of containers
 *
 *
 * Notes:
 *
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"





namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             * This is the default set of traits for a collection which only requires the operator==
             */
            template    <typename T>
            struct  TWithCompareEquals {
				bool    opererator== (const T& lhs, const T& rhs);
            };


            /*
             * TWithCompareEqualsAndHashFunction is TWithCompareEquals<T> and defines a HashValue() funciton.
             */
            template    <typename T>
            struct  TWithCompareEqualsAndHashFunction : TWithCompareEquals<T> {
                unsigned int    HashValue (const T& val);
            };


            /*
             * TWithCompareEqualsAndHashFunction is TWithCompareEquals<T> and defines a operator< funciton.
             */
            template    <typename T>
            struct  TWithCompareEqualsAndLess : TWithCompareEquals {
                bool    opererator < (const T& lhs, const T& rhs);
            };


            /*
             * TWithCompareEqualsAndHashFunction is TWithCompareEquals<T> and defines a operator<= funciton.
             */
            template    <typename T>
            struct  TWithCompareEqualsAndLessOrEquals : TWithCompareEquals {
                bool    opererator<= (const T& lhs, const T& rhs);
            };


        }
    }
}


#endif  /*_Stroika_Foundation_Containers_ElementTraits_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ElementTraits.inl"

