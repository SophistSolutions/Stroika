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
            template    <>
            inline  signed char VariantValue::As () const
            {
                return static_cast<signed char> (AsInteger_ ());
            }
            template    <>
            inline  short int   VariantValue::As () const
            {
                return static_cast<short> (AsInteger_ ());
            }
            template    <>
            inline  int VariantValue::As () const
            {
                return static_cast<int> (AsInteger_ ());
            }
            template    <>
            inline  long int    VariantValue::As () const
            {
                return static_cast<long int> (AsInteger_ ());
            }
            template    <>
            inline  long long int   VariantValue::As () const
            {
                return static_cast<long long int> (AsInteger_ ());
            }
            template    <>
            inline  unsigned char VariantValue::As () const
            {
                return static_cast<unsigned char> (AsUnsignedInteger_ ());
            }
            template    <>
            inline  unsigned short int  VariantValue::As () const
            {
                return static_cast<unsigned short int> (AsUnsignedInteger_ ());
            }
            template    <>
            inline  unsigned int VariantValue::As () const
            {
                return static_cast<unsigned int> (AsUnsignedInteger_ ());
            }
            template    <>
            inline  unsigned long int   VariantValue::As () const
            {
                return static_cast<unsigned long int> (AsUnsignedInteger_ ());
            }
            template    <>
            inline  unsigned long long  int VariantValue::As () const
            {
                return static_cast<unsigned long long int> (AsUnsignedInteger_ ());
            }
            template    <>
            inline  float VariantValue::As () const
            {
                return static_cast<float> (AsFloatType_ ());
            }
            template    <>
            inline  double VariantValue::As () const
            {
                return static_cast<double> (AsFloatType_ ());
            }
            template    <>
            inline  long double VariantValue::As () const
            {
                return static_cast<long double> (AsFloatType_ ());
            }
#if     !qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
            template    <typename   RETURNTYPE>
            RETURNTYPE VariantValue::As () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
#endif
            inline  bool    VariantValue::operator== (const VariantValue& rhs) const
            {
                return Equals (rhs);
            }
            inline  bool    VariantValue::operator!= (const VariantValue& rhs) const
            {
                return not Equals (rhs);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_VariantValue_inl_*/
