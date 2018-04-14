/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_inl_
#define _Stroika_Foundation_Containers_Private_IterableUtils_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {

                template <typename T, typename ELEMENT_COMPARER_TYPE>
                int Compare_ (const Iterable<T>& lhs, const Iterable<T>& rhs, const ELEMENT_COMPARER_TYPE& comparer)
                {
                    auto li = lhs.begin ();
                    auto le = lhs.end ();
                    auto ri = rhs.begin ();
                    auto re = rhs.end ();
                    DISABLE_COMPILER_MSC_WARNING_START (6001)
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
                    // no need for c' initialization cuz only used in else return at end, but never get there
                    // unless set at least once
                    int c;
                    while ((li != le) and (ri != re) and (c = comparer (*li, *ri)) == 0) {
                        ++li;
                        ++ri;
                    }
                    if (li == le) {
                        if (ri == re) {
                            return 0; // all items same and loop ended with both things at end
                        }
                        else {
                            return -1; // lhs shorter but an initial sequence of rhs
                        }
                    }
                    else if (ri == re) {
                        return 1; // rhs shorter but an initial sequence of lhs
                    }
                    else {
                        Assert (li != le and ri != re);
                        Assert (c == comparer (*li, *ri));
                        return c;
                    }
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
                    DISABLE_COMPILER_MSC_WARNING_END (6001)
                }

                template <typename T, typename ELEMENT_ELEMENT_EQUALS_COMPARER>
                bool Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs, const ELEMENT_ELEMENT_EQUALS_COMPARER& equalsComparer)
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
                    while (not li.Done () and equalsComparer (*li, *ri)) {
                        ++li;
                        ++ri;
                    }
                    return li.Done ();
                }

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                Memory::Optional<size_t> IndexOf_ (const Iterable<T>& c, ArgByValueType<T> item, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer)
                {
                    constexpr bool kUseApply_{true}; // I think apply faster due to single lock
                    if (kUseApply_) {
                        size_t n = 0;
                        return c.FindFirstThat ([&n, item, equalsComparer](ArgByValueType<T> ii) { return equalsComparer (ii, item) ? true : (n++, false); }) ? n : Memory::Optional<size_t>{};
                    }
                    else {
                        size_t n = 0;
                        for (T i : c) {
                            if (equalsComparer (i, item)) {
                                return n;
                            }
                            n++;
                        }
                        return {};
                    }
                }

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                Memory::Optional<size_t> IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer)
                {
                    size_t n = 0;
                    for (auto i = c.begin (); i != c.end (); ++i, ++n) {
                        bool foundDiff = false;
                        auto ii        = i;
                        for (T r : rhs) {
                            if (ii == c.end ()) {
                                return {};
                            }
                            if (not(equalsComparer (r, *ii))) {
                                foundDiff = true;
                                break;
                            }
                            ++ii;
                        }
                        if (not foundDiff) {
                            return n;
                        }
                    }
                    return {};
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Private_IterableUtils_inl_ */
