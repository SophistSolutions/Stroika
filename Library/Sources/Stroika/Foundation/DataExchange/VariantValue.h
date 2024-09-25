/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_VariantValue_h_
#define _Stroika_Foundation_Memory_VariantValue_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <concepts>
#include <map>
#include <vector>

#if qHasFeature_boost
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wstringop-overflow\""); // for g++-13 g++-release-sanitize_address_undefined warning: 'long unsigned int __atomic_sub_fetch_8(volatile void*, long unsigned int, int)' writing 8 bytes into a region of size 0 overflows the destination [-Wstringop-overflow=
#include <boost/json/value.hpp>
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wstringop-overflow\"");
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Time/DateTime.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using Memory::BLOB;
    using Time::Date;
    using Time::DateTime;

    class VariantValue;
    namespace Private_ {
        template <typename T>
        concept IVariantValueAsBasic_ =
            Common::IAnyOf<T, bool, BLOB, Date, DateTime, wstring, String, Mapping<String, VariantValue>, map<wstring, VariantValue>, Sequence<VariantValue>, vector<VariantValue>>
#if qHasFeature_boost
            or same_as<T, boost::json::value>
#endif
            or integral<T> or floating_point<T>;
    }

    /**
     * \brief   Simple variant-value (case variant union) object, with (variant) basic types analogous to a value in any weakly typed language (like JavaScript, Lisp, etc)
     *
     *  These objects are internally efficiently copied (shared_ptr), but have copy
     *  by value semantics (since they are never modifiable).
     *
     *  Design Notes:
     *      o   This is similar to std::variant, but generally simpler to use because of brevity,
     *          and a well chosen (see COM, or JSON) but useful predefined set of things that are shared in the
     *          variant.
     *
     *      o   Note that it is never possible to create circular references (e.g. with Array or Map)
     *          types because these are constructed from existing already constructed VariantValue
     *          objects, and can never be modified thereafter.
     *
     *      o   Note that this VariantValue is analogous to, and inspired by, the Microsoft
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
     *          o static_assert (totally_ordered<VariantValue>);    // but notice that such comparisons expensive, as they imply normalizing
     * 
     *          @todo UPDATE DOCS FOR EQUALS COMPARER AND SHARE APPRORIATE DOCS FOR THREEWAY COMPARER AND SAY WHAT PART IS FOR 
     ANY COMPARER...
     *
     *          EQUALSCOMPARER:
     *
     *          If exactTypeMatchOnly is true, no type coercion takes place, but by default types
     *          are automatically coerced, if reasonable, so that they can be compared for equality.
     *
     *          When comparing two items, at least one of which is a floating point number, the other type
     *          is coerced into a floating point number and  @Math::NearlyEquals() is used
     *          (because often these values come from serialization/deserialization which loses a tiny bit of precision).
     *          Note that NANs compare as equal, and Equals ("NAN", Math::nan<double> ()) compares as true.
     *
     *          When comparing any other types (except Map or Array) with a String, the to types are coerced
     *          into Strings, and compared as strings.
     * 
     * 
     *  \note - VariantValue can be used to 'normalize' various type representations for externalization in String formats.
     *          Not perfect for all types (like string) - but does about as well as you can reasonably... And often good enuf.
     * 
     *          For example, dates written out as ISO-2022 strings, and read back as such. Numbers - like NAN written portably and read back.
     *          For structured types (like arrays) - but explicitly use JSON reader/writer ;-).
     * 
     *  \par Example Usage
     *      \code
     *          optional<String> x1;
     *          optional<Date> x2;
     * 
     *          String representAs1 = VariantValue{x1}.As<String> ();
     *          String representAs2 = VariantValue{x12}.As<String> ();
     * 
     *          x1 = VariantValue{representAs1}.As<optional<String>> ();
     *          x2= VariantValue{representAs2}.As<optional<String>> ();
     *      \endcode    
     * 
     *  \par Example Usage
     *      \code
     *          auto roundTrip = [] (auto tValue) {
     *              using T = remove_cvref_t<decltype (tValue)>;
     *              String representation = VariantValue{tValue}.As<String> ();
     *              return VariantValue{representation}.As<T> ();
     *          };
     *          EXPECT_EQ (roundTrip ("v"_k), "v");
     *          EXPECT_EQ (roundTrip (5), 5);
     *          EXPECT_EQ (roundTrip (optional<int>{}), optional<int>{});
     *          EXPECT_EQ (roundTrip (optional<Date>{}), optional<Date>{});
     *          constexpr DateTime kT1_ = DateTime{Date{January / 3 / 1944}};
     *          EXPECT_EQ (roundTrip (kT1_), kT1_);
     *
     *          // But doesn't work perfectly. Empty string and optional<String>{} get represented as the same so that's ambiguous
     *          EXPECT_EQ (roundTrip (String{}), String{});
     *          EXPECT_EQ (roundTrip (optional<String>{}), nullopt);
     *          EXPECT_EQ (roundTrip (optional<String>{String{}}), nullopt);    // oops - but really how could it tell?
     *      \endcode    
     * 
     *  \note Satisfies Concepts:
     *      o   static_assert (regular<VariantValue);
     * 
     *  TODO:
     *
     *      @todo   XPath / JPath / JSONPath feature in DataExchange::VariantValue - https://github.com/SophistSolutions/Stroika/issues/110
     *
     *      @todo   POSSIBLY add support for Precision (see Characters::Float2String) - once that module has cleaned up
     *              notion of precision. Not sure how to add unobtrusively. - for As<String>()? optional param?...
     *              Maybe Float2StringOptions is optional param to As<String> ()???
     *
     *      @todo   Add SINFAE CTOR template, so we can lose explicit map<> CTOR, and handle other
     *              cases automatically, like vector<wstring> CTOR. And/or fix KeyValuePair<> ctor so
     *              maps 'convertible' key and convertible 'value' types.
     *
     *      @todo   Use Debug::AssertExternallySynchronizedMutex<> to assure not used from multiple threads.
     *
     *      @todo   Re-review the signed/unsigned compare etc code. I think its all correct, but its tricky enough to
     *              warrant a careful review
     */
    class [[nodiscard]] VariantValue {
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
         *  \note   Common::DefaultNames<> supported
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
        using Type::eArray;
        using Type::eBLOB;
        using Type::eBoolean;
        using Type::eDate;
        using Type::eDateTime;
        using Type::eFloat;
        using Type::eInteger;
        using Type::eMap;
#if qCompilerAndStdLib_InternalCompilerErrorTSubCopy_Buggy
        static constexpr Type eNull = Type::eNull;
#else
        using Type::eNull;
#endif
        using Type::eString;
        using Type::eUnsignedInteger;

    public:
        /**
         *  \brief construct a VariantValue from most any 'basic type' you would expect to find in a weakly typed language (e.g. lisp, javascript)
         *
         *  When constructing a 'stringish' VariantValue, any arguments used to construct a String object maybe used, with the same constraints
         *  (e.g. req basic_string_view<char> MUST contain only ascii text)
         * 
         *  \note   Most constructors are not explicit to make it more easy to assign to (could do operator= for that)?
         *          A few are marked as explicit, only because its too easy to accidentally invoke 
         *          (as with http://stroika-bugs.sophists.com/browse/STK-739) - Iterable<VariantValue> or Set<VariantValue> etc...
         *          Roughly, explicit types can be used directly, and composite (aggregating) types are explicit.
         * 
         *  \todo Can shorten this a bit using templates and concepts (integral, floating_point etc - like I did for "StringIsh").
         */
        VariantValue () = default;
        VariantValue (nullopt_t);
        VariantValue (nullptr_t);
        VariantValue (bool val);
        VariantValue (const BLOB& val);
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
        template <Characters::IConvertibleToString STRINGISH_T>
        VariantValue (STRINGISH_T&& val)
            requires (not same_as<remove_cvref_t<STRINGISH_T>, String>);
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
            requires (is_convertible_v<T, VariantValue>); // @todo redo with constraint/typename syntax (I think converitble_to<VariantValue>)
#if qHasFeature_boost
        VariantValue (const boost::json::value& val);
#endif

    public:
        /**
         *  Assign anything to a VariantValue you can construct a VariantValue with.
         */
        nonvirtual VariantValue& operator= (VariantValue&& rhs) noexcept = default;
        nonvirtual VariantValue& operator= (const VariantValue& rhs)     = default;
        template <typename T>
        nonvirtual VariantValue& operator= (T&& val)
            requires (requires (T x) { VariantValue{x}; }); // @todo redo with constraint/typename syntax (I think converitble_to<VariantValue>)

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
         *  @see Characters::ToString()
         *  Return a debug-friendly, display version of the current variant. This is not guaranteed parsable or usable except for debugging.
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  Only (see requires) types supported;  There is no generic As<T> implementation.
         *
         *  If the caller attempts a conversion that isn't supported, or doesn't make sense
         *  then DataExchange::BadFormatException will be thrown (not assertion error).
         *
         *  For the optional<OF_T> specialization, if this is empty (nullopt counts as empty) - then return nullopt, and only otherwise attempt to coerce (As<the optional type>).
         *
         *  \note   Why As<T> () instead of conversion operator / static_cast support?
         *          1) - not sure - maybe a mistake
         *          2) without explicit, can cause confusion with construction conversions etc. With explicit, maybe
         *             still causes confusion with unform initialization.
         *          3) At least for the case of bool - operator bool and As<bool> can return different results (maybe also a mistake?).
         * 
         *  \note   About As<String> () and As<wstring> ()
         *          Conversion to these types is ALWAYS supported, no matter what the source type.
         *          Coercion of Date/DateTime values uses ISO8601 format.
         *          Coercion of array and map values is vaguely json-ish in format, readable, but not intended
         *          to be regular enough to be parsable.
         * 
         *  \note   Unsigned int and Signed int both converted to string representations base 10 (might have
         *          considered hex for base 16, but no).
         *
         *  \note   About As<bool> ()
         *          Coerces String value 'true' - case sensitive - to true, and any integer or unsigned integer value
         *          to true if non-zero.
         *
         *  \note   About As<BLOB> ()
         *          If type=eBLOB, return that. If type = null, return empty blob.
         *          Else, converts any type to String, and use base64 conversion.
         *          Similarly - for As<String> on types that are eBLOB - they are decoded as Base64.
         *
         *  \note   About As<Date> (), About As<DateTime> ()
         *          Null maps to empty Date.DateTime; 
         *          String value mapped to Date/DateTIme assuming ISO8601 string format  - may return empty or exception on failure.
         *
         *  \note   About As<float> (), As<double> (), As<long double> ()
         *          This converts strings and integer and floating point types. 'empty' - or the null type - is converted to nan().
         *          Similarly, if the string cannot be converted, a nan will be returned. Other types (like Mapping) generate
         *          an exception.
         * 
         *  \todo   Try to find a neater way to express the IAnyOf OPTIONAL case - shouldn't need to repeat everything. Tried using
         *          template <typename OF_T> nonvirtual optional<OF_T> As () const requires (requires (OF_T v) { VariantValue{}.As<OF_T> (); });
         *          But somehow on visual studio that produced ambiguity errors from compiler.
         */
        template <typename RETURNTYPE>
        nonvirtual RETURNTYPE As () const
            requires (Private_::IVariantValueAsBasic_<RETURNTYPE> or
                      (Common::IOptional<RETURNTYPE> and Private_::IVariantValueAsBasic_<Common::ExtractValueType_t<RETURNTYPE>>));

    public:
        /**
         *  \brief return true iff value GetType () != null;
         *
         *  \note - this is NOT the same as not empty ()!
         *  \note this is NOT the same same as As<bool> ()
         * 
         *  And because of that - at least partly - we avoided support explicit operator (for each type in the basic variant like explicit operator String).
         *  \see As<T>
         */
        nonvirtual explicit operator bool () const;

        // Consider if explicit operator T a suitable replacement for As() pattern? Just more standard c++ syntax for the same idea? Experimenting in VariantValue
        // maybe allow BOTH - cuz As<> sytnax often better/more functional a then b then c - not having to put the type conversion before thing its operating on...
        //CONSIDER FIXING issue with diff for operator bool and As<bool> () - confusing special case...
        template <typename T>
        explicit operator T () const
            requires (requires (T) { As<T> (); })
        {
            return As<T> ();
        }

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
         *  You generally don't need to use this, but its helpful for the definition of equality and comparison.
         */
        nonvirtual VariantValue Normalize () const;

    private:
        nonvirtual bool                 AsBool_ () const;
        nonvirtual Date                 AsDate_ () const;
        nonvirtual DateTime             AsDateTime_ () const;
        nonvirtual BLOB                 AsBLOB_ () const;
        nonvirtual IntegerType_         AsInteger_ () const;
        nonvirtual UnsignedIntegerType_ AsUnsignedInteger_ () const;
        nonvirtual FloatType_           AsFloatType_ () const;
        nonvirtual String               AsString_ () const;
        nonvirtual Mapping<String, VariantValue> AsMapping_ () const;
        nonvirtual Sequence<VariantValue> AsSequence_ () const;
#if qHasFeature_boost
        nonvirtual boost::json::value AsBoostJSONValue_ () const;
#endif

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
        shared_ptr<IRep_> fVal_;

    private:
        template <typename T>
        struct TIRep_;

    private:
        static const shared_ptr<IRep_> kFalseRep_; // avoid even cheap needless allocations
        static const shared_ptr<IRep_> kTrueRep_;
    };
    static_assert (totally_ordered<VariantValue>);

    /**
     *  \brief Compares values as if first normalized with Normalize () method
     * 
     *  \note Before Stroika v3.0d1, EqualsComparer had an fExactTypeMatchOnly option, which defaulted false.
     *        This did various not clearly specified type coercions - being expensive, and buggy, and confusing.
     * 
     *        The trickiest part is that to do this properly, we needed to pass along the flag (through default constructors
     *        using thread_local storage trick) - and that was never done.
     * 
     *        I know of no use-case for this functionality, its ambiguous, and costly. So we lose it.
     * 
     *        The ONLY remaining coercions that are done in comparing, are that if two numbers (int,unsigned, float) are compared
     *        they are first promoted.
     */
    struct VariantValue::EqualsComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
        constexpr EqualsComparer () = default;
        nonvirtual bool operator() (const VariantValue& lhs, const VariantValue& rhs) const;
    };

    /**
     *  \brief Compares values as if first normalized with Normalize () method
     * 
     *  \note Before Stroika v3.0d1, EqualsComparer had an fExactTypeMatchOnly option, which defaulted false.
     *        This did various not clearly specified type coercions - being expensive, and buggy, and confusing.
     * 
     *        The trickiest part is that to do this properly, we needed to pass along the flag (through default constructors
     *        using thread_local storage trick) - and that was never done.
     * 
     *        I know of no use-case for this functionality, its ambiguous, and costly. So we lose it.
     * 
     *        The ONLY remaining coercions that are done in comparing, are that if two numbers (int,unsigned, float) are compared
     *        they are first promoted.
     */
    struct VariantValue::ThreeWayComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr ThreeWayComparer () = default;
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
