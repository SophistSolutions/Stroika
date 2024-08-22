/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include <set>

#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_h_

// THIS FILE IS DEPRECATED SINCE STROIKA v3.0d10 - use SortedCollection_stdmultiset

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class SortedCollection_stdmultiset; // avoid issue with #include deadly embrace

    template <typename T>
    using Collection_stdmultiset [[deprecated ("Since Stroika v3.0d10 - use SortedCollection_stdmultiset")]] = SortedCollection_stdmultiset<T>;

}

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_h_ */
