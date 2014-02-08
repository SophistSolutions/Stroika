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
            template    <typename FUNCTION_SIGNATURE >
            struct  Function {

                using STDFUNCTION = function<FUNCTION_SIGNATURE>;

                using result_type = typename STDFUNCTION::result_type;

                //using FUNCTION_SIGNATURE = function<R(Args...)>;


                Function (const STDFUNCTION& f)
                    : fFun_ (new STDFUNCTION (f))
                {
                }
                Function (const Function&) = default;
                Function () = default;



                operator STDFUNCTION ()
                {
                    if (fFun_ == nullptr) {
                        return STDFUNCTION ();
                    }
                    return *fFun_;
                }
                operator const STDFUNCTION () const
                {
                    if (fFun_ == nullptr) {
                        return STDFUNCTION ();
                    }
                    return *fFun_;
                }

                template< typename... Args>
                result_type operator()( Args... args ) const
                {
                    RequireNotNull (fFun_);
                    return (*fFun_) (args...);
                }
#if 0
                R operator()( Args... args ) const
                {
                    return (*fFun_) (args...);
                }
#endif


                // todo add all other compare operators and Compare() compare(), Equals()
                bool operator< (const Function& rhs) const
                {
                    return fFun_ < rhs.fFun_;
                }
                bool operator== (const Function& rhs) const
                {
                    return fFun_ == rhs.fFun_;
                }
                bool operator!= (const Function& rhs) const
                {
                    return fFun_ != rhs.fFun_;
                }
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
