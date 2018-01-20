/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_h_
#define _Stroika_Foundation_Memory_VariantValue_h_ 1

#include "../StroikaPreComp.h"

#include <map>
#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Containers/Mapping.h"
#include "../Containers/Sequence.h"
#include "../Memory/BLOB.h"
#include "../Memory/SharedPtr.h"
#include "../Time/DateTime.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      @todo   XPath feature - https://github.com/SophistSolutions/Stroika/issues/110
 *
 *      @todo   POSSIBLY add support for Precision (see Characters::Float2String) - once that module has clenaned up
 *              notion of precision. Not sure how to add unobtrusively. - for As<String>()? optional param?...
 *              Maybe Float2StringOptions is optional param to As<String> ()???
 *
 *      @todo   Add SINFAE CTOR template, so we can lose explicit map<> CTOR, and handle other
 *              cases automatically, like vector<wstring> CTOR. And/or fix KeyValuePair<> ctor so
 *              maps 'convertible' key and convertabile 'value' types.
 *
 *      @todo   Use Debug::AssertExternallySynchronizedLock<> to assure not used from multiple threads.
 *
 *      @todo   Need Comapare (ICompare....) support - maybe operator< and/or maybe compare (V) -> int
 *              (mostly done but review)
 *
 *      @todo   Re-review the signed/unsigned compare etc code. I think its all correct, but its tricky enough to
 *              warrent a careful review
 *
 *      @todo   If we add ATOM class support (like HF/RFLLib Enumeration) - consider adding it here?
 *              Though probably not.
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {

            using Characters::String;
            using Containers::Mapping;
            using Containers::Sequence;
            using Time::Date;
            using Time::DateTime;

/**
             *  \def qStroika_Foundation_DataExchange_VariantValueUsesStroikaSharedPtr_
             *      If true, use Stroika's SharedPtr<> in place of std::shared_ptr<>. This is an
             *      internal implementaiton detail, and may go away as an option.
             *
             *      This defaults to @see qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr
             */
#ifndef qStroika_Foundation_DataExchange_VariantValueUsesStroikaSharedPtr_
#define qStroika_Foundation_DataExchange_VariantValueUsesStroikaSharedPtr_ qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr
#endif

            /**
             * \brief   Simple variant-value object, with (variant) basic types analagous to a value in any weakly typed language (like JavaScript, Lisp, etc)
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
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            class VariantValue {
            private:
                /**
                 *  Internal format for storing floating point data in a VariantValue.
                 */
                using FloatType_ = long double;

            private:
                /**
                 *  Internal format for storing int data in a VariantValue.
                 */
                using IntegerType_ = long long int;

            private:
                /**
                 *  Internal format for storing unsigned int data in a VariantValue.
                 */
                using UnsignedIntegerType_ = unsigned long long int;

            public:
                /**
                 * \brief   Enumeration of variant types
                 */
                enum class Type : uint8_t {
                    eNull,
                    eBLOB,
                    eBoolean,
                    eInteger,
                    eUnsignedInteger,
                    eFloat,
                    eDate,
                    eDateTime,
                    eString,
                    eArray,
                    eMap, // Mapping<String,VariantValue>

                    Stroika_Define_Enum_Bounds (eNull, eMap)
                };

            public:
                /**
                 */
                VariantValue () = default;
                VariantValue (nullptr_t);
                VariantValue (bool val);
                VariantValue (const Memory::BLOB& val);
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
                VariantValue (Mapping<String, VariantValue>&& val);
                VariantValue (const vector<VariantValue>& val);
                VariantValue (const Sequence<VariantValue>& val);
                VariantValue (const Traversal::Iterable<VariantValue>& val);
                VariantValue (Sequence<VariantValue>&& val);
                VariantValue (const VariantValue& src) = default;
                VariantValue (VariantValue&& src);

            private:
                VariantValue (const string& val) = delete;
                VariantValue (const char* val)   = delete;

            public:
                nonvirtual VariantValue& operator= (const VariantValue& rhs) = default;
                nonvirtual VariantValue& operator                            = (VariantValue&& rhs);

            public:
                /**
                 */
                nonvirtual Type GetType () const;

            public:
                /**
                 *  Return if the given object is logically 'empty'. The meaning of this depends
                 *  on the type of the variant. For example, eNull type is always empty. Most
                 *  other types are empty iff their native type (e.g. basic_string) is 'empty'.
                 *  A FLOAT is empty iff its std::isnan().
                 *  Booleans and integer types are never empty (even if zero/false).
                 */
                nonvirtual bool empty () const;

            public:
                /**
                 *  @see Characters::ToString()
                 *  Return a debug-friendly, display version of the current variant. This is not guarnateed parseable or usable except for debugging.
                 */
                nonvirtual String ToString () const;

            public:
                /**
                 *  Only these (enum Type) types supported.
                 *  There is no generic As<T> implementation.
                 *
                 *  If the caller attempts a conversion that isn't supported, or doesn't make sense
                 *  then DataExchange::BadFormatException will be thrown.
                 *
                 *      Supported (RETURNTYPE) types include:
                 *          o   bool
                 *          o   Memory::BLOB
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
                 *
                 *  \note   About As<String> () and As<wstring> ()
                 *          Conversion to these types is ALWAYS supported, no matter what the source type.
                 *          Coersion of Date/DateTime values uses ISO8601 format.
                 *          Coersion of array and map values is vaguely json-ish in format, readable, but not intended
                 *          to be regular enough to be parsable.
                 *
                 *  \note   About As<bool> ()
                 *          Coerences String value 'true' - case sensative - to true, and any integer or unsigned intger value
                 *          to true if non-zero.
                 *
                 *  \note   About As<Memory::BLOB> ()
                 *          Converts String to BLOB using base64 conversion. If thats not what you want, convert yourself.
                 *          null-type converted to empty BLOB
                 *          other types an assertion error.
                 *
                 *  \note   About As<DateTime> ()
                 *          Coerences String value to dates assuming ISO8601 string format.
                 *
                 *  \note   About As<float> ()
                 *          This converts strings and integer and floating point types. 'empty' - or the null type - is converted to nan().
                 *          Similarly, if the string cannot be converted, a nan will be returned. Other types (like Mapping) generate
                 *          an exception.
                 */
                template <typename RETURNTYPE>
                nonvirtual RETURNTYPE As () const;

            private:
                nonvirtual Memory::BLOB AsBLOB_ () const;
                nonvirtual IntegerType_ AsInteger_ () const;
                nonvirtual UnsignedIntegerType_ AsUnsignedInteger_ () const;
                nonvirtual FloatType_ AsFloatType_ () const;
                nonvirtual String AsString_ () const;

            public:
                /**
                *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                *   This coerces types.
                *   @todo - thinkout bettter and document what it means for differnt types
                *           FOR NOW - just key off first type and convert RHS to same type as LHS, where possible
                */
                nonvirtual int Compare (const VariantValue& rhs) const;

            public:
                /**
                 *  If exactTypeMatchOnly is true, no type coersion takes place, but by default types
                 *  are automatically coereced, if reasonable, so that they can be compared for equality.
                 *
                 *  When comparing two items, at least one of which is a floating point number, the other type
                 *  is coereced into a floating point number and  @Math::NearlyEquals() is used
                 *  (because often these values come from serializaiton/deserializaiton which loses a tiny bit of precision).
                 *  Note that NANs compare as equal, and Equals (L"NAN", Math::nan<double> ()) compares as true.
                 *
                 *  When comparing any other types (except Map or Array) with a String, the to types are coerenced
                 *  into Strings, and compared as strings.
                 */
                nonvirtual bool Equals (const VariantValue& rhs, bool exactTypeMatchOnly = false) const;

            private:
                struct IRep_;

            private:
#if qStroika_Foundation_DataExchange_VariantValueUsesStroikaSharedPtr_
                template <typename T>
                using SharedRepImpl_ = Memory::SharedPtr<T>;
#else
                template <typename T>
                using SharedRepImpl_ = shared_ptr<T>;
#endif

            private:
                /**
                 */
                template <typename T, typename... ARGS_TYPE>
                static SharedRepImpl_<T> MakeSharedPtr_ (ARGS_TYPE&&... args);

            private:
                SharedRepImpl_<IRep_> fVal_;

            private:
                template <typename T>
                struct TIRep_;
            };

            template <>
            bool VariantValue::As () const;
            template <>
            Memory::BLOB VariantValue::As () const;
            template <>
            signed char VariantValue::As () const;
            template <>
            short int VariantValue::As () const;
            template <>
            int VariantValue::As () const;
            template <>
            long int VariantValue::As () const;
            template <>
            long long int VariantValue::As () const;
            template <>
            unsigned char VariantValue::As () const;
            template <>
            unsigned short int VariantValue::As () const;
            template <>
            unsigned int VariantValue::As () const;
            template <>
            unsigned long int VariantValue::As () const;
            template <>
            unsigned long long VariantValue::As () const;
            template <>
            float VariantValue::As () const;
            template <>
            double VariantValue::As () const;
            template <>
            long double VariantValue::As () const;
            template <>
            Date VariantValue::As () const;
            template <>
            DateTime VariantValue::As () const;
            template <>
            wstring VariantValue::As () const;
            template <>
            String VariantValue::As () const;
            template <>
            map<wstring, VariantValue> VariantValue::As () const;
            template <>
            Mapping<String, VariantValue> VariantValue::As () const;
            template <>
            vector<VariantValue> VariantValue::As () const;
            template <>
            Sequence<VariantValue> VariantValue::As () const;

            /**
             *  operator indirects to VariantValue::Compare ()
             */
            bool operator< (const VariantValue& lhs, const VariantValue& rhs);

            /**
             *  operator indirects to VariantValue::Compare ()
             */
            bool operator<= (const VariantValue& lhs, const VariantValue& rhs);

            /**
             *  operator indirects to lhs.Equals (rhs, exactTypeMatchOnly = false) -- so conversions applied as needed to compare
             */
            bool operator== (const VariantValue& lhs, const VariantValue& rhs);

            /**
             *  operator indirects to not lhs.Equals (rhs, exactTypeMatchOnly = false) -- so conversions applied as needed to compare
             */
            bool operator!= (const VariantValue& lhs, const VariantValue& rhs);

            /**
             *  operator indirects to VariantValue::Compare ()
             */
            bool operator>= (const VariantValue& lhs, const VariantValue& rhs);

            /**
             *  operator indirects to VariantValue::Compare()
             */
            bool operator> (const VariantValue& lhs, const VariantValue& rhs);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VariantValue.inl"

#endif /*_Stroika_Foundation_Memory_VariantValue_h_*/
