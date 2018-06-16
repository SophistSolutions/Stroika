/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *  TODO
 *      @todo   Instead of using shared_ptr<> - we could have an INT counter (except for special case of null where we use
 *              0). That might be cheaper, and preserve the same semantics.
 *
 *      @todo   Better understand, and fix qFoundation_Execution_Function_OperatorForwardNeedsRefBug, and eliminate it
 *
 *      @todo   Consider if this should be copy-by-value (use SharedByValue instead of shared_ptr) so it more closely
 *              mimics copy behavior of std::function?
 */

/// NOT SURE WHAT's GOING ON. Seems like my bug, but I dont fully
/// understand
/// doesn't seem needed with msvc, but is with gcc/clang++
#ifndef qFoundation_Execution_Function_OperatorForwardNeedsRefBug
#define qFoundation_Execution_Function_OperatorForwardNeedsRefBug 1
#endif

namespace Stroika {
    namespace Foundation {
        namespace Execution {

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
                template <typename CTOR_FUNC_SIG>
                Function (const CTOR_FUNC_SIG& f);

            public:
                /**
                 */
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
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 *  Note - this has nothing todo with the actual value of the 'target' function passed in.
                 *  This is just magic associated with the object so it can be stored in a map.
                 *      @todo DOCUMENT BETTER
                 */
                nonvirtual int Compare (const Function& rhs) const;

            private:
                shared_ptr<STDFUNCTION> fFun_;
            };

            /**
             *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Function& rhs)
             */
            template <typename FUNCTION_SIGNATURE>
            bool operator< (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);

            /**
             *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Function& rhs)
             */
            template <typename FUNCTION_SIGNATURE>
            bool operator<= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);

            /**
             *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Function& rhs)
             */
            template <typename FUNCTION_SIGNATURE>
            bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
            template <typename FUNCTION_SIGNATURE>
            bool operator== (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t);

            /**
             *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Function& rhs)
             */
            template <typename FUNCTION_SIGNATURE>
            bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
            template <typename FUNCTION_SIGNATURE>
            bool operator!= (const Function<FUNCTION_SIGNATURE>& lhs, nullptr_t);

            /**
             *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Function& rhs)
             */
            template <typename FUNCTION_SIGNATURE>
            bool operator> (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);

            /**
             *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Function& rhs)
             */
            template <typename FUNCTION_SIGNATURE>
            bool operator>= (const Function<FUNCTION_SIGNATURE>& lhs, const Function<FUNCTION_SIGNATURE>& rhs);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Function.inl"

#endif /*_Stroika_Foundation_Execution_Function_h_*/
