/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_MultiSet_h_
#define _Stroika_Foundation_Containers_DefaultTraits_MultiSet_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Common/CountedValue.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/Concepts.h"

/*
 *  \file
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace DefaultTraits {

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
        }
    }
}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Containers_DefaultTraits_MultiSet_h_ */
