/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_inl_
#define _Stroika_Foundation_Execution_Function_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** Execution::Function ***************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    template <typename CTOR_FUNC_SIG>
    inline Function<FUNCTION_SIGNATURE>::Function (const CTOR_FUNC_SIG& f)
        : fFun_ (new STDFUNCTION (f))
    {
    }
    template <typename FUNCTION_SIGNATURE>
    inline Function<FUNCTION_SIGNATURE>::Function (nullptr_t)
        : fFun_{}
    {
    }
    template <typename FUNCTION_SIGNATURE>
    inline Function<FUNCTION_SIGNATURE>::operator STDFUNCTION () const
    {
        if (fFun_ == nullptr) {
            return nullptr;
        }
        return *fFun_;
    }
    template <typename FUNCTION_SIGNATURE>
    template <typename... Args>
    inline typename Function<FUNCTION_SIGNATURE>::result_type Function<FUNCTION_SIGNATURE>::operator() (Args... args) const
    {
        RequireNotNull (fFun_);
        return (*fFun_) (forward<Args> (args)...);
    }
    template <typename FUNCTION_SIGNATURE>
    inline int Function<FUNCTION_SIGNATURE>::Compare (const Function& rhs) const
    {
        if (fFun_.get () < rhs.fFun_.get ()) {
            return -1;
        }
        else if (fFun_.get () == rhs.fFun_.get ()) {
            return 0;
        }
        else {
            return 1;
        }
    }

    /*
     ********************************************************************************
     ************************************ operator< *********************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator< (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return lhs.Compare (rhs) < 0;
    }

    /*
     ********************************************************************************
     *********************************** operator<= *********************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator<= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }

    /*
     ********************************************************************************
     ************************************* operator== *******************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return lhs.Compare (rhs) == 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t)
    {
        return lhs.Compare (nullptr) == 0;
    }

    /*
     ********************************************************************************
     ************************************* operator!= *******************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return lhs.Compare (rhs) != 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t)
    {
        return lhs.Compare (nullptr) != 0;
    }

    /*
     ********************************************************************************
     ************************************** operator> *******************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator> (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return lhs.Compare (rhs) > 0;
    }

    /*
     ********************************************************************************
     ************************************* operator>= *******************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator>= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }

}

#endif /*_Stroika_Foundation_Execution_Function_inl_*/
