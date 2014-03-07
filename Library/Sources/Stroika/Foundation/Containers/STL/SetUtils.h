/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_SetUtils_h_
#define _Stroika_Foundation_Containers_STL_SetUtils_h_  1

#include    "../../StroikaPreComp.h"

#include    <set>

#include    "../../Configuration/Common.h"

#include    "Utilities.h"

//// THIS FILE DEPRECATED - DELETE FOR v2.0a12

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


                //DEPREACRED
                /// @todo - THIS really SB obsolete - not sure why not just use vector<T> () CTOR?
                template    <typename T, typename FROMCONTAINER>
                _DeprecatedFunction_ (set<T>  mkSfromC (const FROMCONTAINER& rhs), "Use Make<> instead - to be removed in v2.0a12");
                template    <typename T, typename FROMCONTAINER>
                inline  set<T>  mkSfromC (const FROMCONTAINER& rhs)
                {
                    return Make<set<T>> (rhs);
                }


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
