/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_MultiSet_h_
#define _Stroika_Foundation_Containers_DefaultTraits_MultiSet_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/CountedValue.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Concepts.h"

/*
 *  \file
 */

namespace Stroika::Foundation::Containers::DefaultTraits {

    /**
     *  Default MultiSet<> Traits
     */
    template <typename T>
    struct MultiSet {
        /**
         *      \brief
         */
        using CountedValueType = Common::CountedValue<T>;

        /**
         *      \brief
         */
        using CounterType = typename CountedValueType::CounterType;
    };
}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Containers_DefaultTraits_MultiSet_h_ */
