/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_inl_
#define _Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ******************** Common::EmptyObjectForConstructorSideEffect ***************
     ********************************************************************************
     */
    template <typename CALL>
    inline EmptyObjectForConstructorSideEffect::EmptyObjectForConstructorSideEffect (const CALL& c)
    {
        c ();
    }

}

#endif /*_Stroika_Foundation_Common_EmptyObjectForConstructorSideEffect_inl_*/