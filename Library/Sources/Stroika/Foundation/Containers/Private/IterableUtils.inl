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


                template    <typename T>
                bool    Contains_ (const Iterable<T>& c, T item)
                {
                    for (T i : c) {
                        if (i == item) {
                            return true;
                        }
                    }
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


                template    <typename T>
                int   Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs)
                {
                    auto li = lhs.begin ();
                    auto le = lhs.end ();
                    auto ri = rhs.begin ();
                    auto re = rhs.end ();
                    while (li != le and ri != re and * li == *ri) {
                        ++li;
                        ++ri;
                    }
                    return (li == le) and (ri == re);
                }


                template    <typename T>
                size_t    IndexOf_ (const Iterable<T>& c, T item)
                {
                    size_t  n = 0;
                    for (T i : c) {

                        if (i == item) {
                            return n;
                        }
                        n++;
                    }
                    return kBadIndex;
                }


                template    <typename T>
                size_t    IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs)
                {
                    size_t  n = 0;
                    ///tmphack - more complicated - must do like equals above - iterating over both at same time...
                    AssertNotImplemented ();
                    return kBadIndex;
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Private_IterableUtils_inl_ */
