/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Function_h_
#define _Stroika_Foundation_Execution_Function_h_  1

#include    "../StroikaPreComp.h"

#include    <functional>
#include    <memory>

#include    "../Configuration/Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  TODO
 *      @todo   Better understand, and fix qFoundation_Execution_Function_OperatorForwardNeedsRefBug, and eliminate it
 */



/// NOT SURE WHAT's GOING ON. Seems like my bug, but I dont fully
/// understand
/// doesnt seem needed with msvc, but is with gcc/clang++
#ifndef qFoundation_Execution_Function_OperatorForwardNeedsRefBug
#define qFoundation_Execution_Function_OperatorForwardNeedsRefBug           1
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  IDEA is be SAME AS std::function<> but allow for operator< etc...
             *
             *  \note   Alias
             *      This template could have been called Callback<> - as thats principally what its used for.
             *      Callbacks you need to be able to create, and then later remove (by value) - and this class
             *      lets you create an object (callback/Function) - which can then be added to a Mapping (or Set)
             *      and then later removed by value.
             */
            template    <typename FUNCTION_SIGNATURE>
            class   Function {
            public:
                using STDFUNCTION   =   function<FUNCTION_SIGNATURE>;
                using result_type   =   typename STDFUNCTION::result_type;
                //using argument_type   =   typename STDFUNCTION::argument_type;
                //using first_argument_type   =   typename STDFUNCTION::first_argument_type;
                //using second_argument_type   =   typename STDFUNCTION::second_argument_type;

            public:
                Function () = default;
                Function (const Function&) = default;
                template    <typename CTOR_FUNC_SIG>
                Function (const CTOR_FUNC_SIG& f);

            public:
                nonvirtual  operator STDFUNCTION () const;

            public:
                template    <typename... Args>
                nonvirtual  result_type     operator() ( Args... args ) const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 *  Note - this has nothing todo with the actual value of the 'target' function passed in.
                 *  This is just magic associated with the object so it can be stored in a map.
                 *      @todo DOCUMENT BETTER
                 */
                nonvirtual  int Compare (const Function& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Atom& rhs)
                 */
                nonvirtual  bool operator< (const Function& rhs) const;
                nonvirtual  bool operator<= (const Function& rhs) const;
                nonvirtual  bool operator> (const Function& rhs) const;
                nonvirtual  bool operator>= (const Function& rhs) const;
                nonvirtual  bool operator== (const Function& rhs) const;
                nonvirtual  bool operator!= (const Function& rhs) const;

            private:
                shared_ptr<STDFUNCTION> fFun_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Function.inl"

#endif  /*_Stroika_Foundation_Execution_Function_h_*/
