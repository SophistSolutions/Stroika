
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_STL_Utilities_inl_
#define _Stroika_Foundation_Containers_STL_Utilities_inl_ 1

#include <algorithm>

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::STL {

    template <typename ITERABLE_OF_T, typename T, typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    bool equal (const ITERABLE_OF_T& lhs, const ITERABLE_OF_T& rhs, EQUALS_COMPARER&& equalsComparer)
    {
        return lhs.size () == rhs.size () and equal (lhs.begin (), lhs.end (), rhs.begin (), equalsComparer);
    }

    /*
        ********************************************************************************
        ***************************** Containers::STL::Make ****************************
        ********************************************************************************
        */
    template <typename CREATE_CONTAINER_TYPE, typename FROM_CONTAINER_TYPE>
    inline CREATE_CONTAINER_TYPE Make (const FROM_CONTAINER_TYPE& rhs)
    {
        return CREATE_CONTAINER_TYPE (rhs.begin (), rhs.end ());
    }

    /*
        ********************************************************************************
        ************************ Containers::STL::Append *******************************
        ********************************************************************************
        */
    template <typename TARGET_CONTAINER>
    inline void Append (TARGET_CONTAINER* v)
    {
        RequireNotNull (v);
    }
    template <typename TARGET_CONTAINER, typename SRC_CONTAINER>
    inline void Append (TARGET_CONTAINER* v, const SRC_CONTAINER& v2)
    {
        RequireNotNull (v);
        size_t c = max (v->capacity (), v->size () + v2.size ());
        v->reserve (c);
        v->insert (v->end (), v2.begin (), v2.end ());
    }
    template <typename TARGET_CONTAINER, typename SRC_CONTAINER, typename... Args>
    inline void Append (TARGET_CONTAINER* v, const SRC_CONTAINER& v2, Args... args)
    {
        RequireNotNull (v);
        Append (v, v2);
        Append (v, args...);
    }

    /*
        ********************************************************************************
        ***************************** Containers::STL::Concat **************************
        ********************************************************************************
        */
    template <typename TARGET_CONTAINER, typename SRC_CONTAINER, typename... Args>
    TARGET_CONTAINER Concat (const SRC_CONTAINER& v2, Args... args)
    {
        TARGET_CONTAINER tmp;
        Append (&tmp, v2, args...);
        return tmp;
    }

    /*
        ********************************************************************************
        ************************ Containers::STL::Concatenate **************************
        ********************************************************************************
        */
    template <typename SRC_CONTAINER, typename... Args>
    inline vector<typename SRC_CONTAINER::value_type> Concatenate (const SRC_CONTAINER& v2, Args... args)
    {
        return Concat<vector<typename SRC_CONTAINER::value_type>> (v2, args...);
    }

    /*
        ********************************************************************************
        ************************ Containers::STL::Intersects ***************************
        ********************************************************************************
        */
    template <typename T>
    bool Intersects (const set<T>& s1, const set<T>& s2)
    {
        for (typename set<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
            if (s2.find (*i) != s2.end ()) {
                return true;
            }
        }
        return false;
    }

    /*
        ********************************************************************************
        ********************* Containers::STL::Intersection ****************************
        ********************************************************************************
        */
    template <typename T>
    vector<T> Intersection (const vector<T>& s1, const vector<T>& s2)
    {
        vector<T> result;
        result.reserve (min (s1.size (), s2.size ()));
        for (typename vector<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
            if (find (s2.begin (), s2.end (), *i) != s2.end ()) {
                result.push_back (*i);
            }
        }
        return result;
    }
    template <typename T>
    set<T> Intersection (const set<T>& s1, const set<T>& s2)
    {
        set<T> result;
        for (typename set<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
            if (s2.find (*i) != s2.end ()) {
                result.insert (*i);
            }
        }
        return result;
    }
    template <typename T>
    void Intersection (set<T>* s1, const set<T>& s2)
    {
        RequireNotNull (s1);
        // Sloppy - but hopefully adequate implementation
        if (not s1->empty () and not s2.empty ()) {
            *s1 = Intersection (*s1, s2);
        }
    }

    /*
        ********************************************************************************
        **************************** Containers::STL::Union ****************************
        ********************************************************************************
        */
    template <typename T, typename FROMCONTAINER>
    void Union (set<T>* s1, const FROMCONTAINER& s2)
    {
        for (auto i : s2) {
            if (s1->find (i) == s1->end ()) {
                s1->insert (i);
            }
        }
    }
    template <typename T, typename FROMCONTAINER>
    set<T> Union (const set<T>& s1, const FROMCONTAINER& s2)
    {
        set<T> result = s1;
        Union (&result, s2);
        return result;
    }

    /*
        ********************************************************************************
        **************************** Containers::STL::Difference ***********************
        ********************************************************************************
        */
    template <typename T, typename FROMCONTAINER>
    void Difference (set<T>* s1, const FROMCONTAINER& s2)
    {
        RequireNotNull (s1);
        for (auto i : s2) {
            if (s1->find (i) != s1->end ()) {
                s1->erase (i);
            }
        }
    }
    template <typename T, typename FROMCONTAINER>
    set<T> Difference (const set<T>& s1, const FROMCONTAINER& s2)
    {
        set<T> result = s1;
        Difference (&result, s2);
        return result;
    }
}
#endif /*_Stroika_Foundation_Containers_STL_Utilities_inl_*/
