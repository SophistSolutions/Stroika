/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
#if __cpp_impl_three_way_comparison >= 201907
    template <typename FUNCTION_SIGNATURE>
    inline strong_ordering Function<FUNCTION_SIGNATURE>::operator<=> (const Function& rhs) const
    {
        return fOrdering_ <=> rhs.fOrdering_;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool Function<FUNCTION_SIGNATURE>::operator== (const Function& rhs) const
    {
        return fOrdering_ == rhs.fOrdering_;
    }
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ************** Function<FUNCTION_SIGNATURE> operators **************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    inline bool operator< (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, rhs.fOrdering_) < 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator<= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, rhs.fOrdering_) <= 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, rhs.fOrdering_) == 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, nullptr) == 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, rhs.fOrdering_) != 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, nullptr) != 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator> (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, rhs.fOrdering_) > 0;
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool operator>= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs)
    {
        return Common::ThreeWayCompare (lhs.fOrdering_, rhs.fOrdering_) >= 0;
    }
#endif

}

#endif /*_Stroika_Foundation_Execution_Function_inl_*/
