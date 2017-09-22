
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_STL_Utilities_inl_
#define _Stroika_Foundation_Containers_STL_Utilities_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace STL {

                template <typename CREATE_CONTAINER_TYPE, typename FROM_CONTAINER_TYPE>
                inline CREATE_CONTAINER_TYPE Make (const FROM_CONTAINER_TYPE& rhs)
                {
                    return CREATE_CONTAINER_TYPE (rhs.begin (), rhs.end ());
                }

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

                template <typename T, typename ContainerOfT>
                void Append (vector<T>* v, const ContainerOfT& v2)
                {
                    //@todo consider rewrite using v->insert (v->end (), v2.begin (), v2.end ()); not sure we want/need the capacity thing
                    RequireNotNull (v);
                    size_t c = max (v->capacity (), v->size () + v2.size ());
                    v->reserve (c);
                    for (typename ContainerOfT::const_iterator i = v2.begin (); i != v2.end (); ++i) {
                        v->push_back (*i);
                    }
                }

                template <typename T>
                vector<T> Concatenate (const vector<T>& v1, const vector<T>& v2)
                {
                    vector<T> result = v1;
                    result.insert (result.end (), v2.begin (), v2.end ());
                    return result;
                }
                template <typename T>
                vector<T> Concatenate (const vector<T>& v1, const vector<T>& v2, const vector<T>& v3)
                {
                    vector<T> result = v1;
                    result.insert (result.end (), v2.begin (), v2.end ());
                    result.insert (result.end (), v3.begin (), v3.end ());
                    return result;
                }

                template <typename T>
                vector<T> Intersection (const vector<T>& s1, const vector<T>& s2)
                {
                    vector<T> result;
                    result.reserve (min (s1.size (), s2.size ()));
                    for (typename vector<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
                        if (std::find (s2.begin (), s2.end (), *i) != s2.end ()) {
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
        }
    }
}
#endif /*_Stroika_Foundation_Containers_STL_Utilities_inl_*/
