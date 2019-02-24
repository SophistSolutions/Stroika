/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_h_
#define _Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_h_ 1

#include "../StroikaPreComp.h"

namespace Stroika::Foundation::Common {

    /**
     */
    struct EmptyObjectForConstructorSideEffect {
    public:
        /**
        [[no_unique_address]]

        use example from Declare
         */
        template <typename CALL>
        inline EmptyObjectForConstructorSideEffect (const CALL& c)
        {
            c ();
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EmptyObjectForConstructorSideEffect.inl"

#endif /*_Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_h_*/