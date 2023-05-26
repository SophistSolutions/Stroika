/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
     ************************************** Activity ********************************
     ********************************************************************************
     */
    template <typename STRINGISH_T>
    constexpr Activity<STRINGISH_T>::Activity (const STRINGISH_T& arg)
        : fArg_{arg}
    {
    }
    template <typename CTOR_ARG>
    Characters::String Activity<CTOR_ARG>::AsString () const
    {
        return fArg_;
    }

    /*
     ********************************************************************************
     ************* LazyEvalActivity<CTOR_ARG,enable_if_t<>> *************************
     ********************************************************************************
     */
    template <typename CTOR_ARG>
    LazyEvalActivity<CTOR_ARG>::LazyEvalActivity (const CTOR_ARG& arg)
        requires (is_invocable_r_v<Characters::String, CTOR_ARG>)
        : fArg_{arg}
    {
    }
    template <typename CTOR_ARG>
    Characters::String LazyEvalActivity<CTOR_ARG>::AsString () const
    {
        return fArg_ (); // what makes this more efficient is that we can just capture data in a lambda (by reference)
                         // and just invoke that logic during exception processing when we need to convert the activity to a string rep
    }
    /*
     ********************************************************************************
     *************************** DeclareActivity<ACTIVITY> **************************
     ********************************************************************************
     */
    template <typename ACTIVITY>
    inline DeclareActivity<ACTIVITY>::DeclareActivity (const ACTIVITY* activity) noexcept
        : fNewTopOfStackElt_{activity, Private_::Activities_::sTop_}
    {
        // no locks needed because the variables are thread local
        if (activity != nullptr) {
            Private_::Activities_::sTop_ = &fNewTopOfStackElt_;
        }
    }
    template <typename ACTIVITY>
    inline DeclareActivity<ACTIVITY>::~DeclareActivity ()
    {
        if (fNewTopOfStackElt_.fActivity != nullptr) {
            // no locks needed because the variables are thread local
            Assert (Private_::Activities_::sTop_ == &fNewTopOfStackElt_);
            Private_::Activities_::sTop_ = Private_::Activities_::sTop_->fPrev;
        }
    }

}

#endif /*_Stroika_Foundation_Execution_Activity_inl_*/
