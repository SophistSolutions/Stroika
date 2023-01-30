/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_h_
#define _Stroika_Foundation_Memory_VariantValue_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <map>
#include <vector>

#if __has_include("boost/json/value.hpp")
#include <boost/json/value.hpp>
#endif

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
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using Time::Date;
    using Time::DateTime;

    /**
     *  \def kVariantValueUsesStroikaSharedPtr_
     *      If true, use Stroika's SharedPtr<> in place of std::shared_ptr<>. This is an
     *      internal implementation detail, and may go away as an option.
     *
     *      This defaults to @see Memory::kSharedPtr_IsFasterThan_shared_ptr
     */
    constexpr bool kVariantValueUsesStroikaSharedPtr_ = Memory::kSharedPtr_IsFasterThan_shared_ptr;

    /**
     * \brief   Simple variant-value object, with (variant) basic types analagous to a value in any weakly typed language (like JavaScript, Lisp, etc)
     *
     *  These objects are internally efficiently copied (shared_ptr), but have copy
     *  by value semantics (since they are never modifyable).
     *
     *  Design Notes:
     *      o   This is very similar to std::variant, but generally simpler to use because of brevety,
     *          and a well chosen (see COM, or JSON) but useful predefined set of things that are shared in the
     *          variant.
     *
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
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          @todo UPDATE DOCS FOR EQUALS COMPARER AND SHARE APPRORIATE DOCS FOR THREEWAY COMPARER AND SAY WHAT PART IS FOR 
     ANY COMPARER...
     *
     *          EQUALSCOMPARER:
     *
     *          If exactTypeMatchOnly is true, no type coersion takes place, but by default types
     *          are automatically coereced, if reasonable, so that they can be compared for equality.
     *
     *          When comparing two items, at least one of which is a floating point number, the other type
     *          is coereced into a floating point number and  @Math::NearlyEquals() is used
     *          (because often these values come from serializaiton/deserializaiton which loses a tiny bit of precision).
     *          Note that NANs compare as equal, and Equals ("NAN", Math::nan<double> ()) compares as true.
     *
     *          When comparing any other types (except Map or Array) with a String, the to types are coerceced
     *          into Strings, and compared as strings.
     *
     *  TODO:
     *
     *      @todo   XPath / JPath / JSONPath feature in DataExchange::VariantValue - https://github.com/SophistSolutions/Stroika/issues/110
     *
     *      @todo   POSSIBLY add support for Precision (see Characters::Float2String) - once that module has clenaned up
     *              notion of precision. Not sure how to add unobtrusively. - for As<String>()? optional param?...
     *              Maybe Float2StringOptions is optional param to As<String> ()???
     *
     *      @todo   Add SINFAE CTOR template, so we can lose explicit map<> CTOR, and handle other
     *              cases automatically, like vector<wstring> CTOR. And/or fix KeyValuePair<> ctor so
     *              maps 'convertible' key and convertabile 'value' types.
     *
     *      @todo   Use Debug::AssertExternallySynchronizedMutex<> to assure not used from multiple threads.
     *
     *      @todo   Re-review the signed/unsigned compare etc code. I think its all correct, but its tricky enough to
     *              warrent a careful review
     *
     *      @todo   If we add ATOM class support (like HF/RFLLib Enumeration) - consider adding it here?
     *              Though probably not.
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
         *
         *  \note Design Note
         *        It is important that ALL these types can easily and naturally be represented in JSON, but that
         *        the types here are a strict superset of the types that are native to JSON, so that roundtripping
         *        through JSON will not always produce the exact same object. For example, if you have BLOB, and write
         *        it in JSON, it might be represented as a string (or array of bytes). When you map back, its type will be
         *        that type its represented as. But - however its represented, if you say 
         *              T x;
         *              VariantValue v = x;
         *              VariantValue v2 = fromJSON (toJSON (v));
         *              v2 == v1;  // maybe false
         *              T y = v2.As<T> (); // will produce value x == y
         *
         *  \note   Configuration::DefaultNames<> supported
         * 
         *  \note   the Normalize () method can be used to return the limited subset of information that appears in JSON
         *          (but beware, that also sorts the mappings).
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
        static constexpr Type eNull            = Type::eNull;
        static constexpr Type eBLOB            = Type::eBLOB;
        static constexpr Type eBoolean         = Type::eBoolean;
        static constexpr Type eInteger         = Type::eInteger;
        static constexpr Type eUnsignedInteger = Type::eUnsignedInteger;
        static constexpr Type eFloat           = Type::eFloat;
        static constexpr Type eDate            = Type::eDate;
        static constexpr Type eDateTime        = Type::eDateTime;
        static constexpr Type eString          = Type::eString;
        static constexpr Type eArray           = Type::eArray;
        static constexpr Type eMap             = Type::eMap;

    public:
        /**
         *  \brief construct a VariantValue from most any 'basic type' you would expect to find in a weakly typed language (e.g. lisp, javascript)
         *
         *  When constructing a 'stringish' VariantValue, any arguments used to construct a String object maybe used, with the same constraints
         *  (e.g. req basic_string_view<char> MUST contain only ascii text)
         * 
         *  \note   Most constructors are not explicit to make it more easy to assign to (could do operator= for that)?
         *          A few are marked as explicit, only because its too easy to accidentally invoke 
         *          (as with https://stroika.atlassian.net/browse/STK-739) - Iterable<VariantValue> or Set<VariantValue> etc...
         *          Roughly, explicit types can be used directly, and composite (aggregating) types are explicit.
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
        template <Characters::ConvertibleToString STRINGISH_T>
        VariantValue (STRINGISH_T&& val)
            requires (not is_same_v<remove_cvref_t<STRINGISH_T>, String>);
        VariantValue (const String& val);
        explicit VariantValue (const map<wstring, VariantValue>& val);
        explicit VariantValue (Mapping<String, VariantValue>&& val);
        explicit VariantValue (const Mapping<String, VariantValue>& val);
        explicit VariantValue (const vector<VariantValue>& val);
        explicit VariantValue (Sequence<VariantValue>&& val);
        explicit VariantValue (const Sequence<VariantValue>& val);
        explicit VariantValue (const Traversal::Iterable<VariantValue>& val);
        VariantValue (const VariantValue& src)     = default;
        VariantValue (VariantValue&& src) noexcept = default;
        template <typename T>
        VariantValue (const optional<T>& val)
            requires (is_convertible_v<T, VariantValue>);
#if __has_include("boost/json/value.hpp")
        VariantValue (const boost::json::value& val);
#endif

    private:
        VariantValue (const char* val) = delete;

    public:
        /**
         *  Assign anything to a VariantValue you can construct a VariantValue with.
         */
        nonvirtual VariantValue& operator= (VariantValue&& rhs) noexcept = default;
        nonvirtual VariantValue& operator= (const VariantValue& rhs)     = default;
        template <typename T>
        nonvirtual VariantValue& operator= (T&& val)
            requires (requires (T x) { VariantValue{x}; });

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
         *  Maps and arrays are empty iff they contain no elements;
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  \brief return true iff value GetType () != null;
         *
         *  \note - this is NOT the same as not empty ()!
         */
        nonvirtual explicit operator bool () const;

    public:
        /**
         *  @see Characters::ToString()
         *  Return a debug-friendly, display version of the current variant. This is not guaranteed parseable or usable except for debugging.
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  Only these (enum Type) types supported.
         *  There is no generic As<T> implementation.
         *
         *  If the caller attempts a conversion that isn't supported, or doesn't make sense
         *  then DataExchange::BadFormatException will be thrown (not assertion error).
         *
         *      Only specifically specialized variants are supported. Supported (RETURNTYPE) types include:
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
         *          o   boost::json::value  (iff __has_include("boost/json/value.hpp"))
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
         *          If type=eBLOB, return that. If type = null, return empty blob.
         *          Else, converts any type to String, and use base64 conversion.
         *
         *  \note   About As<Date> (), About As<DateTime> ()
         *          Null maps to empty Date.DateTime; 
         *          String value mapped to Date/DateTIme assuming ISO8601 string format  - may return empty or exception on failure.
         *
         *  \note   About As<float> (), As<double> (), As<long double> ()
         *          This converts strings and integer and floating point types. 'empty' - or the null type - is converted to nan().
         *          Similarly, if the string cannot be converted, a nan will be returned. Other types (like Mapping) generate
         *          an exception.
         */
        template <typename RETURNTYPE>
        nonvirtual RETURNTYPE As () const;

    public:
        /**
         *  Return true if this VariantValue would be convertible to the argument type.
         *  Note - not just potentially convertible, but the data is actually formatted to allow the conversion.
         *  So for example, if to is DateTime, then an ill formed string would NOT be IsConvertibleTo
         * 
         *  @see ConvertTo
         */
        nonvirtual bool IsConvertibleTo (Type to) const;

    public:
        /**
         *  \brief Return this VariantValue converted to the given type (as if by As<T> for the T appropriate to 'Type to')
         * 
         *  This will throw DataExchange::BadFormatException if the conversion doesn't make sense.
         */
        nonvirtual VariantValue ConvertTo (Type to) const;

    public:
        /**
         *  Return a (possibly new, possibly same) object with certain 'features' standardized. Essentially this converts to basic
         *  JSON-writable types. So BLOB, and Date, etc, converted to string, integers converted to Float (number), nans converted
         *  to strings, etc.
         * 
         *  This also produced 'sorted' mappings.
         * 
         *  You generally dont need to use this, but its helpful for the definition of equality and comparison.
         */
        nonvirtual VariantValue Normalize () const;

    private:
        nonvirtual Memory::BLOB         AsBLOB_ () const;
        nonvirtual IntegerType_         AsInteger_ () const;
        nonvirtual UnsignedIntegerType_ AsUnsignedInteger_ () const;
        nonvirtual FloatType_           AsFloatType_ () const;
        nonvirtual String               AsString_ () const;

    public:
        /**
         * \brief compares as if first normalized with Normalize()
         */
        nonvirtual strong_ordering operator<=> (const VariantValue& rhs) const;

    public:
        /**
         * \brief compares as if first normalized with Normalize()
         */
        nonvirtual bool operator== (const VariantValue& rhs) const;

    public:
        struct EqualsComparer;

    public:
        struct ThreeWayComparer;

    private:
        struct IRep_;

    private:
        template <typename T>
        using SharedRepImpl_ = conditional_t<kVariantValueUsesStroikaSharedPtr_, Memory::SharedPtr<T>, shared_ptr<T>>;

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

    private:
        static const SharedRepImpl_<IRep_> kFalseRep_; // avoid even cheap needless allocations
        static const SharedRepImpl_<IRep_> kTrueRep_;
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
     *  \brief Compares values as if first normalized with Normalize () method
     * 
     *  \note Before Stroika v3.0d1, EqualsComparer had an fExactTypeMatchOnly option, which defaulted false.
     *        This did various not clearly specified type coersions - being expensive, and buggy, and confusing.
     * 
     *        The trickiest part is that to do this properly, we needed to pass along the flag (through default constructors
     *        using thread_local storage trick) - and that was never done.
     * 
     *        I know of no use-case for this functionality, its ambiguous, and costly. So we lose it.
     * 
     *        The ONLY remaining coertions that are done in comparing, are that if two numbers (int,unsigned, float) are compared
     *        they are first promoted.
     */
    struct VariantValue::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr EqualsComparer ();
        nonvirtual bool operator() (const VariantValue& lhs, const VariantValue& rhs) const;
    };

    /**
     *  \brief Compares values as if first normalized with Normalize () method
     * 
     *  \note Before Stroika v3.0d1, EqualsComparer had an fExactTypeMatchOnly option, which defaulted false.
     *        This did various not clearly specified type coersions - being expensive, and buggy, and confusing.
     * 
     *        The trickiest part is that to do this properly, we needed to pass along the flag (through default constructors
     *        using thread_local storage trick) - and that was never done.
     * 
     *        I know of no use-case for this functionality, its ambiguous, and costly. So we lose it.
     * 
     *        The ONLY remaining coertions that are done in comparing, are that if two numbers (int,unsigned, float) are compared
     *        they are first promoted.
     */
    struct VariantValue::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr ThreeWayComparer ();
        nonvirtual strong_ordering operator() (const VariantValue& lhs, const VariantValue& rhs) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VariantValue.inl"

#endif /*_Stroika_Foundation_Memory_VariantValue_h_*/
