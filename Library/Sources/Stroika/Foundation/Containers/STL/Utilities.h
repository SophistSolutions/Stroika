/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_Utilities_h_
#define _Stroika_Foundation_Containers_STL_Utilities_h_ 1

#include "../../StroikaPreComp.h"

#include <set>
#include <vector>

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"

/**
*  \file
*
*  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
*
* TODO:
*
*/

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace STL {

                /**
                 *  \note   similar to std::equal () - but takes iterables as arguments, not iterators
                 */
                template <typename ITERABLE_OF_T, typename T = typename ITERABLE_OF_T::value_type, typename EQUALS_COMPARER = std::equal_to<T>, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
                bool equal (const ITERABLE_OF_T& lhs, const ITERABLE_OF_T& rhs, EQUALS_COMPARER&& equalsComparer = {});

                /*
                 *  STL container constructors take a begin/end combo, but no CONTAINER ctor (except for same as starting).
                 *  So to convert from set<T> to vector<T> is a pain (if you have a function returning set<>). To do pretty
                 *  easily with STL, you need a named temporary. Thats basically all this helper function does.
                 *
                 *  So instead of:
                 *          set<T>  tmp = funCall_returning_set();
                 *          vector<T>   realAnswer = vector<T> (tmp.begin (), tmp.end ());
                 *
                 *  You can write:
                 *      vector<T>   realAnswer = STL::Make<vector<T>> (funCall_returning_set());
                 */
                template <typename CREATE_CONTAINER_TYPE, typename FROM_CONTAINER_TYPE>
                CREATE_CONTAINER_TYPE Make (const FROM_CONTAINER_TYPE& rhs);

                /**
                 *  Though you can append to a vector<> with
                 *      insert (this->begin (), arg.begin (), arg.end ())
                 *  That's awkward if 'arg' is an unnamed value - say the result of a function. You must
                 *  assign to a named temporary. This helper makes that unneeded.
                 */
                template <typename TARGET_CONTAINER>
                void Append (TARGET_CONTAINER* v);
                template <typename TARGET_CONTAINER, typename SRC_CONTAINER>
                void Append (TARGET_CONTAINER* v, const SRC_CONTAINER& v2);
                template <typename TARGET_CONTAINER, typename SRC_CONTAINER, typename... Args>
                void Append (TARGET_CONTAINER* v, const SRC_CONTAINER& v2, Args... args);

                /**
                 *  \brief construct a new STL container by concatenating the args together.
                 *
                 *  \note Hard to believe this is so awkward in STL!
                 *
                 *  @see Concatenate
                 */
                template <typename TARGET_CONTAINER, typename SRC_CONTAINER, typename... Args>
                TARGET_CONTAINER Concat (const SRC_CONTAINER& v2, Args... args);

                /**
                 *  \brief construct a new vector<T> by concatenating the args together. Alias for Concat<vector<typename SRC_CONTAINER::value_type>> ()
                 *
                 *  @see Concat
                 */
                template <typename SRC_CONTAINER, typename... Args>
                vector<typename SRC_CONTAINER::value_type> Concatenate (const SRC_CONTAINER& v2, Args... args);

                /**
                 * Returns true if the intersetion of s1 and s2 is non-empty
                 */
                template <typename T>
                bool Intersects (const set<T>& s1, const set<T>& s2);

                /**
                *@todo - redo with RHS as arbirrary container. Probably redo with stroika Set<>
                // maybe osbolete cuz can alway use
                // (Containers::Set<T> (s1) & s2).As<vector<T>> ()
                */
                template <typename T>
                set<T> Intersection (const set<T>& s1, const set<T>& s2);
                template <typename T>
                void Intersection (set<T>* s1, const set<T>& s2);
                template <typename T>
                vector<T> Intersection (const vector<T>& s1, const vector<T>& s2);

                /**
                */
                template <typename T, typename FROMCONTAINER>
                void Union (set<T>* s1, const FROMCONTAINER& s2);
                template <typename T, typename FROMCONTAINER>
                set<T> Union (const set<T>& s1, const FROMCONTAINER& s2);

                /**
                */
                template <typename T, typename FROMCONTAINER>
                void Difference (set<T>* s1, const FROMCONTAINER& s2);
                template <typename T, typename FROMCONTAINER>
                set<T> Difference (const set<T>& s1, const FROMCONTAINER& s2);
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Utilities.inl"

#endif /*_Stroika_Foundation_Containers_STL_Utilities_h_*/
