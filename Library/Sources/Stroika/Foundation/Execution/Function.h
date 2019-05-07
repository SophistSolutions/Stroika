/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_h_
#define _Stroika_Foundation_Execution_Function_h_ 1

#include "../StroikaPreComp.h"

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
         */
        Function () = default;
        Function (nullptr_t);
        Function (const Function&) = default;
        Function (Function&&)      = default;
        template <typename CTOR_FUNC_SIG>
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
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         *  Note - this has nothing todo with the actual value of the 'target' function passed in.
         *  This is just magic associated with the object so it can be stored in a map.
         *      @todo DOCUMENT BETTER
         */
        [[deprecated ("in Stroika v2.1d24 - use Common::ThreeWayCompare () or ThreeWayComparer{} () instead")]] int Compare (const Function& rhs) const;

    private:
        STDFUNCTION fFun_;
        void*       fOrdering_{}; // captured early when we have the right type info, so we can safely compare (since Stroika v2.1d8)
    };

    /**
     *  @see Common::ThreeWayComparer<> template
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    template <typename FUNCTION_SIGNATURE>
    struct Function<FUNCTION_SIGNATURE>::ThreeWayComparer {
        nonvirtual int operator() (const Function& lhs, const Function& rhs) const;
    };

    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Function<FUNCTION_SIGNATURE>::ThreeWayComparer ()
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    template <typename FUNCTION_SIGNATURE>
    bool operator< (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
    template <typename FUNCTION_SIGNATURE>
    bool operator<= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
    template <typename FUNCTION_SIGNATURE>
    bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
    template <typename FUNCTION_SIGNATURE>
    bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t);
    template <typename FUNCTION_SIGNATURE>
    bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
    template <typename FUNCTION_SIGNATURE>
    bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t);
    template <typename FUNCTION_SIGNATURE>
    bool operator> (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
    template <typename FUNCTION_SIGNATURE>
    bool operator>= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Function.inl"

#endif /*_Stroika_Foundation_Execution_Function_h_*/
