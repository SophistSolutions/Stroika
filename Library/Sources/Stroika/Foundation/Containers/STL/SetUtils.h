/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_SetUtils_h_
#define _Stroika_Foundation_Containers_STL_SetUtils_h_  1

#include    "../../StroikaPreComp.h"

#include    <set>

#include    "../../Configuration/Common.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   STL {


                template    <typename T>
                set<T>  operator- (const set<T>& lhs, const set<T>& rhs);
                template    <typename T>
                set<T>  operator+ (const set<T>& lhs, const set<T>& rhs);
                template    <typename T, typename FROMCONTAINER>
                set<T>& operator+= (set<T>& lhs, const FROMCONTAINER& rhs);
                template    <typename T, typename FROMCONTAINER>
                set<T>& operator-= (set<T>& lhs, const FROMCONTAINER& rhs);

                template    <typename T>
                set<T>  mkS ();
                template    <typename T>
                set<T>  mkS (const T& t1);
                template    <typename T>
                set<T>  mkS (const T& t1, const T& t2);
                template    <typename T>
                set<T>  mkS (const T& t1, const T& t2, const T& t3);
                template    <typename T>
                set<T>  mkS (const T& t1, const T& t2, const T& t3, const T& t4);
                template    <typename T, typename FROMCONTAINER>
                set<T>  mkSfromC (const FROMCONTAINER& rhs);

                template    <typename T>
                void    Intersect (set<T>* s1, const set<T>& s2);

                template    <typename T>
                set<T>  Intersection (const set<T>& s1, const set<T>& s2);
                template    <typename T>
                bool    Intersect (const set<T>& s1, const set<T>& s2);

                template    <typename T>
                void    Union (set<T>* s1, const set<T>& s2);
                template    <typename T>
                set<T>  Union (const set<T>& s1, const set<T>& s2);

                template    <typename T>
                set<T>  Difference (const set<T>& lhs, const set<T>& rhs);

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_STL_SetUtils_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SetUtils.inl"
