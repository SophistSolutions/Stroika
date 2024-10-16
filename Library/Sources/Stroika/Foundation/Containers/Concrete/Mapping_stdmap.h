/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace with SortedMapping/Mapping/SortedMapping_std
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

// NOTE FILE DEPRECATED SINCE STROIKA v3.0d10 - use SortedMapping_stdmap

namespace Stroika::Foundation::Containers::Concrete {

    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_stdmap; // avoid issue with #include deadly embrace

    /**
     *  \todo probably deprecate -- LGP 2024-08-20
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    using Mapping_stdmap [[deprecated ("Since Stroika v3.0d10 - use SortedMapping_stdmap")]] = SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>;

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
