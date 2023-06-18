/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_inl_
#define _Stroika_Foundation_Execution_Function_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Characters/ToString.h"
#include "../Common/Compare.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ****************************** Execution::Function *****************************
     ********************************************************************************
     */
#if qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy
    template <typename FUNCTION_SIGNATURE>
    template <typename CTOR_FUNC_SIG, enable_if_t<is_convertible_v<remove_cvref_t<CTOR_FUNC_SIG>, function<FUNCTION_SIGNATURE>> and
                                                  not derived_from<remove_cvref_t<CTOR_FUNC_SIG>, Function<FUNCTION_SIGNATURE>>>*>
    inline Function<FUNCTION_SIGNATURE>::Function (CTOR_FUNC_SIG&& f)
        : fFun_{forward<CTOR_FUNC_SIG> (f)}
        , fOrdering_{fFun_ == nullptr ? OrderingType_{} : ++Private_::sFunctionObjectNextPtrID_}
    {
        Assert ((fOrdering_ == OrderingType_{}) == (fFun_ == nullptr));
    }
#elif !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename FUNCTION_SIGNATURE>
    template <typename CTOR_FUNC_SIG>
        requires (is_convertible_v<remove_cvref_t<CTOR_FUNC_SIG>, function<FUNCTION_SIGNATURE>> and
                  not derived_from<remove_cvref_t<CTOR_FUNC_SIG>, Function<FUNCTION_SIGNATURE>>)
    inline Function<FUNCTION_SIGNATURE>::Function (CTOR_FUNC_SIG&& f)
        : fFun_{forward<CTOR_FUNC_SIG> (f)}
        , fOrdering_{fFun_ == nullptr ? OrderingType_{} : ++Private_::sFunctionObjectNextPtrID_}
    {
        Assert ((fOrdering_ == OrderingType_{}) == (fFun_ == nullptr));
    }
#endif
    template <typename FUNCTION_SIGNATURE>
    inline Function<FUNCTION_SIGNATURE>::Function (nullptr_t)
        : fFun_{}
    {
        Assert (fOrdering_ == OrderingType_{});
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
    template <typename FUNCTION_SIGNATURE>
    inline Characters::String Function<FUNCTION_SIGNATURE>::ToString () const
    {
        return Characters::ToString (fOrdering_);
    }

}

#endif /*_Stroika_Foundation_Execution_Function_inl_*/
