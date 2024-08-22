/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Concrete/SortedAssociation_stdmultimap.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_
#define _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_

// THIS FILE IS DEPRECATED SINCE STROIKA v3.0d10 - use SortedAssociation_stdmultimap

namespace Stroika::Foundation::Containers::Concrete {

    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_stdmultimap; // avoid issue with #include deadly embrace

    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    using Association_stdmultimap [[deprecated ("Since Stroika v3.0d10 - use SortedAssociation_stdmultimap")]] =
        SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>;

}

#endif /*_Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_ */
