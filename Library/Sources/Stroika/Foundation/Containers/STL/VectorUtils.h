/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_VectorUtils_h_
#define _Stroika_Foundation_Containers_STL_VectorUtils_h_   1

#include    "../../StroikaPreComp.h"

#include    <algorithm>
#include    <vector>

#include    "../../Configuration/Common.h"


/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   STL {


                template    <typename T, typename ContainerOfT>
                vector<T>   mkVC (const ContainerOfT& ts);


                template    <typename T, typename ContainerOfT>
                void    Append (vector<T>* v, const ContainerOfT& v2);


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
