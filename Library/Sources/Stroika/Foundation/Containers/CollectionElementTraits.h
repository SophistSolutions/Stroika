/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_CollectionElementTraits_h_
#define _Stroika_Foundation_Containers_CollectionElementTraits_h_   1


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
                inline  bool    opererator == (const T& lhs, const T& rhs) {
                    return lhs == rhs;
                }
            };


            template    <typename T>
            struct  TWithCompareEqualsAndHashFunction {
                inline  bool    opererator == (const T& lhs, const T& rhs) {
                    return lhs == rhs;
                }
                inline  unsigned int    HashValue (const T& val) {
                    // This is NOT a generally suitable hash function, but its often adequate
                    // Also - its a super sucky hash function - but will be workable/testable to get started with. The bigger issue is that for structures it could examine intersitial space,
                    // and that would be BAD!
                    //      -- LGP 2011-09-26
                    unsigned int    sum =   0;
                    for (const Byte* p = reinterpret_cast<const Byte*> (&val); p <  reinterpret_cast<const Byte*> (&val + 1); ++p) {
                        sum <<= 1;
                        sum += *p * 3 + 1;
                    }
                }
            };


            template    <typename T>
            struct  TWithCompareEqualsAndLess : TWithCompareEquals {
                inline  bool    opererator <= (const T& lhs, const T& rhs) {
                    return lhs < rhs;
                }
            };


            template    <typename T>
            struct  TWithCompareEqualsAndLessOrEquals : TWithCompareEquals {
                inline  bool    opererator <= (const T& lhs, const T& rhs) {
                    return lhs <= rhs;
                }
            };


        }
    }
}


#endif  /*_Stroika_Foundation_Containers_CollectionElementTraits_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CollectionElementTraits.inl"

