/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_AnyVariantValue_h_
#define _Stroika_Foundation_Memory_AnyVariantValue_h_  1

#include    "../StroikaPreComp.h"

#include    <memory>
#include    <type_traits>

#include    "../Configuration/Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Explain rationale for difference between VariantValue, and AnyVariantValue
 *
 *              HINT: this is useful (and why I created) for a Queue of command objects.
 *
 *      @todo   Consider making operator T () less restrictive - and work if conertable. But then not
 *              sure how to get the right type (can I do decltype (GetType())??? - in other words make a T out of
 *              the type_info? I think not...
 *
 *
 *  Awknolwedgements:
 *      I came up with this idea before looking at the BOOST any code, but when I thought of it
 *      I first checked out what boost had that was similar. Before implementing, I looked at
 *      http://www.boost.org/doc/libs/1_54_0/doc/html/any.html
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             * \brief   Simple variant-value object, with basic (variant) types.
             *
             *  @see VariantValue
             */
            class   AnyVariantValue {
            public:
                AnyVariantValue ();
                template    <typename   T>
                AnyVariantValue (T val);

            public:
                nonvirtual  const type_info&    GetType () const;

            public:
                /**
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                template    <typename   RETURNTYPE>
#if qCompilerAndStdLib_Supports_ExplicitConversionOperators
                nonvirtual explicit operator RETURNTYPE () const;
#else
                nonvirtual operator RETURNTYPE () const;
#endif

            public:
                /**
                 */
                nonvirtual  bool    Equals (const AnyVariantValue& rhs) const;

            public:
                /**
                 */
                nonvirtual  bool    operator== (const AnyVariantValue& rhs) const;

            public:
                /**
                 */
                nonvirtual  bool    operator!= (const AnyVariantValue& rhs) const;

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                struct  IRep_;

            private:
                shared_ptr<IRep_>   fVal_;

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                template    <typename T>
                struct  TIRep_;
            };




        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "AnyVariantValue.inl"

#endif  /*_Stroika_Foundation_Memory_AnyVariantValue_h_*/
