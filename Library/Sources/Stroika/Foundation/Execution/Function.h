/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_h_
#define _Stroika_Foundation_Execution_Function_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <functional>
#include <memory>

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO
 *      @todo   Instead of using shared_ptr<> - we could have an INT counter (except for special case of null where we use
 *              0). That might be cheaper, and preserve the same semantics.
 *
 *      @todo   Better understand, and fix qFoundation_Execution_Function_OperatorForwardNeedsRefBug, and eliminate it
 *
 *      @todo   Consider if this should be copy-by-value (use SharedByValue instead of shared_ptr) so it more closely
 *              mimics copy behavior of std::function?
 */

namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Foundation::Execution {

    // SB in .inl file except to support qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    namespace Private_ {
        inline atomic<uint32_t> sFunctionObjectNextPtrID_{1};
    }

    /**
     *  IDEA is be SAME AS std::function<> but allow for operator<, a usable operator== etc...,
     *  which is an unfortunate omission from the c++ standard.
     *
     *  This should be convertable to a normal std::function<>, and fairly if not totally interoprable.
     *
     *  \note   Alias
     *      This template could have been called Callback<> - as thats principally what its used for.
     *      Callbacks you need to be able to create, and then later remove (by value) - and this class
     *      lets you create an object (callback/Function) - which can then be added to a Mapping (or Set)
     *      and then later removed by value.
     *
     *  \note   This was implemented using a shared_ptr<function<...>> instead of a directly aggregated function object
     *          until Stroika v2.1d8.
     *
     *  \note   <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    template <typename FUNCTION_SIGNATURE>
    class Function {
    public:
        using STDFUNCTION = function<FUNCTION_SIGNATURE>;

    public:
        /**
         */
        using result_type = typename STDFUNCTION::result_type;

    public:
        /**
         *  \note Implementation note:
         *        Reason for the not dervied_from<> restriction on CTOR/1(CTOR_FUNC_SIG&&) is to prevent compiler from
         *        instantiating that constructor template for argument subclasses of this Function type, and having those take precedence over the
         *        default X(const X&) CTOR.
         * 
         *        And also careful not to apply to non-functions.
         * 
         *  \note Constructor with nullptr or a null function object - will produce a UNIQUE function value (according to operator==).
         *        ANY other function object - each time you call the constructor - you will get a differnt (according to operator==) Function
         *        object. See http://stroika-bugs.sophists.com/browse/STK-960 for some of the reasoning behind this.
         */
        Function () = default;
        Function (nullptr_t);
        Function (const Function&) = default;
        Function (Function&&)      = default;
#if qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy
        template <typename CTOR_FUNC_SIG, enable_if_t<is_convertible_v<remove_cvref_t<CTOR_FUNC_SIG>, function<FUNCTION_SIGNATURE>> and
                                                      not derived_from<remove_cvref_t<CTOR_FUNC_SIG>, Function<FUNCTION_SIGNATURE>>>* = nullptr>
        Function (CTOR_FUNC_SIG&& f);
#else
        template <typename CTOR_FUNC_SIG>
            requires (is_convertible_v<remove_cvref_t<CTOR_FUNC_SIG>, function<FUNCTION_SIGNATURE>> and
                      not derived_from<remove_cvref_t<CTOR_FUNC_SIG>, Function<FUNCTION_SIGNATURE>>)
        Function (CTOR_FUNC_SIG&& f)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : fFun_{forward<CTOR_FUNC_SIG> (f)}
            , fOrdering_{fFun_ == nullptr ? OrderingType_{} : ++Private_::sFunctionObjectNextPtrID_}
        {
            Assert ((fOrdering_ == OrderingType_{}) == (fFun_ == nullptr));
        }
#endif
#endif
        ;

    public:
        /**
         */
        nonvirtual Function& operator= (Function&&)      = default;
        nonvirtual Function& operator= (const Function&) = default;

    public:
        /**
         */
        nonvirtual operator STDFUNCTION () const;

    public:
        /**
         */
        template <typename... Args>
        nonvirtual result_type operator() (Args... args) const;

    public:
        struct ThreeWayComparer;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Function& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const Function& rhs) const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

    private:
        STDFUNCTION fFun_;

    private:
        /*
         *  Before Stroika 2.1.11, we used f.template target<remove_cvref_t<CTOR_FUNC_SIG>> ()
         *  as OrderingType_. But this caused problems on g++-10 release builds (at least inside WTF). Not sure
         *  exactly how this happened, but sometimes different lambdas produced the same address. And the docs
         *  for std::function<> suggest this is possible.
         * 
         *  So just avoid altogether. Simply store a separate number ID.
         */
        using OrderingType_ = uint32_t;
        OrderingType_ fOrdering_{}; // captured early when we have the right type info, so we can safely compare, and print
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Function.inl"

#endif /*_Stroika_Foundation_Execution_Function_h_*/
