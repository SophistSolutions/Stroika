/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_h_
#define _Stroika_Foundation_Execution_Function_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <functional>
#include <memory>

#include "../Configuration/Common.h"

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

/// NOT SURE WHAT's GOING ON. Seems like my bug, but I don't fully
/// understand
/// doesn't seem needed with msvc, but is with gcc/clang++
#ifndef qFoundation_Execution_Function_OperatorForwardNeedsRefBug
#define qFoundation_Execution_Function_OperatorForwardNeedsRefBug 1
#endif

namespace Stroika::Foundation::Execution {

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
     *  \note   <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
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
         *        Reason for the not is_base_of_v<> restriction on CTOR/1(CTOR_FUNC_SIG&&) is to prevent compiler from
         *        instantiating that constructor template for argument subclasses of this Function type, and having those take precedence over the
         *        default X(const X&) CTOR.
         * 
         *        And also careful not to apply to non-functions.
         */
        Function () = default;
        Function (nullptr_t);
        Function (const Function&) = default;
        Function (Function&&)      = default;
        template <typename CTOR_FUNC_SIG, enable_if_t<is_convertible_v<CTOR_FUNC_SIG, function<FUNCTION_SIGNATURE>> and not is_base_of_v<Function<FUNCTION_SIGNATURE>, Configuration::remove_cvref_t<CTOR_FUNC_SIG>>>* = nullptr>
        Function (const CTOR_FUNC_SIG& f);

    public:
        /**
         */
        nonvirtual Function& operator= (Function&&) = default;
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

    private:
        STDFUNCTION fFun_;
        void*       fOrdering_{}; // captured early when we have the right type info, so we can safely compare (since Stroika v2.1d8)
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Function.inl"

#endif /*_Stroika_Foundation_Execution_Function_h_*/
