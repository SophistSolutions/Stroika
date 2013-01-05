/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_inl_
#define _Stroika_Foundation_Memory_VariantValue_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockAllocated.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            struct  VariantValue::IRep_ {
                virtual ~IRep_ () {}
                virtual Type    GetType () const    =   0;
            };


            template    <typename T, VariantValue::Type t>
            struct  VariantValue::TIRep_ : VariantValue::IRep_ {
                TIRep_ (T v)
                    : fVal (v) {
                }
                virtual Type    GetType () const override {
                    return t;
                }
                T               fVal;
                DECLARE_USE_BLOCK_ALLOCATION(TIRep_);
            };


            /*
             ********************************************************************************
             ********************************** VariantValue ********************************
             ********************************************************************************
             */
            inline  VariantValue::Type  VariantValue::GetType () const
            {
                if (fVal_.get () == nullptr) {
                    return Type::eNull;
                }
                return fVal_->GetType ();
            }
#if     !qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
            template    <typename   RETURNTYPE>
            RETURNTYPE VariantValue::As () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
#endif


            /*
             ********************************************************************************
             ************************************* Operators ********************************
             ********************************************************************************
             */
            inline  bool    operator== (const VariantValue& lhs, const VariantValue& rhs)
            {
                return Equals (lhs, rhs);
            }
            inline  bool    operator!= (const VariantValue& lhs, const VariantValue& rhs)
            {
                return not Equals (lhs, rhs);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_VariantValue_inl_*/
