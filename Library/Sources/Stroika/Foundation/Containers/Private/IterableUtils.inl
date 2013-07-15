/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_inl_
#define _Stroika_Foundation_Containers_Private_IterableUtils_inl_  1

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {


                template    <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                bool    Contains_ (const Iterable<T>& c, T item)
                {
                    for (T i : c) {
                        if (ELEMENT_COMPARE_EQUALS_TYPE::Equals (i, item)) {
                            return true;
                        }
                    }
                    return false;
                }


                template    <typename T>
                int    Compare_ (const Iterable<T>& lhs, const Iterable<T>& rhs)
                {
                    auto li = lhs.begin ();
                    auto le = lhs.end ();
                    auto ri = rhs.begin ();
                    auto re = rhs.end ();
                    int c;
                    while (li != le and ri != re and ((c = (*li).Compare (*ri) ) == 0)) {
                        ++li;
                        ++ri;
                    }
                    if (li == le) {
                        if (ri == re) {
                            return 0;   // all items same and loop ended with both things at end
                        }
                        else {
                            return -1;  // lhs shorter but an initial sequence of rhs
                        }
                    }
                    else {
                        if (ri == re) {
                            return 1;   // rhs shorter but an initial sequence of lhs
                        }
                        else {
                            Assert (li != le and ri != re);
                            Assert (c ==  (*li).Compare (*ri));
                            return c;
                        }
                    }
                }


                template    <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                bool    Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs)
                {
                    // Check length, so we dont need to check both iterators for end/done
                    if (&lhs == &rhs) {
                        return true;
                    }
                    if (lhs.GetLength () != rhs.GetLength ()) {
                        return false;
                    }
                    auto li = lhs.MakeIterator ();
                    auto ri = rhs.MakeIterator ();
                    while (not li.Done () and ELEMENT_COMPARE_EQUALS_TYPE::Equals (*li, *ri)) {
                        ++li;
                        ++ri;
                    }
                    return li.Done ();
                }


                template    <typename T>
                size_t    IndexOf_ (const Iterable<T>& c, T item, size_t sentinalValueForNotFound)
                {
                    size_t  n = 0;
                    for (T i : c) {

                        if (i == item) {
                            return n;
                        }
                        n++;
                    }
                    return sentinalValueForNotFound;
                }


                template    <typename T>
                size_t    IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs, size_t sentinalValueForNotFound)
                {
                    size_t  n = 0;
                    for (auto i = c.begin (); i != c.end (); ++i, ++n) {
                        bool foundDiff = false;
                        auto ii = i;
                        for (T r : rhs) {
                            if (ii == c.end ()) {
                                return sentinalValueForNotFound;
                            }
                            if (not (r == *ii)) {
                                foundDiff = true;
                                break;
                            }
                            ++ii;
                        }
                        if (not foundDiff) {
                            return n;
                        }
                    }
                    return sentinalValueForNotFound;
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Private_IterableUtils_inl_ */
