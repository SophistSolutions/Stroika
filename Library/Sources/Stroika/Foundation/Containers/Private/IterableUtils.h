/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_h_
#define _Stroika_Foundation_Containers_Private_IterableUtils_h_

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Memory/Optional.h"

#include "../Common.h"

/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *
 * Notes:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {

                template <typename T, typename ELEMENT_COMPARER_TYPE>
                int ThreeWayCompare_ (const Iterable<T>& lhs, const Iterable<T>& rhs, const ELEMENT_COMPARER_TYPE& threeWayComparer);

                template <typename T, typename ELEMENT_ELEMENT_EQUALS_COMPARER>
                bool Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs, const ELEMENT_ELEMENT_EQUALS_COMPARER& equalsComparer = {});

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                Memory::Optional<size_t> IndexOf_ (const Iterable<T>& c, ArgByValueType<T> item, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer = {});

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                Memory::Optional<size_t> IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer = {});
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IterableUtils.inl"

#endif /*_Stroika_Foundation_Containers_Private_IterableUtils_h_ */
