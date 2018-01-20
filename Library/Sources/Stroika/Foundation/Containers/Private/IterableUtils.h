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

                // This contains has the advantage that can be interrupted between iteration steps to update the container,
                // and if calling lambdas was slow, this might be quicker.
                template <typename ELEMENT_COMPARE_EQUALS_TYPE, typename T>
                bool Contains_ByDirectIteration_ (const Iterable<T>& c, ArgByValueType<T> item);

                // This contains has the advantage that the contains operation is atomic (not quite guarnateed), but
                // also avoids re-locking the iterator for each more
                template <typename ELEMENT_COMPARE_EQUALS_TYPE, typename T>
                bool Contains_ByFunctional_ (const Iterable<T>& c, ArgByValueType<T> item);

                template <typename ELEMENT_COMPARE_EQUALS_TYPE, typename T>
                bool Contains_ (const Iterable<T>& c, ArgByValueType<T> item);

                template <typename T, typename ELEMENT_COMPARER_TYPE>
                int Compare_ (const Iterable<T>& lhs, const Iterable<T>& rhs);

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                bool Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs);

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                Memory::Optional<size_t> IndexOf_ (const Iterable<T>& c, ArgByValueType<T> item);

                template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                Memory::Optional<size_t> IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs);
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
