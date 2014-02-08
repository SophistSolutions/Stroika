/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 */



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

            public:
                Function () = default;
                Function (const Function&) = default;
                Function (const STDFUNCTION& f);
                template    <typename X>
                Function (const X& f)
                    : Function (STDFUNCTION (f))
                {

                }

            public:
                nonvirtual  operator STDFUNCTION () const;

            public:
                template    < typename... Args>
                nonvirtual  result_type     operator() ( Args... args ) const;

            public:
                // todo add all other compare operators and Compare() compare(), Equals()
                nonvirtual  bool operator< (const Function& rhs) const;
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
