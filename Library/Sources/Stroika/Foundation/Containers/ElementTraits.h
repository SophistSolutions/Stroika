/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_ElementTraits_h_
#define _Stroika_Foundation_Containers_ElementTraits_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"



/*
 *
 * Description:
 *
 *      THis code MAY end up being a TRAITs mechanism for defining traits in various templates (like hashing etc). Or
 *      It maybe a 'CONCEPT' - like feature to generate better error messages and documentation about requiremnts on T
 *      for given types.
 *
 *
 * TODO:
 *
 *      (o)     Must add more types - for other sorts of containers.
 *
 *      (o)     Consider separating out traits types - so no direct inheritance (or put combined ones in separate ns?)
 *              This will depend on how we end up using thse.
 *
 *
 * Notes:
 *
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             * This is the default set of traits for a collection which only requires the operator==
             */
            template    <typename T>
            struct  TWithCompareEquals {
                bool    opererator == (const T& lhs, const T& rhs);
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
                bool    opererator <= (const T& lhs, const T& rhs);
            };


        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ElementTraits.inl"

#endif  /*_Stroika_Foundation_Containers_ElementTraits_h_ */
