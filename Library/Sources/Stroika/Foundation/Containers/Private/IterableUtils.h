/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_h_
#define _Stroika_Foundation_Containers_Private_IterableUtils_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Configuration/Common.h"

#include "Stroika/Foundation/Containers/Common.h"

/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *
 * Notes:
 *
 */

namespace Stroika::Foundation::Containers::Private {

    template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
    optional<size_t> IndexOf_ (const Iterable<T>& c, ArgByValueType<T> item, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer = {});

    template <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
    optional<size_t> IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs, const ELEMENT_COMPARE_EQUALS_TYPE& equalsComparer = {});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IterableUtils.inl"

#endif /*_Stroika_Foundation_Containers_Private_IterableUtils_h_ */
