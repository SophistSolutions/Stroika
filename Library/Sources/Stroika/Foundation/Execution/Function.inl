/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
     ****************************** Execution::Function *****************************
     ********************************************************************************
     */
    template <typename FUNCTION_SIGNATURE>
    template <typename CTOR_FUNC_SIG, enable_if_t<is_convertible_v<CTOR_FUNC_SIG, function<FUNCTION_SIGNATURE>> and not is_base_of_v<Function<FUNCTION_SIGNATURE>, Configuration::remove_cvref_t<CTOR_FUNC_SIG>>>*>
    inline Function<FUNCTION_SIGNATURE>::Function (const CTOR_FUNC_SIG& f)
        : fFun_ (STDFUNCTION (f)) // use non-uniform-initialzation convert to STDFUNCTION so we can get implicit conversions here
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
    inline strong_ordering Function<FUNCTION_SIGNATURE>::operator<=> (const Function& rhs) const
    {
#if qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy
        if (fOrdering_ < rhs.fOrdering_) {
            return strong_ordering::less;
        }
        else if (fOrdering_ > rhs.fOrdering_) {
            return strong_ordering::greater;
        }
        return strong_ordering::equal;
#else
        return fOrdering_ <=> rhs.fOrdering_;
#endif
    }
    template <typename FUNCTION_SIGNATURE>
    inline bool Function<FUNCTION_SIGNATURE>::operator== (const Function& rhs) const
    {
        return fOrdering_ == rhs.fOrdering_;
    }

}

#endif /*_Stroika_Foundation_Execution_Function_inl_*/
