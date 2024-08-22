/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/StroikaPreComp.h"
#include <set>

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_h_

// THIS FILE IS DEPRECATED SINCE STROIKA v3.0d10 - use SortedKeyedCollection_stdset

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class SortedKeyedCollection_stdset; // avoid issue with #include deadly embrace

    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    using KeyedCollection_stdset [[deprecated ("Since Stroika v3.0d10 - use SortedKeyedCollection_stdset")]] = SortedKeyedCollection_stdset<T>;

}

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_h_ */
