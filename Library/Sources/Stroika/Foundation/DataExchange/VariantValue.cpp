/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Math/Common.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Characters::Literals;
using namespace Stroika::Foundation::DataExchange;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    // Mimic private class values - but cannot reference directly cuz private and this isn't part of the class
    using IntegerType_         = long long int;
    using UnsignedIntegerType_ = unsigned long long int;
    using FloatType_           = long double;

    // simple mappings to make use of TIRep_<> private template
    template <typename T>
    struct TN_ {};
    template <>
    struct TN_<bool> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eBoolean};
    };
    template <>
    struct TN_<Memory::BLOB> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eBLOB};
    };
    template <>
    struct TN_<IntegerType_> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eInteger};
    };
    template <>
    struct TN_<UnsignedIntegerType_> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eUnsignedInteger};
    };
    template <>
    struct TN_<FloatType_> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eFloat};
    };
    template <>
    struct TN_<Date> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eDate};
    };
    template <>
    struct TN_<DateTime> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eDateTime};
    };
    template <>
    struct TN_<String> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eString};
    };
    template <>
    struct TN_<Sequence<VariantValue>> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eArray};
    };
    template <>
    struct TN_<Mapping<String, VariantValue>> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::eMap};
    };
}

template <typename T>
struct VariantValue::TIRep_ final : VariantValue::IRep_, public Memory::UseBlockAllocationIfAppropriate<TIRep_<T>> {
    template <typename FWD>
    inline TIRep_ (FWD&& v)
        : fVal{forward<FWD> (v)}
    {
    }
    virtual Type GetType () const override
    {
        return TN_<T>::kTYPEENUM;
    }
    T fVal;
};

/*
 ********************************************************************************
 ******************************** VariantValue **********************************
 ********************************************************************************
 */
const shared_ptr<VariantValue::IRep_> VariantValue::kFalseRep_ = Memory::MakeSharedPtr<TIRep_<bool>> (false);
const shared_ptr<VariantValue::IRep_> VariantValue::kTrueRep_  = Memory::MakeSharedPtr<TIRep_<bool>> (true);

VariantValue::VariantValue (bool val)
    : fVal_{val ? kTrueRep_ : kFalseRep_}
{
    // not inline so this is guaranteed true (to inline would need to be defs for TIRep_ stuff into .inl file)
    // due to link time codegen/inlining, probably not needed
}

VariantValue::VariantValue (const Memory::BLOB& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Memory::BLOB>> (val)}
{
}

VariantValue::VariantValue (signed char val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (short int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (long int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (long long int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned char val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned short int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned long int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned long long int val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (float val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (double val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (long double val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (const Date& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Date>> (val)}
{
}

VariantValue::VariantValue (const DateTime& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<DateTime>> (val)}
{
}
VariantValue::VariantValue (const String& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<String>> (val)}
{
}
VariantValue::VariantValue (const map<wstring, VariantValue>& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Mapping<String, VariantValue>>> (Mapping<String, VariantValue>{val})}
{
}

VariantValue::VariantValue (const Mapping<String, VariantValue>& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Mapping<String, VariantValue>>> (val)}
{
}

VariantValue::VariantValue (Mapping<String, VariantValue>&& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Mapping<String, VariantValue>>> (move (val))}
{
}

VariantValue::VariantValue (Sequence<VariantValue>&& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Sequence<VariantValue>>> (move (val))}
{
}

VariantValue::VariantValue (const Sequence<VariantValue>& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Sequence<VariantValue>>> (val)}
{
}

VariantValue::VariantValue (const Traversal::Iterable<VariantValue>& val)
    : fVal_{Memory::MakeSharedPtr<TIRep_<Sequence<VariantValue>>> (Sequence<VariantValue> (val))}
{
}

#if qHasFeature_boost
namespace {
    inline auto mk_ (const boost::json::value& val) -> VariantValue
    {
        using namespace boost;
        switch (val.kind ()) {
            case json::kind::null:
                return VariantValue{};
                break;
            case json::kind::bool_:
                return val.as_bool ();
                break;
            case json::kind::double_:
                return val.as_double ();
                break;
            case json::kind::int64:
                return val.as_int64 ();
                break;
            case json::kind::uint64:
                return val.as_uint64 ();
                break;
            case json::kind::string: {
                const json::string& bs = val.as_string (); // boost::json::string documents it represents a string as a series of UTF-8 characters
                return String::FromUTF8 (span{bs});
            } break;
            case json::kind::array: {
                const auto& a = val.as_array ();
                std::vector<VariantValue> r; // performance tweak, add in STL, avoiding virtual calls for each add, and then move to Stroika Seqeunce
                r.reserve (a.size ());
                for (const boost::json::value& i : a) {
                    r.emplace_back (mk_ (i));
                }
                return VariantValue{Containers::Concrete::Sequence_stdvector<VariantValue>{std::move (r)}};
            } break;
            case json::kind::object: {
                const auto& o = val.as_object ();
                Containers::Concrete::Mapping_stdhashmap<String, VariantValue>::STDHASHMAP<> r; // performance tweak, add in STL, avoiding virtual calls for each add, and then move to Stroika mapping
                r.reserve (o.size ());
                for (const auto& i : o) {
                    r.insert ({String::FromUTF8 (span{i.key ()}), mk_ (i.value ())});
                }
                return VariantValue{Containers::Concrete::Mapping_stdhashmap<String, VariantValue>{std::move (r)}};
            } break;
            default:
                AssertNotReached ();
                return VariantValue{};
        }
    }
}
VariantValue::VariantValue (const boost::json::value& val)
    : VariantValue{mk_ (val)}
{
}
#endif

bool VariantValue::empty () const
{
    if (fVal_ == nullptr) {
        return true;
    }
    switch (fVal_->GetType ()) {
        case Type::eBoolean:
        case Type::eInteger:
        case Type::eUnsignedInteger: {
            return false; // cannot be empty
        }
        case Type::eBLOB: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            return v->fVal.empty ();
        }
        case Type::eFloat: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return isnan (v->fVal);
        }
        case Type::eDate: {
            return false; // cannot be empty (a change since Stroika v2.1d11 - used to be v->fVal.empty ())
        }
        case Type::eDateTime: {
            return false; // cannot be empty (a change since Stroika v2.1d11 - used to be v->fVal.empty ())
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        case Type::eMap: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        case Type::eArray: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        default: {
            return As<String> ().empty ();
        }
    }
}

String VariantValue::ToString () const
{
    return As<String> ();
}

bool VariantValue::AsBool_ () const
{
    if (fVal_ == nullptr) {
        return false;
    }
    switch (fVal_->GetType ()) {
        case Type::eBoolean: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<bool>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            //return tmp != "false";          // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensitive
            return As<String> () == "true"sv; // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensitive
        }
        case Type::eInteger: {
            return As<IntegerType_> () != 0;
        }
        case Type::eUnsignedInteger: {
            return As<UnsignedIntegerType_> () != 0;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed coerce-to-bool: type={}, value={}"_f, fVal_->GetType (), *this);
#endif
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue to bool"sv});
        }
    }
}

bool VariantValue::IsConvertibleTo (Type to) const
{
    if (GetType () == to) [[likely]] {
        return true; // performance tweak
    }
    try {
        Debug::TraceContextSuppressor suppressTraceInThisBlock;
        (void)ConvertTo (to);
        return true;
    }
    catch (...) {
        return false;
    }
}

VariantValue VariantValue::ConvertTo (Type to) const
{
    if (GetType () == to) [[likely]] {
        return *this; // performance tweak
    }
    static const DataExchange::BadFormatException kCannotCoerce2Null_{"Cannot coerce VariantValue to null"sv};
    switch (to) {
        case Type::eNull:
            // Only null (caught above) can translate to null...
            Execution::Throw (kCannotCoerce2Null_);
        case Type::eBLOB:
            return As<Memory::BLOB> ();
        case Type::eBoolean:
            return As<bool> ();
        case Type::eInteger:
            return As<int> ();
        case Type::eUnsignedInteger:
            return As<unsigned int> ();
        case Type::eFloat:
            return As<FloatType_> ();
        case Type::eDate:
            return As<Time::Date> ();
        case Type::eDateTime:
            return As<Time::DateTime> ();
        case Type::eString:
            return As<String> ();
        case Type::eArray:
            return VariantValue{As<Sequence<VariantValue>> ()};
        case Type::eMap:
            return VariantValue{As<Mapping<String, VariantValue>> ()};
    }
    static const DataExchange::BadFormatException kCannotCoerce2ThatType_{"Cannot coerce VariantValue to that type"sv};
    Execution::Throw (kCannotCoerce2ThatType_);
}

VariantValue VariantValue::Normalize () const
{
    using KVPT = Common::KeyValuePair<String, VariantValue>;
    switch (GetType ()) {
        case Type::eNull:
            return *this;
        case Type::eBLOB:
            return ConvertTo (Type::eString);
        case Type::eBoolean:
            return *this;
        case Type::eInteger:
            return ConvertTo (Type::eFloat);
        case Type::eUnsignedInteger:
            return ConvertTo (Type::eFloat);
        case Type::eFloat: {
            // tricky case - nans and infs must be converted to strings, because they cannot be emitted in JSON as 'numbers'
            // and so when are read back, they are read back as strings.
            FloatType_ f = As<FloatType_> ();
            if (std::isnan (f) or std::isinf (f)) {
                return Characters::FloatConversion::ToString (f);
            }
            return *this;
        }
        case Type::eDate:
            return ConvertTo (Type::eString);
        case Type::eDateTime:
            return ConvertTo (Type::eString);
        case Type::eString:
            return *this;
        case Type::eArray:
            // must recursively normalize all sub-elements
            return VariantValue{
                As<Sequence<VariantValue>> ().Map<Sequence<VariantValue>> ([] (const VariantValue& v) { return v.Normalize (); })};
        case Type::eMap:
            // must recursively normalize all sub-elements, but also produce a sorted-map
            return VariantValue{As<Mapping<String, VariantValue>> ().Map<Containers::SortedMapping<String, VariantValue>> ([] (const KVPT& kvp) {
                return KVPT{kvp.fKey, kvp.fValue.Normalize ()};
            })};
        default:
            AssertNotReached ();
            return nullptr;
    }
}

Memory::BLOB VariantValue::AsBLOB_ () const
{
    if (fVal_ == nullptr) {
        return Memory::BLOB{};
    }
    switch (fVal_->GetType ()) {
        case Type::eBLOB: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
            return Cryptography::Encoding::Algorithm::Base64::Decode (As<String> ());
        }
    }
}

VariantValue::IntegerType_ VariantValue::AsInteger_ () const
{
    if (fVal_ == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case Type::eBoolean: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<bool>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eFloat: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return Math::Round<VariantValue::IntegerType_> (v->fVal);
        }
        case Type::eInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eUnsignedInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return Characters::String2Int<IntegerType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed coerce-to-int: type={}, value={}"_f, fVal_->GetType (), *this);
#endif
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue to integer"sv});
        }
    }
}

VariantValue::UnsignedIntegerType_ VariantValue::AsUnsignedInteger_ () const
{
    if (fVal_ == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case Type::eFloat: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return Math::Round<VariantValue::UnsignedIntegerType_> (v->fVal);
        }
        case Type::eInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eUnsignedInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            // not sure this is right for high order bit set for unsigned long long?
            // --LGP 2013-08-25
            return Characters::String2Int<UnsignedIntegerType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed coerce-to-uint: type={}, value={}"_f, fVal_->GetType (), *this);
#endif
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue to unsigned integer"sv});
        }
    }
}

VariantValue::FloatType_ VariantValue::AsFloatType_ () const
{
    if (fVal_ == nullptr) {
        return Math::nan<FloatType_> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return static_cast<FloatType_> (v->fVal);
        }
        case Type::eUnsignedInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return static_cast<FloatType_> (v->fVal);
        }
        case Type::eFloat: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            // NB. this may return NAN if string not a well-formed number (including empty string case)
            return Characters::FloatConversion::ToFloat<FloatType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed coerce-to-float: type={}, value={}"_f, fVal_->GetType (), *this);
#endif
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue to float"sv});
        }
    }
}

Date VariantValue::AsDate_ () const
{
    if (fVal_ == nullptr) {
        Execution::Throw (Date::FormatException::kThe); // until Stroika v2.1d11 this returned Date{}, but no nonger support empty Date objects
    }
    switch (fVal_->GetType ()) {
        case Type::eDate: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eDateTime: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.GetDate ();
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return Date::Parse (v->fVal, Date::kISO8601Format);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed coerce-to-date: type={}, value={}"_f, fVal_->GetType (), *this);
#endif
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue to date"sv});
        }
    }
}

DateTime VariantValue::AsDateTime_ () const
{
    if (fVal_ == nullptr) {
        Execution::Throw (DateTime::FormatException::kThe); // until Stroika v2.1d11 this returned DateTime{}, but no longer support empty DateTime objects
    }
    switch (fVal_->GetType ()) {
        case Type::eDate: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return DateTime{v->fVal};
        }
        case Type::eDateTime: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return DateTime::Parse (v->fVal, DateTime::kISO8601Format);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed coerce-to-datetime: type={}, value={}"_f, fVal_->GetType (), *this);
#endif
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue to date-time"sv});
        }
    }
}

#if qHasFeature_boost
boost::json::value VariantValue::AsBoostJSONValue_ () const
{
    using namespace boost;
    if (fVal_ == nullptr) {
        return json::value{nullptr};
    }
    switch (fVal_->GetType ()) {
        case Type::eNull: {
            return json::value{nullptr};
        }
        case Type::eBoolean: {
            return As<bool> ();
        }
        case Type::eString: {
            // I think boost uses / expects UTF8?
            return As<String> ().AsUTF8<string> ().c_str ();
        }
        case Type::eInteger: {
            return As<IntegerType_> ();
        }
        case Type::eUnsignedInteger: {
            return As<UnsignedIntegerType_> ();
        }
        case Type::eFloat: {
            return static_cast<double> (As<FloatType_> ());
        }
        case Type::eBLOB:
        case Type::eDate:
        case Type::eDateTime: {
            // not a boost::json type, so convert to string
            return As<String> ().AsASCII ().c_str ();
        }
        case Type::eArray: {
            using Containers::Sequence;
            Sequence<VariantValue> srcArray = As<Sequence<VariantValue>> ();
            json::array            result;
            result.reserve (srcArray.size ());
            for (const auto& i : srcArray) {
                result.push_back (i.As<json::value> ());
            }
            return result;
        }
        case Type::eMap: {
            using Containers::Mapping;
            Mapping<String, VariantValue> srcMap = As<Mapping<String, VariantValue>> ();
            json::object                  result;
            for (const auto& i : srcMap) {
                result.insert (json::key_value_pair{i.fKey.As<String> ().AsUTF8<string> ().c_str (), i.fValue.As<json::value> ()});
            }
            return result;
        }
        default:
            AssertNotReached ();
            return json::value{nullptr};
    }
}
#endif

String VariantValue::AsString_ () const
{
    if (fVal_ == nullptr) [[unlikely]] {
        return String{};
    }
    switch (fVal_->GetType ()) {
        case Type::eNull: {
            AssertNotReached ();
            return String{};
        }
        case Type::eBoolean: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<bool>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal ? "true"sv : "false"sv;
        }
        case Type::eBLOB: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            AssertNotNull (v);
            return String{Cryptography::Encoding::Algorithm::Base64::Encode (v->fVal)};
        }
        case Type::eInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return Characters::Format ("{}"_f, v->fVal);
        }
        case Type::eUnsignedInteger: {
            // Note - unsigned numbers converted to DECIMAL still as text representation
            auto v = Debug::UncheckedDynamicCast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return Characters::Format ("{}"_f, v->fVal);
        }
        case Type::eFloat: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            using namespace Characters;
            /*
             *  numeric_limits<FloatType_>::digits gives the number of digits of the mantissa in radix units, but since we
             *  write the numbers as decimal, this doesn't come to an integral number of decimal digits.
             *
             *  So we either write too many digits of precision, or too few.
             *
             *  VERY VERY unsure what is best. From 2013-11-17 until 2015-08-27 I wrote digits10 + 2.
             *
             *  As of 2015-08-27 - I'm switching to digits10 + 1 - so effectively just rounding up instead of down.
             *  This means we will always write all the precision we have and then some ;-).
             *
             *  For a bit, I'll leave behind the older comment, which I think is not 100% right, but we can lose it soon.
             // given a number of digits of precision, its fractional (and rounded down). digits10 + 1 gives you the number
             // of actual digits after the decimal point. But there is one before the decimal point to give the precision we
             // use in iostream.
             //
             // Actually - I'm really not sure of any of this. But this seems to work for now...
             //      -- LGP 2013-11-17
             */
            //
            //
            //????              static  const   Float2StringOptions kFmtOptions_ { FloatConversion::Precision{numeric_limits<FloatType_>::digits10 + 1} };

            // From https://en.cppreference.com/w/cpp/types/numeric_limits/digits10
            //  The value of std::numeric_limits<T>::digits10 is the number of base-10 digits that can be represented by the type T without change,
            //  that is, any number with this many significant decimal digits can be converted to a value of type T and back to decimal form,
            //  without change due to rounding or overflow. For base-radix types, it is the value of digits() (digits - 1 for floating-point types)
            //  multiplied by log 10 radix and rounded down.

            static const FloatConversion::ToStringOptions kFmtOptions_{FloatConversion::Precision{numeric_limits<FloatType_>::digits10 - 1}};
            return FloatConversion::ToString (v->fVal, kFmtOptions_);
        }
        case Type::eDate: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.Format (Date::kISO8601Format);
        }
        case Type::eDateTime: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.Format (DateTime::kISO8601Format);
        }
        case Type::eString: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eArray: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            Characters::StringBuilder tmp;
            tmp << "["sv;
            for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                if (i != v->fVal.begin ()) {
                    tmp << ", "sv;
                }
                tmp << i->As<String> ();
            }
            tmp << "]"sv;
            return tmp;
        }
        case Type::eMap: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            Characters::StringBuilder tmp;
            tmp << "{"sv;
            for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                if (i != v->fVal.begin ()) {
                    tmp << ", ";
                }
                tmp << i->fKey << " -> "sv << i->fValue.As<String> ();
            }
            tmp << "}"sv;
            return tmp;
        }
        default: {
            AssertNotReached (); // That was all types enumerated, and all types convertable to string
            return String{};
        }
    }
}

Mapping<String, VariantValue> VariantValue::AsMapping_ () const
{
    if (fVal_ == nullptr) [[unlikely]] {
        return Mapping<String, VariantValue>{};
    }
    switch (fVal_->GetType ()) {
        case Type::eMap: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue of type {} to map"_f(fVal_->GetType ())});
        }
    }
}

Sequence<VariantValue> VariantValue::AsSequence_ () const
{
    if (fVal_ == nullptr) [[unlikely]] {
        return Sequence<VariantValue>{};
    }
    switch (fVal_->GetType ()) {
        case Type::eArray: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
            Execution::Throw (DataExchange::BadFormatException{"Cannot coerce VariantValue of type {} to array"_f(fVal_->GetType ())});
        }
    }
}

/*
 ********************************************************************************
 ***************** DataExchange::VariantValue::EqualsComparer *******************
 ********************************************************************************
 */
bool Stroika::Foundation::DataExchange::VariantValue::EqualsComparer::operator() (const VariantValue& lhs, const VariantValue& rhs) const
{
    VariantValue       ln = lhs.Normalize ();
    VariantValue       rn = rhs.Normalize ();
    VariantValue::Type lt = ln.GetType ();
    VariantValue::Type rt = rn.GetType ();
    if (lt != rt) {
        return false;
    }
    switch (lt) {
        case VariantValue::eNull:
            return true;
        case VariantValue::eBoolean:
            return ln.As<bool> () == ln.As<bool> ();
        case VariantValue::eFloat:
            return Math::NearlyEquals (ln.As<FloatType_> (), ln.As<FloatType_> ());
        case VariantValue::eString:
            return ln.As<String> () == ln.As<String> ();
        case VariantValue::eArray: {
            // same iff all elts same (after normalizing sub-elts above)
            return ln.As<Sequence<VariantValue>> () == rn.As<Sequence<VariantValue>> ();
        }
        case VariantValue::eMap: {
            // same iff all elts same (importantly after normalizing which sorts)
            return ln.As<Mapping<String, VariantValue>> () == rn.As<Mapping<String, VariantValue>> ();
        }
        case VariantValue::eBLOB:
        case VariantValue::eInteger:
        case VariantValue::eUnsignedInteger:
        case VariantValue::eDate:
        case VariantValue::eDateTime:
            AssertNotReached (); // cuz normalized
        default:
            AssertNotReached ();
            return false;
    }
}

/*
 ********************************************************************************
 ************************* VariantValue::ThreeWayComparer ***********************
 ********************************************************************************
 */
strong_ordering VariantValue::ThreeWayComparer::operator() (const VariantValue& lhs, const VariantValue& rhs) const
{
    VariantValue       ln = lhs.Normalize ();
    VariantValue       rn = rhs.Normalize ();
    VariantValue::Type lt = ln.GetType ();
    VariantValue::Type rt = rn.GetType ();
    if (lt != rt) {
        return lt <=> rt; // no obvious sort order, so just use numeric type value
    }
    switch (lt) {
        case VariantValue::eNull:
            return strong_ordering::equal;
        case VariantValue::eBoolean:
            return ln.As<bool> () <=> ln.As<bool> ();
        case VariantValue::eFloat: {
            // explicit test so we can do NearlyEquals()
            FloatType_ l = ln.As<FloatType_> ();
            FloatType_ r = rn.As<FloatType_> ();
            if (Math::NearlyEquals (l, r)) {
                return strong_ordering::equal;
            }
            else if (l < r) {
                return strong_ordering::less;
            }
            else {
                return strong_ordering::greater;
            }
        }
        case VariantValue::eString:
            return ln.As<String> () <=> ln.As<String> ();
        case VariantValue::eArray: {
            // same iff all elts same (after normalizing sub-elts above)
            return ln.As<Sequence<VariantValue>> () <=> rn.As<Sequence<VariantValue>> ();
        }
        case VariantValue::eMap: {
            //https://stroika.atlassian.net/browse/STK-985
            //
            // same iff all elts same (importantly after normalizing which sorts)
            // @todo find way to make this compare work, but for now, just hack and re-create sorted mapping
            // Maybe add 'virtual' rep method on Mapping - AsSorted - and then say
            // As<SortedMapping<...>> on the mapping that comes back from VariantValue.
            // Or add that same 'feature' just in VariantValue - with flag when creating with SortedMapping (maybe additional 'type')
            // and logic privately in here so you dont need to worry outside.
            if (false) {
                //return ln.As<Mapping<String, VariantValue>> () <=> rn.As<Mapping<String, VariantValue>> ();
            }
            else {
                using SMT = Containers::SortedMapping<String, VariantValue>;
                return SMT{ln.As<Mapping<String, VariantValue>> ()} <=> SMT{rn.As<Mapping<String, VariantValue>> ()};
            }
        }
        case VariantValue::eBLOB:
        case VariantValue::eInteger:
        case VariantValue::eUnsignedInteger:
        case VariantValue::eDate:
        case VariantValue::eDateTime:
            AssertNotReached (); // cuz normalized
        default:
            AssertNotReached ();
            return strong_ordering::equal;
    }
}