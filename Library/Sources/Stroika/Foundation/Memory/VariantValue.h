/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_h_
#define _Stroika_Foundation_Memory_VariantValue_h_  1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <vector>

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"
#include    "../Time/DateTime.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Complete the conversion to Stroika types (e.g. String) - (so internally we use Stroika types) - but
 *              continue to make it easy to use this with vector/map/wstring. Started - but incomplete.
 *
 *      @todo   Add r-value reference -- && -- overloads for CTORs (and more) - as performance hack
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


            using   Characters::String;
            using   Containers::Mapping;
            using   Containers::Sequence;
            using   Time::Date;
            using   Time::DateTime;


            /**
             * \brief   Simple variant-value object, with basic (variant) types.
             *
             *  These objects are internally efficiently copied (shared_ptr), but have copy
             *  by value semantics (since they are never modifyable).
             *
             *  Design Notes:
             *      o   Note that it is never possible to create circular references (e.g. with Array or Map)
             *          types because these are constructed from existing already constructed VariantValue
             *          objects, and can never be modified thereafter.
             *
             *      o   Note that this VariantValue is analagous to, and inspired by, the Microsoft
             *          COM VARIANT object type.
             *
             *      o   The reason we chose to interpret As<T>() as generating an exception, instead of an
             *          assertion, was based on experience using the VariantValue class. We found it was
             *          typically used extracting data from an unreliable external source (typically JSON -
             *          either from a web service or configuration data), and if that was mal-formed
             *          (since there is no JSON schema) - we would assert. At least for this usage (and
             *          that now seems the primary one) exceptions on  type mismatches seemed most helpful.
             */
            class   VariantValue {
            public:
                /**
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
                    eMap,           // Mapping<String,VariantValue>

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
                VariantValue (const String& val);
                VariantValue (const map<wstring, VariantValue>& val);
                VariantValue (const Mapping<String, VariantValue>& val);
                VariantValue (const vector<VariantValue>& val);
                VariantValue (const Sequence<VariantValue>& val);

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
            bool VariantValue::As () const;
            template    <>
            int VariantValue::As () const;
            template    <>
            float VariantValue::As () const;
            template    <>
            double VariantValue::As () const;
            template    <>
            Date VariantValue::As () const;
            template    <>
            DateTime VariantValue::As () const;
            template    <>
            wstring VariantValue::As () const;
            template    <>
            String VariantValue::As () const;
            template    <>
            map<wstring, VariantValue> VariantValue::As () const;
            template    <>
            Mapping<String, VariantValue> VariantValue::As () const;
            template    <>
            vector<VariantValue> VariantValue::As () const;
            template    <>
            Sequence<VariantValue> VariantValue::As () const;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "VariantValue.inl"

#endif  /*_Stroika_Foundation_Memory_VariantValue_h_*/
