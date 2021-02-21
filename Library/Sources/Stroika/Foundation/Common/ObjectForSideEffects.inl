/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_ObjectForSideEffects_inl_
#define _Stroika_Foundation_Common_ObjectForSideEffects_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ******************** Common::EmptyObjectForSideEffects ***************
     ********************************************************************************
     */
    template <typename CONSTRUCTOR_CALL>
    inline EmptyObjectForSideEffects::EmptyObjectForSideEffects (CONSTRUCTOR_CALL&& ctorCall)
    {
        forward<CONSTRUCTOR_CALL> (ctorCall) ();
    }

    /*
     ********************************************************************************
     ******************** Common::ObjectForSideEffects ********************
     ********************************************************************************
     */
    template <typename CONSTRUCTOR_CALL, typename DESTRUCTOR_CALL>
    inline ObjectForSideEffects::ObjectForSideEffects (CONSTRUCTOR_CALL&& ctorCall, DESTRUCTOR_CALL&& dtorCall)
        : fDestructor_ (dtorCall)   // no {} to allow converison
    {
        forward<CONSTRUCTOR_CALL> (ctorCall) ();
    }
    inline ObjectForSideEffects::~ObjectForSideEffects ()
    {
        fDestructor_ ();
    }

}

#endif /*_Stroika_Foundation_Common_ObjectForSideEffects_inl_*/