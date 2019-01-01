/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_inl_
#define _Stroika_Foundation_Execution_Function_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Common/Compare.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** Execution::Function ***************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    template <typename CTOR_FUNC_SIG>
    inline Function<FUNCTION_SIGNATURE>::Function (const CTOR_FUNC_SIG& f)
        : fFun_ (STDFUNCTION{f})
        , fOrdering_{fFun_.template target<CTOR_FUNC_SIG> ()}
    {
        Assert ((fOrdering_ == nullptr) == (fFun_ == nullptr));
    }
    template <typename FUNCTION_SIGNATURE>
    inline Function<FUNCTION_SIGNATURE>::Function (nullptr_t)
        : fFun_{}
    {
        Assert (fOrdering_ == nullptr);
    }
    template <typename FUNCTION_SIGNATURE>
    inline Function<FUNCTION_SIGNATURE>::operator STDFUNCTION () const
    {
        return fFun_;
    }
    template <typename FUNCTION_SIGNATURE>
    template <typename... Args>
    inline typename Function<FUNCTION_SIGNATURE>::result_type Function<FUNCTION_SIGNATURE>::operator() (Args... args) const
    {
        RequireNotNull (fFun_);
        return fFun_ (forward<Args> (args)...);
    }
    template <typename FUNCTION_SIGNATURE>
    inline int Function<FUNCTION_SIGNATURE>::Compare (const Function& rhs) const
    {
        return Common::ThreeWayCompareNormalizer (fOrdering_, rhs.fOrdering_);
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
