/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_inl_
#define _Stroika_Foundation_Containers_Private_IterableUtils_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::Private {

    template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
    optional<size_t> IndexOf_ (const Iterable<T>& c, ArgByValueType<T> item, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer)
    {
        constexpr bool kUseApply_{true}; // I think apply faster due to single lock
        if (kUseApply_) {
            size_t n = 0;
            return c.FindFirstThat ([&n, item, equalsComparer] (ArgByValueType<T> ii) { return equalsComparer (ii, item) ? true : (n++, false); }) ? n : optional<size_t>{};
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
    optional<size_t> IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer)
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

#endif /* _Stroika_Foundation_Containers_Private_IterableUtils_inl_ */
