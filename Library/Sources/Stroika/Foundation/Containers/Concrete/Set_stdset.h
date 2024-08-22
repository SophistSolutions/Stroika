/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include <set>

#include "Stroika/Foundation/Containers/Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_h_

// NOTE FILE DEPRECATED SINCE STROIKA v3.0d10 - use SortedSet_stdset

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class SortedSet_stdset; // avoid issue with #include deadly embrace

    /**
     *  \todo probably deprecate -- LGP 2024-08-20
     */
    template <typename T>
    using Set_stdset [[deprecated ("Since Stroika v3.0d10 - use SortedSet_stdset")]] = SortedSet_stdset<T>;

}

#endif /*_Stroika_Foundation_Containers_Concrete_Set_stdset_h_ */
