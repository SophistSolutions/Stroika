/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Activity_inl_
#define _Stroika_Foundation_Execution_Activity_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *********************** Activity<Characters::String> ***************************
     ********************************************************************************
     */
    inline Activity<Characters::String>::Activity (const Characters::String& arg)
        : fArg_ (arg)
    {
    }

    /*
     ********************************************************************************
     *************************** Activity<wstring_view> *****************************
     ********************************************************************************
     */
    constexpr Activity<wstring_view>::Activity (const wstring_view& arg) noexcept
        : fArg_ (arg)
    {
    }

    /*
     ********************************************************************************
     ************* LazyEvalActivity<CTOR_ARG,enable_if_t<>> *************************
     ********************************************************************************
     */
    // Cannot figure out how to move here cuz of enable_if_t stuff

    /*
     ********************************************************************************
     *************************** DeclareActivity<ACTIVITY> **************************
     ********************************************************************************
     */
    template <typename ACTIVITY>
    inline DeclareActivity<ACTIVITY>::DeclareActivity (const ACTIVITY* arg) noexcept
        : fNewTopOfStackElt_{arg, Private_::Activities_::sTop_}
    {
        // no locks needed because the variables are thread local
        Private_::Activities_::sTop_ = &fNewTopOfStackElt_;
    }
    template <typename ACTIVITY>
    inline DeclareActivity<ACTIVITY>::~DeclareActivity ()
    {
        // no locks needed because the variables are thread local
        Assert (Private_::Activities_::sTop_ == &fNewTopOfStackElt_);
        Private_::Activities_::sTop_ = Private_::Activities_::sTop_->fPrev;
    }

}

#endif /*_Stroika_Foundation_Execution_Activity_inl_*/
