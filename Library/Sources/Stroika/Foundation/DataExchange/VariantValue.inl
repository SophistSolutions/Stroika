/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_inl_
#define _Stroika_Foundation_Memory_VariantValue_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Memory/BlockAllocated.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {

            /*
             *  NB: we use enable_shared_from_this<> for performance reasons, not for any semantic purpose
             */
            struct  VariantValue::IRep_
#if     qStroika_Foundation_DataExchange_VariantValueUsesStroikaSharedPtr_
                    : public Memory::enable_shared_from_this<VariantValue::IRep_>
#else
                    : public std::enable_shared_from_this<VariantValue::IRep_>
#endif
            {
                virtual ~IRep_ () {}
                virtual Type    GetType () const    =   0;
            };


            /*
             ********************************************************************************
             ********************************** VariantValue ********************************
             ********************************************************************************
             */
            inline  VariantValue::VariantValue ()
                : fVal_ ()
            {
            }
            inline  VariantValue::VariantValue (const vector<VariantValue>& val)
                : VariantValue (Sequence<VariantValue> (val))
            {
            }
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
            template    <>
            inline  wstring VariantValue::As () const
            {
                return AsString_ ().As<wstring> ();
            }
            template    <>
            inline  String VariantValue::As () const
            {
                return AsString_ ();
            }
            template    <typename   RETURNTYPE>
            RETURNTYPE VariantValue::As () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            inline  bool VariantValue::operator< (const VariantValue& rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline  bool VariantValue::operator<= (const VariantValue& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline  bool VariantValue::operator> (const VariantValue& rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline  bool VariantValue::operator>= (const VariantValue& rhs) const
            {
                return Compare (rhs) >= 0;
            }
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
