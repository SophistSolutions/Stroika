/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_h_
#define _Stroika_Foundation_Memory_VariantValue_h_  1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Time/DateTime.h"



/**
 *  TODO:
 *
 *      @todo   Consider moving this class to DataExcahngeFormat module.
 *
 *      @todo   If we add ATOM class support (like HF/RFLLib Enumeration) - consider adding it here?
 *              Though probably not.
 *
 *      @todo   Consider adding more integer types, like int32, int64, etc... COM VARIANT has many more
 *              types than we do.
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {

            using   Time::Date;
            using   Time::DateTime;

            /**
             * \brief   Simple variant-value object, with basic (variant) types.
             *
             *  These objects are internally efficiently copied (shared_ptr), but have copy
             *  by value semantics (since they are never modifyable).
             *
             *  Note that it is never possible to create circular refrences (e.g. with Array or Map)
             *  types because these are constructed from existing already constructed VariantValue
             *  objects, and can never be modified thereafter.
             *
             *  Note that this VariantValue is analagous to, and inspired by, the Microsoft
             *  COM VARIANT object type.
             */
            class   VariantValue {
            public:
                /*
                 *  There are several floating point types - float, double, long double (and others?).
                 *  This selects which we use to represent a VariantValue internally, but either double
                 *  or float (maybe more) can be used to access
                 */
                typedef double  FloatType;

            public:
                /**
                 * \brief   Enumeration of variant types
                 */
            enum class Type : uint8_t {
                    eNull,
                    eBoolean,
                    eInteger,
                    eFloat,
                    eDate,
                    eDateTime,
                    eString,
                    eArray,
                    eMap,           // string to variant

                    Define_Start_End_Count(eNull, eMap)
                };

            public:
                VariantValue ();
                VariantValue (bool val);
                VariantValue (int val);
                VariantValue (float val);
                VariantValue (double val);
                VariantValue (const Date& val);
                VariantValue (const DateTime& val);
                VariantValue (const wstring& val);
                VariantValue (const wchar_t* val);
                VariantValue (const map<wstring, VariantValue>& val);
                VariantValue (const vector<VariantValue>& val);

            private:
                VariantValue (const string& val);       // not implemented - declared to avoid confusing automatic type conversions - use wstring/wchar_t*
                VariantValue (const char* val);         // ""

            public:
                nonvirtual  Type    GetType () const;

            public:
                /**
                 *  Return if the given object is logically 'empty'. The meaning of this depends
                 *  on the type of the variant. For example, eNull type is always empty. Most
                 *  other types are empty iff their native type (e.g. basic_string) is 'empty'.
                 *  A FLOAT is empty iff its std::isnan().
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 * Only these (enum Type) types supported. No generic 'As<>' implementation.
                 *  There is no generic As<T> implementation.
                 *
                 *  If the caller attempts a conversion that isn't supported, or doesn't make sense
                 *  then DataExchangeFormat::BadFormatException will be thrown.
                 */
                template    <typename   RETURNTYPE>
                nonvirtual RETURNTYPE As () const;

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
                template    <typename T, Type t>
                struct  TIRep_;
            };


            bool    Equals (const VariantValue& lhs, const VariantValue& rhs, bool exactTypeMatchOnly = false);

            bool    operator== (const VariantValue& lhs, const VariantValue& rhs);
            bool    operator!= (const VariantValue& lhs, const VariantValue& rhs);


            template    <>
            nonvirtual bool VariantValue::As () const;
            template    <>
            nonvirtual int VariantValue::As () const;
            template    <>
            nonvirtual float VariantValue::As () const;
            template    <>
            nonvirtual double VariantValue::As () const;
            template    <>
            nonvirtual Date VariantValue::As () const;
            template    <>
            nonvirtual DateTime VariantValue::As () const;
            template    <>
            nonvirtual wstring VariantValue::As () const;
            template    <>
            nonvirtual map<wstring, VariantValue> VariantValue::As () const;
            template    <>
            nonvirtual vector<VariantValue> VariantValue::As () const;

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_VariantValue_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "VariantValue.inl"
