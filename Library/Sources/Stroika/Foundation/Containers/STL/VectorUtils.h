/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_VectorUtils_h_
#define _Stroika_Foundation_Containers_STL_VectorUtils_h_   1

#include    "../../StroikaPreComp.h"

#include    <algorithm>
#include    <vector>

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
                _DeprecatedFunction_ (vector<T>  mkVC (const FROMCONTAINER& rhs), "Use Make<> directly - to be removed in v2.0a12");
                template    <typename T, typename FROMCONTAINER>
                inline  vector<T>  mkVC (const FROMCONTAINER& rhs)
                {
                    return Make<vector<T>> (rhs);
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
#include    "VectorUtils.inl"

#endif  /*_Stroika_Foundation_Containers_STL_VectorUtils_h_*/
