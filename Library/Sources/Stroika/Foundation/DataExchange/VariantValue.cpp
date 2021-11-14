/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <sstream>

#include "../Characters/FloatConversion.h"
#include "../Characters/Format.h"
#include "../Characters/String2Int.h"
#include "../Characters/ToString.h"
#include "../Cryptography/Encoding/Algorithm/Base64.h"
#include "../DataExchange/BadFormatException.h"
#include "../Debug/Cast.h"
#include "../Math/Common.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    // Mimic private class values - but cannot reference directly cuz private and this isn't part of the class
    using IntegerType_         = long long int;
    using UnsignedIntegerType_ = unsigned long long int;
    using FloatType_           = long double;

    // simple mappings to make use of TIRep_<> private template
    template <typename T>
    struct TN_ {
    };
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
struct VariantValue::TIRep_ : VariantValue::IRep_, public Memory::UseBlockAllocationIfAppropriate<TIRep_<T>> {
    template <typename FWD>
    inline TIRep_ (FWD&& v)
        // @todo - Understand why changing this to uniform-initialization changes this to an infinite stack overflow recursion
        : fVal (forward<FWD> (v))
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
VariantValue::VariantValue (bool val)
    : fVal_{MakeSharedPtr_<TIRep_<bool>> (val)}
{
}

VariantValue::VariantValue (const Memory::BLOB& val)
    : fVal_{MakeSharedPtr_<TIRep_<Memory::BLOB>> (val)}
{
}

VariantValue::VariantValue (signed char val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (short int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (long int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (long long int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned char val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned short int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned long int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned long long int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (float val)
    : fVal_{MakeSharedPtr_<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (double val)
    : fVal_{MakeSharedPtr_<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (long double val)
    : fVal_{MakeSharedPtr_<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (const Date& val)
    : fVal_{MakeSharedPtr_<TIRep_<Date>> (val)}
{
}

VariantValue::VariantValue (const DateTime& val)
    : fVal_{MakeSharedPtr_<TIRep_<DateTime>> (val)}
{
}

VariantValue::VariantValue (const wchar_t* val)
    : fVal_{MakeSharedPtr_<TIRep_<String>> (val)}
{
}

VariantValue::VariantValue (const wstring& val)
    : fVal_{MakeSharedPtr_<TIRep_<String>> (val)}
{
}

VariantValue::VariantValue (const String& val)
    : fVal_{MakeSharedPtr_<TIRep_<String>> (val)}
{
}

VariantValue::VariantValue (const map<wstring, VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Mapping<String, VariantValue>>> (Mapping<String, VariantValue>{val})}
{
}

VariantValue::VariantValue (const Mapping<String, VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Mapping<String, VariantValue>>> (val)}
{
}

VariantValue::VariantValue (Mapping<String, VariantValue>&& val)
    : fVal_{MakeSharedPtr_<TIRep_<Mapping<String, VariantValue>>> (move (val))}
{
}

VariantValue::VariantValue (const Sequence<VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Sequence<VariantValue>>> (val)}
{
}

VariantValue::VariantValue (const Traversal::Iterable<VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Sequence<VariantValue>>> (Sequence<VariantValue> (val))}
{
}

VariantValue::VariantValue (Sequence<VariantValue>&& val)
    : fVal_{MakeSharedPtr_<TIRep_<Sequence<VariantValue>>> (move (val))}
{
}

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

template <>
bool VariantValue::As () const
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
            //return tmp != L"false";           // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
            return As<String> () == L"true"sv; // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
        }
        case Type::eInteger: {
            return As<IntegerType_> () != 0;
        }
        case Type::eUnsignedInteger: {
            return As<UnsignedIntegerType_> () != 0;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-bool: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to bool"sv});
        }
    }
}

bool VariantValue::IsConvertibleTo (Type to) const
{
    if (GetType () == to) [[LIKELY_ATTR]] {
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
    if (GetType () == to) [[LIKELY_ATTR]] {
        return *this; // performance tweak
    }
    static const DataExchange::BadFormatException kCannotCoerce2Null_{L"Cannot coerce VariantValue to null"sv};
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
            return As<float> ();
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
    static const DataExchange::BadFormatException kCannotCoerce2ThatType_{L"Cannot coerce VariantValue to that type"sv};
    Execution::Throw (kCannotCoerce2ThatType_);
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
            return Cryptography::Encoding::Algorithm::DecodeBase64 (As<String> ());
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
            DbgTrace (L"failed coerce-to-int: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to integer"sv});
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
            DbgTrace (L"failed coerce-to-uint: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to unsigned integer"sv});
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
            return Characters::String2Float<FloatType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-float: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to float"sv});
        }
    }
}

template <>
Date VariantValue::As () const
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
            DbgTrace (L"failed coerce-to-date: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to date"sv});
        }
    }
}

template <>
DateTime VariantValue::As () const
{
    if (fVal_ == nullptr) {
        Execution::Throw (DateTime::FormatException::kThe); // until Stroika v2.1d11 this returned DateTime{}, but no nonger support empty DateTime objects
    }
    switch (fVal_->GetType ()) {
        case Type::eDate: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return DateTime (v->fVal);
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
            DbgTrace (L"failed coerce-to-datetime: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to date-time"sv});
        }
    }
}

String VariantValue::AsString_ () const
{
    if (fVal_ == nullptr) [[UNLIKELY_ATTR]] {
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
            return v->fVal ? L"true"sv : L"false"sv;
        }
        case Type::eBLOB: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            AssertNotNull (v);
            return String::FromASCII (Cryptography::Encoding::Algorithm::EncodeBase64 (v->fVal));
        }
        case Type::eInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            Assert (typeid (v->fVal) == typeid (long long));
            return Characters::Format (L"%lld", v->fVal);
        }
        case Type::eUnsignedInteger: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            Assert (typeid (v->fVal) == typeid (unsigned long long));
            return Characters::Format (L"%llu", v->fVal);
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
             // Acutally - I'm really not sure of any of this. But this seems to work for now...
             //      -- LGP 2013-11-17
             */
            //
            //
            //????              static  const   Float2StringOptions kFmtOptions_ { Float2StringOptions::Precision{numeric_limits<FloatType_>::digits10 + 1} };
            static const FloatConversion::ToStringOptions kFmtOptions_{FloatConversion::ToStringOptions::Precision{numeric_limits<FloatType_>::digits10 + 2}};
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
            wstringstream tmp;
            tmp << L"[";
            for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                if (i != v->fVal.begin ()) {
                    tmp << L", ";
                }
                tmp << i->As<wstring> ();
            }
            tmp << L"]";
            return tmp.str ();
        }
        case Type::eMap: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            wstringstream tmp;
            tmp << L"{";
            for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                if (i != v->fVal.begin ()) {
                    tmp << L", ";
                }
                tmp << i->fKey.As<wstring> () << L" -> " << i->fValue.As<wstring> ();
            }
            tmp << L"}";
            return tmp.str ();
        }
        default: {
            AssertNotReached (); // That was all types enumerated, and all types convertable to string
            return String{};
        }
    }
}

template <>
map<wstring, VariantValue> VariantValue::As () const
{
    using namespace Characters;
    if (fVal_ == nullptr) [[UNLIKELY_ATTR]] {
        return map<wstring, VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eMap: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            map<wstring, VariantValue> tmp;
            for (auto i : v->fVal) {
                tmp.insert (map<wstring, VariantValue>::value_type (i.fKey.As<wstring> (), i.fValue));
            }
            return tmp;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-map<>: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to map"sv});
        }
    }
}

template <>
Mapping<String, VariantValue> VariantValue::As () const
{
    using namespace Characters;
    if (fVal_ == nullptr) [[UNLIKELY_ATTR]] {
        return Mapping<String, VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eMap: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-Mapping<>: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to map"sv});
        }
    }
}

template <>
vector<VariantValue> VariantValue::As () const
{
    return As<Sequence<VariantValue>> ().As<vector<VariantValue>> ();
}

template <>
Sequence<VariantValue> VariantValue::As () const
{
    if (fVal_ == nullptr) [[UNLIKELY_ATTR]] {
        return Sequence<VariantValue>{};
    }
    switch (fVal_->GetType ()) {
        case Type::eArray: {
            auto v = Debug::UncheckedDynamicCast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-Sequence<>: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException{L"Cannot coerce VariantValue to array"sv});
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
    VariantValue::Type lt = lhs.GetType ();
    VariantValue::Type rt = rhs.GetType ();
    if (lt != rt) {
        if (fExactTypeMatchOnly) {
            return false;
        }
        else if (
            (lt == VariantValue::eInteger or lt == VariantValue::eUnsignedInteger) and
            (rt == VariantValue::eInteger or rt == VariantValue::eUnsignedInteger)) {
            // Set compare as type and fall through
            lt = VariantValue::eUnsignedInteger;
            rt = VariantValue::eUnsignedInteger;
        }
        else if (
            (lt == VariantValue::eFloat and (rt == VariantValue::eString or rt == VariantValue::eInteger or rt == VariantValue::eUnsignedInteger)) or
            (rt == VariantValue::eFloat and (lt == VariantValue::eString or lt == VariantValue::eInteger or lt == VariantValue::eUnsignedInteger))) {
            // Set compare as type and fall through
            lt = VariantValue::eFloat;
            rt = VariantValue::eFloat;
        }
        else if (
            ((lt != VariantValue::eArray and lt != VariantValue::eMap) and rt == VariantValue::eString) or
            ((rt != VariantValue::eArray and rt != VariantValue::eMap) and lt == VariantValue::eString)) {
            // special case - comparing a string with any type except map or array, trat as strings
            // Set compare as type and fall through
            lt = VariantValue::eString;
            rt = VariantValue::eString;
        }
        else {
            return false;
        }
        // can fall through for some fallthrough cases above...
    }
    switch (lt) {
        case VariantValue::eNull:
            return true;
        case VariantValue::eBLOB:
            return lhs.As<Memory::BLOB> () == rhs.As<Memory::BLOB> ();
        case VariantValue::eBoolean:
            return lhs.As<bool> () == rhs.As<bool> ();
        case VariantValue::eInteger:
            return lhs.As<IntegerType_> () == rhs.As<IntegerType_> ();
        case VariantValue::eUnsignedInteger:
            return lhs.As<UnsignedIntegerType_> () == rhs.As<UnsignedIntegerType_> ();
        case VariantValue::eFloat:
            return Math::NearlyEquals (lhs.As<FloatType_> (), rhs.As<FloatType_> ());
        case VariantValue::eDate:
            return lhs.As<Date> () == rhs.As<Date> ();
        case VariantValue::eDateTime:
            return lhs.As<DateTime> () == rhs.As<DateTime> ();
        case VariantValue::eString:
            return lhs.As<String> () == rhs.As<String> ();
        case VariantValue::eArray: {
            // same iff all elts same
            Sequence<VariantValue> lhsV = lhs.As<Sequence<VariantValue>> ();
            Sequence<VariantValue> rhsV = rhs.As<Sequence<VariantValue>> ();
            if (lhsV.size () != rhsV.size ()) {
                return false;
            }
            for (size_t i = 0; i < lhsV.size (); ++i) {
                if (lhsV[i] != rhsV[i]) {
                    return false;
                }
            }
            return true;
        }
        case VariantValue::eMap: {
            // same iff all elts same
            Mapping<String, VariantValue> lhsM = lhs.As<Mapping<String, VariantValue>> ();
            Mapping<String, VariantValue> rhsM = rhs.As<Mapping<String, VariantValue>> ();
            if (lhsM.size () != rhsM.size ()) {
                return false;
            }
            auto li = lhsM.begin ();
            auto ri = rhsM.begin ();
            for (; li != lhsM.end (); ++li, ++ri) {
                if (*li != *ri) {
                    return false;
                }
            }
            Ensure (li == lhsM.end ());
            Ensure (ri == rhsM.end ());
            return true;
        }
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
Common::strong_ordering VariantValue::ThreeWayComparer::operator() (const VariantValue& lhs, const VariantValue& rhs) const
{
    VariantValue::Type lt = lhs.GetType ();
    VariantValue::Type rt = rhs.GetType ();
    if (fExactTypeMatchOnly) {
        // No obvious way to compare elements of different type, so just use the ordering of the types (new in Stroika v2.1d5, and ONLY if new (not default) fExactTypeMatchOnly flag set
        if (lt != rt) {
            return Common::ThreeWayCompare (lt, rt);
        }
    }
    switch (lt) {
        case VariantValue::eNull:
            return rt == VariantValue::eNull ? Common::kEqual : Common::kGreater;
        case VariantValue::eBoolean:
            return Common::ThreeWayCompare (lhs.As<bool> (), rhs.As<bool> ());
        case VariantValue::eInteger:
            return Common::ThreeWayCompare (lhs.As<IntegerType_> (), rhs.As<IntegerType_> ());
        case VariantValue::eUnsignedInteger:
            return Common::ThreeWayCompare (lhs.As<UnsignedIntegerType_> (), rhs.As<UnsignedIntegerType_> ());
        case VariantValue::eFloat: {
            // explicit test so we can do NearlyEquals()
            FloatType_ l = lhs.As<FloatType_> ();
            FloatType_ r = rhs.As<FloatType_> ();
            if (Math::NearlyEquals (l, r)) {
                return Common::kEqual;
            }
            else if (l < r) {
                return Common::kLess;
            }
            else {
                return Common::kGreater;
            }
        }
        case VariantValue::eDate:
            return Common::ThreeWayCompare (lhs.As<Date> (), rhs.As<Date> ());
        case VariantValue::eDateTime:
            return Common::ThreeWayCompare (lhs.As<DateTime> (), rhs.As<DateTime> ());
        case VariantValue::eString:
            return Common::ThreeWayCompare (lhs.As<String> (), rhs.As<String> ());
        case VariantValue::eArray:
            return Common::ThreeWayCompare (lhs.As<Sequence<VariantValue>> (), rhs.As<Sequence<VariantValue>> ());
        case VariantValue::eMap: {
// Cannot do cuz Keys() NYI
// @todo - fix!!!
#if 0
                return Common::ThreeWayCompare (As<Mapping<String, VariantValue>> ().Keys (), rhs.As<Mapping<String, VariantValue>>.Keys () ());
#endif
            // same iff all elts same
            Mapping<String, VariantValue> lhsM{lhs.As<Mapping<String, VariantValue>> ()};
            Mapping<String, VariantValue> rhsM{rhs.As<Mapping<String, VariantValue>> ()};
            auto                          li = lhsM.begin ();
            auto                          ri = rhsM.begin ();
            for (; li != lhsM.end (); ++li, ++ri) {
                if (ri == rhsM.end ()) {
                    return Common::kLess;
                }
                if (*li != *ri) {
                    //return false; CHANGE IN BEHAVIOR (I THINK FIX) 2020-05-04
                    return Common::ThreeWayCompare (*li, *ri);
                }
            }
            Ensure (li == lhsM.end ());
            if (ri == rhsM.end ()) {
                return Common::kEqual;
            }
            else {
                return Common::kGreater;
            }
        }
        default:
            AssertNotReached ();
            return Common::kEqual;
    }
}