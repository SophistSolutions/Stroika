/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_SetUtils_h_
#define _Stroika_Foundation_Containers_STL_SetUtils_h_  1

#include    "../../StroikaPreComp.h"

#include    <set>

#include    "../../Configuration/Common.h"


/**
*  \file
*
*  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
*
* TODO:
*
*      @todo    Perhaps obsolete this entire module? Perhaps these utilities are useful
*               since so much code still uses STL?
*
*/



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   STL {


                /// @todo - THIS really SB obsolete - not sure why not just use vector<T> () CTOR?
                template    <typename T, typename FROMCONTAINER>
                set<T>  mkSfromC (const FROMCONTAINER& rhs);


                template    <typename T>
                bool    Intersect (const set<T>& s1, const set<T>& s2);


                template    <typename T>
                set<T>  Intersection (const set<T>& s1, const set<T>& s2);
                template    <typename T>
                void    Intersection (set<T>* s1, const set<T>& s2);

                template    <typename T, typename FROMCONTAINER>
                void    Union (set<T>* s1, const FROMCONTAINER& s2);
                template    <typename T, typename FROMCONTAINER>
                set<T>  Union (const set<T>& s1, const FROMCONTAINER& s2);

                template    <typename T, typename FROMCONTAINER>
                void    Difference (set<T>* s1, const FROMCONTAINER& s2);
                template    <typename T, typename FROMCONTAINER>
                set<T>  Difference (const set<T>& s1, const FROMCONTAINER& s2);

            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SetUtils.inl"

#endif  /*_Stroika_Foundation_Containers_STL_SetUtils_h_*/
