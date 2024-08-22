/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include <set>

#include "Stroika/Foundation/Containers/MultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_

// THIS FILE IS DEPRECATED SINCE STROIKA v3.0d10 - use SortedMultiSet_stdmap

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class SortedMultiSet_stdmap; // avoid issue with #include deadly embrace

    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    using MultiSet_stdmap [[deprecated ("Since Stroika v3.0d10 - use SortedMultiSet_stdmap")]] = SortedMultiSet_stdmap<T>;

}

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_ */
