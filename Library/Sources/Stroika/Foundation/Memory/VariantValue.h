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
 *
 *      @todo   Fix compare of signed and unsigned int (equals) - when comparing different types (signed/unsigned)
 *              if the unsigned number is in range of valid signed numbers then can compare either way, but dont make
 *              max_uint == -some neg #.
 *
 *      @todo   Complete the conversion to Stroika types (e.g. String) - (so internally we use Stroika types) - but
 *              continue to make it easy to use this with vector/map/wstring. Started - but incomplete.
 *
 *      @todo   fixup int type serialization (bigger int types - probably need to extend VariantValue class -
 *              and maybe should for unsigned?
 *
 *              See ObjectVariantMapper code (for mapping to int types)
 *
 *              <<NOTE - PARTLY DONE - DID RIGHT CTOR overloads and right As<T> () templates for each of the 5 signed and unsigned types (read spec).
 *              <<but still todo is alt storage for each>>
 *
 *              TO FIX - ADD ADDITIONAL 'types'
 *                  //
 *                  // but be careful cuz many places with  VariantValue::Type enums
    <<review - partly done>>>
 *
 *      @todo   Need Comapare (ICompare....) support - maybe operator< and/or maybe compare (V) -> int
 *
 *      @todo   Add r-value reference -- && -- overloads for CTORs (and more) - as performance hack
 *
 *      @todo   Consider moving this class to DataExcahngeFormat module.
 *
 *      @todo   If we add ATOM class support (like HF/RFLLib Enumeration) - consider adding it here?
 *              Though probably not.
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
             *
             *  INTEGER Types for CTOR and As<T> () overloads:
             *      “signed char”, “short int”, “int”, “long int”, and “long long int”
             *      “unsigned char”, “unsigned short int”, “unsigned int”, “unsigned long int”, and “unsigned long long int”,
             *  from section 3.9.1 of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
             */
            class   VariantValue {
            private:
                /**
                 *  Internal format for storing floating point data in a VariantValue.
                 */
                typedef long double  FloatType_;

            private:
                /**
                 *  Internal format for storing int data in a VariantValue.
                 */
                typedef long long int  IntegerType_;

            private:
                /**
                 *  Internal format for storing unsigned int data in a VariantValue.
                 */
                typedef unsigned long long int  UnsignedIntegerType_;

            public:
                /**
                 * \brief   Enumeration of variant types
                 */
                enum class Type : uint8_t {
                    eNull,
                    eBoolean,

                    // From section from section 3.9.1 of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
                    //
                    //      There are five standard signed integer types : signed char, short int, int,
                    //      long int, and long long int. In this list, each type provides at least as much
                    //      storage as those preceding it in the list.
                    //      For each of the standard signed integer types, there exists a corresponding (but different)
                    //      standard unsigned integer type: unsigned char, unsigned short int, unsigned int, unsigned long int,
                    //      and unsigned long long int, each of which occupies the same amount of storage and has the
                    //      same alignment requirements.
                    //
                    //  So this ....just store in largest type

                    //
                    // but be careful cuz many places with  VariantValue::Type enums
                    eInteger,
                    eUnsignedInteger,
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
                VariantValue (signed char val);
                VariantValue (short int val);
                VariantValue (int val);
                VariantValue (long int val);
                VariantValue (long long int val);
                VariantValue (unsigned char val);
                VariantValue (unsigned short int val);
                VariantValue (unsigned int val);
                VariantValue (unsigned long int val);
                VariantValue (unsigned long long int val);
                VariantValue (float val);
                VariantValue (double val);
                VariantValue (long double val);
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
#if     qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
                VariantValue (const string& val) = delete;
                VariantValue (const char* val) = delete;
#else
                VariantValue (const string& val);       // not implemented - declared to avoid confusing automatic type conversions - use wstring/wchar_t*
                VariantValue (const char* val);         // ""
#endif

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
                 *
                 *      Supported types (RETURNTYPE) include:
                 *          o   bool
                 *          o   signed char, signed short, int, long int, long long int (any of the 5 signed int types)
                 *          o   unsigned char, unsigned short, unsigned int, unsigned long int, unsignedlong long int (any of the 5 unsigned int types)
                 *          o   float
                 *          o   double
                 *          o   long double
                 *          o   Date
                 *          o   DateTime
                 *          o   wstring
                 *          o   String
                 *          o   Mapping<String, VariantValue>
                 *          o   Sequence<VariantValue>
                 *          o   map<wstring, VariantValue>
                 *          o   vector<VariantValue>
                 */
                template    <typename   RETURNTYPE>
                nonvirtual RETURNTYPE As () const;

            private:
                nonvirtual  IntegerType_            AsInteger_ () const;
                nonvirtual  UnsignedIntegerType_    AsUnsignedInteger_ () const;
                nonvirtual  FloatType_              AsFloatType_ () const;
                nonvirtual  String                  AsString_ () const;

            public:
                /**
                 */
                nonvirtual  bool    Equals (const VariantValue& rhs, bool exactTypeMatchOnly = false) const;

            public:
                /**
                 */
                nonvirtual  bool    operator== (const VariantValue& rhs) const;

            public:
                /**
                 */
                nonvirtual  bool    operator!= (const VariantValue& rhs) const;

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


            template    <>
            bool VariantValue::As () const;
            template    <>
            signed char VariantValue::As () const;
            template    <>
            short int   VariantValue::As () const;
            template    <>
            int VariantValue::As () const;
            template    <>
            long int    VariantValue::As () const;
            template    <>
            long long int   VariantValue::As () const;
            template    <>
            unsigned char VariantValue::As () const;
            template    <>
            unsigned short int  VariantValue::As () const;
            template    <>
            unsigned int VariantValue::As () const;
            template    <>
            unsigned long int   VariantValue::As () const;
            template    <>
            unsigned long long VariantValue::As () const;
            template    <>
            float VariantValue::As () const;
            template    <>
            double VariantValue::As () const;
            template    <>
            long double VariantValue::As () const;
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
