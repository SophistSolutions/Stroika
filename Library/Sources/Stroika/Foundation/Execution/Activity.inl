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
    constexpr Activity<wstring_view>::Activity (const wstring_view& arg)
        : fArg_ (arg)
    {
    }

    /*
     ********************************************************************************
     *************************** DeclareActivity<ACTIVITY> **************************
     ********************************************************************************
     */
    template <typename ACTIVITY>
    inline DeclareActivity<ACTIVITY>::DeclareActivity (const ACTIVITY* arg)
    {
        // no locks needed because the variables are thread local
        Private_::Activities_::sTop_ = new Private_::Activities_::StackElt_{arg, Private_::Activities_::sTop_};
    }
    template <typename ACTIVITY>
    inline DeclareActivity<ACTIVITY>::~DeclareActivity ()
    {
        // no locks needed because the variables are thread local
        auto deleteMe                = Private_::Activities_::sTop_;
        Private_::Activities_::sTop_ = Private_::Activities_::sTop_->fNext;
        delete deleteMe;
    }

}

#endif /*_Stroika_Foundation_Execution_Activity_inl_*/
