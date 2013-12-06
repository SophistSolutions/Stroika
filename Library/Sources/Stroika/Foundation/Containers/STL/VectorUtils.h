/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_VectorUtils_h_
#define _Stroika_Foundation_Containers_STL_VectorUtils_h_   1

#include    "../../StroikaPreComp.h"

#include    <algorithm>
#include    <vector>

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
                template    <typename T, typename ContainerOfT>
                vector<T>   mkVC (const ContainerOfT& ts);

                /*
                 *  Though you can append to a vector<> with
                 *      insert (this->begin (), arg.begin (), arg.end ())
                 *  That's awkward if 'arg' is an unnamed value - say the result of a function. You must
                 *  assign to a named temporary. This helper makes that unneeded.
                 */
                template    <typename T, typename ContainerOfT>
                void    Append (vector<T>* v, const ContainerOfT& v2);


                // @todo - redo with RHS as arbirrary container. Probably redo with stroika Set<>
                // maybe osbolete cuz can alway use
                // (Containers::Set<T> (s1) & s2).As<vector<T>> ()
                template    <typename T>
                vector<T>   Intersection (const vector<T>& s1, const vector<T>& s2);


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
