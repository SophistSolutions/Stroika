
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_STL_VectorUtils_inl_
#define _Stroika_Foundation_Containers_STL_VectorUtils_inl_ 1

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   STL {


                template    <typename T, typename ContainerOfT>
                vector<T>   mkVC (const ContainerOfT& ts)
                {
                    vector<T>   result;
                    result.reserve (ts.size ());
                    for (typename ContainerOfT::const_iterator i = ts.begin (); i != ts.end (); ++i) {
                        result.push_back (*i);
                    }
                    return result;
                }


                template    <typename T, typename ContainerOfT>
                void    Append (vector<T>* v, const ContainerOfT& v2)
                {
                    //@todo consider rewrite using v->insert (v->end (), v2.begin (), v2.end ()); not sure we want/need the capacity thing
                    RequireNotNull (v);
                    size_t  c   =   max (v->capacity (), v->size () + v2.size ());
                    v->reserve (c);
                    for (typename ContainerOfT::const_iterator i = v2.begin (); i != v2.end (); ++i) {
                        v->push_back (*i);
                    }
                }


                template    <typename T>
                vector<T>   Intersection (const vector<T>& s1, const vector<T>& s2)
                {
                    vector<T>   result;
                    result.reserve (min (s1.size (), s2.size ()));
                    for (typename vector<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
                        if (std::find (s2.begin (), s2.end (), *i) != s2.end ()) {
                            result.push_back (*i);
                        }
                    }
                    return result;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_STL_VectorUtils_inl_*/
