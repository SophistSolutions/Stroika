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
 *      @todo   Add SINFAE CTOR template, so we can lose explicit map<> CTOR, and handle other
 *              cases automatically, like vector<wstring> CTOR. And/or fix KeyValuePair<> ctor so
 *              maps 'convertible' key and convertabile 'value' types.
 *
 *      @todo   Consider adding crit-section-safe version of  VariantValue.
 *              AnD OR MKAE VariantValue (KEY IS DOCUMETNATION) - threasdsafe like other container types.
 *              Might be trivial with careful use of shared_ptr - and maybe safe already?  But if not - maybe worth doing?
 *              ONE Place we can improve if this is threadsafe is use in ProgressMonitor.h
 *
 *              o   But then be extra clear this class is fully threadsafe (maybe already so).
 *
 *              o   Either document why we used  shared_ptr or use SharedByValue. I thinkt he reason
 *                  is that we never modify? So no need for SharedbyValue.
 *
 *      @todo   Need Comapare (ICompare....) support - maybe operator< and/or maybe compare (V) -> int
 *              (mostly done but review)
 *
 *      @todo   Fix VariantValue::Equals () - for case in DoRegressionTests_VariantValue_7_().
 *
 *              Key is compare of string versus int, etc.
 *
 *      @todo   Re-review the signed/unsigned compare etc code. I think its all correct, but its tricky enough to
 *              warrent a careful review
 *
 *      @todo   Add r-value reference -- && -- overloads for CTORs (and more) - as performance hack
 *
 *      @todo   If we add ATOM class support (like HF/RFLLib Enumeration) - consider adding it here?
 *              Though probably not.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


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
             * From section from section 3.9.1 of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
             *      There are five standard signed integer types : signed char, short int, int,
             *      long int, and long long int. In this list, each type provides at least as much
             *      storage as those preceding it in the list.
             *      For each of the standard signed integer types, there exists a corresponding (but different)
             *      standard unsigned integer type: unsigned char, unsigned short int, unsigned int, unsigned long int,
             *      and unsigned long long int, each of which occupies the same amount of storage and has the
             *      same alignment requirements.
             *
             *  So for now - we just store the largest signed and unsigned integer types and cast down to what
             *  the user users/requests.
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
                    eInteger,
                    eUnsignedInteger,
                    eFloat,
                    eDate,
                    eDateTime,
                    eString,
                    eArray,
                    eMap,           // Mapping<String,VariantValue>

                    Stroika_Define_Enum_Bounds(eNull, eMap)
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
                VariantValue (const string& val) = delete;
                VariantValue (const char* val) = delete;

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
                 *  then DataExchange::BadFormatException will be thrown.
                 *
                 *      Supported types (RETURNTYPE) include:
                 *          o   bool
                 *          o   signed char, signed short, int, long int, long long int (any of the 5 signed int types)
                 *          o   unsigned char, unsigned short, unsigned int, unsigned long int, unsigned long long int (any of the 5 unsigned int types)
                 *          o   float, double, long double
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
                *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                *   This coerces types.
                *   @todo - thinkout bettter and document what it means for differnt types
                *           FOR NOW - just key off first type and convert RHS to same type as LHS, where possible
                */
                nonvirtual  int      Compare (const VariantValue& rhs) const;

            public:
                /**
                 *  If exactTypeMatchOnly is true, no type coersion takes place, but by default types
                 *  are automatically coereced, if reasonable, so that they can be compared for equality.
                 *
                 *  When comparing two items, at least one of which is a floating point number,
                 *  @Math::NearlyEquals() is used (because often these values come from serializaiton/deserializaiton which
                 *  loses a tiny bit of precision).
                 */
                nonvirtual  bool    Equals (const VariantValue& rhs, bool exactTypeMatchOnly = false) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const VariantValue& rhs)
                 *  Except for ==/!= which indirect to Equals()
                 */
                nonvirtual  bool operator< (const VariantValue& rhs) const;
                nonvirtual  bool operator<= (const VariantValue& rhs) const;
                nonvirtual  bool operator> (const VariantValue& rhs) const;
                nonvirtual  bool operator>= (const VariantValue& rhs) const;
                nonvirtual  bool operator== (const VariantValue& rhs) const;
                nonvirtual  bool operator!= (const VariantValue& rhs) const;

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
