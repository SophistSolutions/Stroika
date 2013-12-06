
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_STL_SetUtils_inl_
#define _Stroika_Foundation_Containers_STL_SetUtils_inl_    1

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   STL {



                template    <typename T, typename FROMCONTAINER>
                inline  set<T>  mkSfromC (const FROMCONTAINER& rhs)
                {
                    return set<T> (rhs.begin (), rhs.end ());
                }



                template    <typename T>
                bool    Intersects (const set<T>& s1, const set<T>& s2)
                {
                    for (typename set<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
                        if (s2.find (*i) != s2.end ()) {
                            return true;
                        }
                    }
                    return false;
                }




                template    <typename T>
                set<T>  Intersection (const set<T>& s1, const set<T>& s2)
                {
                    set<T>  result;
                    for (typename set<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
                        if (s2.find (*i) != s2.end ()) {
                            result.insert (*i);
                        }
                    }
                    return result;
                }
                template    <typename T>
                void    Intersection (set<T>* s1, const set<T>& s2)
                {
                    RequireNotNull (s1);
                    // Sloppy - but hopefully adequate implementation
                    if (not s1->empty () and not s2.empty ()) {
                        *s1 = Intersection (*s1, s2);
                    }
                }




                template    <typename T, typename FROMCONTAINER>
                void    Union (set<T>* s1, const FROMCONTAINER& s2)
                {
                    for (auto i : s2) {
                        if (s1->find (i) == s1->end ()) {
                            s1->insert (i);
                        }
                    }
                }
                template    <typename T, typename FROMCONTAINER>
                set<T>  Union (const set<T>& s1, const FROMCONTAINER& s2)
                {
                    set<T>  result  =   s1;
                    Union (&result, s2);
                    return result;
                }


                template    <typename T, typename FROMCONTAINER>
                void    Difference (set<T>* s1, const FROMCONTAINER& s2)
                {
                    RequireNotNull (s1);
                    for (auto i : s2) {
                        if (s1->find (i) != s1->end ()) {
                            s1->erase (i);
                        }
                    }
                }
                template    <typename T, typename FROMCONTAINER>
                set<T>  Difference (const set<T>& s1, const FROMCONTAINER& s2)
                {
                    set<T>  result = s1;
                    Difference (&result, s2);
                    return result;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_STL_SetUtils_inl_*/
