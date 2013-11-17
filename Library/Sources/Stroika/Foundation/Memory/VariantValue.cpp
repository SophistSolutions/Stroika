/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <sstream>

#include    "../Characters/Format.h"
#include    "../DataExchangeFormat/BadFormatException.h"
#include    "../Math/Common.h"

#include    "VariantValue.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;


namespace {
    // Mimic private class values - but cannot reference directly cuz private and this isnt part of class
    typedef long long int           IntegerType_;
    typedef unsigned long long int  UnsignedIntegerType_;
    typedef long double             FloatType_;

    // simple mappings to make use of TIRep_<> private template
    template    <typename T>
    struct TN_ {
    };
    template    <>
    struct TN_<bool> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eBoolean;
    };
    template    <>
    struct TN_<IntegerType_> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eInteger;
    };
    template    <>
    struct TN_<UnsignedIntegerType_> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eUnsignedInteger;
    };
    template    <>
    struct TN_<FloatType_> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eFloat;
    };
    template    <>
    struct TN_<Date> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eDate;
    };
    template    <>
    struct TN_<DateTime> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eDateTime;
    };
    template    <>
    struct TN_<String> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eString;
    };
    template    <>
    struct TN_<vector<VariantValue>> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eArray;
    };
    template    <>
    struct TN_<map<wstring, VariantValue>> {
        static  constexpr VariantValue::Type    kTYPEENUM   =   VariantValue::Type::eMap;
    };
}


template    <typename T>
struct  VariantValue::TIRep_ : VariantValue::IRep_ {
    TIRep_ (T v)
        : fVal (v) {
    }
    virtual Type    GetType () const override {
        return TN_<T>::kTYPEENUM;
    }
    T     fVal;
    DECLARE_USE_BLOCK_ALLOCATION(TIRep_);
};







/*
 ********************************************************************************
 ******************************** VariantValue **********************************
 ********************************************************************************
 */
VariantValue::VariantValue ()
    : fVal_ ()
{
}

VariantValue::VariantValue (bool val)
    : fVal_ (DEBUG_NEW TIRep_<bool> (val))
{
}

VariantValue::VariantValue (signed char val)
    : fVal_ (DEBUG_NEW TIRep_<IntegerType_> (val))
{
}

VariantValue::VariantValue (short int val)
    : fVal_ (DEBUG_NEW TIRep_<IntegerType_> (val))
{
}

VariantValue::VariantValue (int val)
    : fVal_ (DEBUG_NEW TIRep_<IntegerType_> (val))
{
}

VariantValue::VariantValue (long int val)
    : fVal_ (DEBUG_NEW TIRep_<IntegerType_> (val))
{
}

VariantValue::VariantValue (long long int val)
    : fVal_ (DEBUG_NEW TIRep_<IntegerType_> (val))
{
}

VariantValue::VariantValue (unsigned char val)
    : fVal_ (DEBUG_NEW TIRep_<UnsignedIntegerType_> (val))
{
}

VariantValue::VariantValue (unsigned short int val)
    : fVal_ (DEBUG_NEW TIRep_<UnsignedIntegerType_> (val))
{
}

VariantValue::VariantValue (unsigned int val)
    : fVal_ (DEBUG_NEW TIRep_<UnsignedIntegerType_> (val))
{
}

VariantValue::VariantValue (unsigned long int val)
    : fVal_ (DEBUG_NEW TIRep_<UnsignedIntegerType_> (val))
{
}

VariantValue::VariantValue (unsigned long long int val)
    : fVal_ (DEBUG_NEW TIRep_<UnsignedIntegerType_> (val))
{
}

VariantValue::VariantValue (float val)
    : fVal_ (DEBUG_NEW TIRep_<FloatType_> (val))
{
}

VariantValue::VariantValue (double val)
    : fVal_ (DEBUG_NEW TIRep_<FloatType_> (val))
{
}

VariantValue::VariantValue (long double val)
    : fVal_ (DEBUG_NEW TIRep_<FloatType_> (val))
{
}

VariantValue::VariantValue (const Date& val)
    : fVal_ (DEBUG_NEW TIRep_<Date> (val))
{
}

VariantValue::VariantValue (const DateTime& val)
    : fVal_ (DEBUG_NEW TIRep_<DateTime> (val))
{
}

VariantValue::VariantValue (const wchar_t* val)
    : fVal_ (DEBUG_NEW TIRep_<String> (val))
{
}

VariantValue::VariantValue (const wstring& val)
    : fVal_ (DEBUG_NEW TIRep_<String> (val))
{
}

VariantValue::VariantValue (const String& val)
    : fVal_ (DEBUG_NEW TIRep_<String> (val))
{
}

VariantValue::VariantValue (const map<wstring, VariantValue>& val)
    : fVal_ (DEBUG_NEW TIRep_<map<wstring, VariantValue>> (val))
{
}
namespace {
    map<wstring, VariantValue> cvt_(const Mapping<String, VariantValue>& val)
    {
        map<wstring, VariantValue>   tmp;
        for (auto i : val) {
            tmp.insert (map<wstring, VariantValue>::value_type (i.fKey.As<wstring> (), i.fValue));
        }
        return tmp;
    }
}
VariantValue::VariantValue (const Mapping<String, VariantValue>& val)
    : fVal_ (DEBUG_NEW TIRep_<map<wstring, VariantValue>> (cvt_ (val)))
{
}

VariantValue::VariantValue (const vector<VariantValue>& val)
    : fVal_ (DEBUG_NEW TIRep_<vector<VariantValue>> (val))
{
}

VariantValue::VariantValue (const Sequence<VariantValue>& val)
    : fVal_ (DEBUG_NEW TIRep_<vector<VariantValue>> (val.As<vector<VariantValue>> ()))
{
}

bool    VariantValue::empty () const
{
    if (fVal_.get () == nullptr) {
        return true;
    }
    switch (fVal_->GetType ()) {
        case    Type::eBoolean:
        case    Type::eInteger:
        case    Type::eUnsignedInteger: {
                return false;       // cannot be empty
            }
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
                AssertNotNull (v);
                return std::isnan (v->fVal) != 0;
            }
        case    Type::eDate: {
                auto    v   =   dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eMap: {
                auto    v   =   dynamic_cast<const TIRep_<map<wstring, VariantValue>>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eArray: {
                auto    v   =   dynamic_cast<const TIRep_<vector<VariantValue>>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        default: {
                return As<String> ().empty ();
            }
    }
}

template    <>
bool    VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return false;
    }
    switch (fVal_->GetType ()) {
        case    Type::eBoolean: {
                auto    v   =   dynamic_cast<const TIRep_<bool>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                //return tmp != L"false";           // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
                return As<String> () == L"true";   // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
            }
        case    Type::eInteger: {
                return As<IntegerType_> () != 0;
            }
        case    Type::eUnsignedInteger: {
                return As<UnsignedIntegerType_> () != 0;
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

VariantValue::IntegerType_ VariantValue::AsInteger_ () const
{
    if (fVal_.get () == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
                AssertNotNull (v);
                return static_cast<int> (v->fVal);
            }
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eUnsignedInteger: {
                auto    v   =   dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                return Characters::String2Int<IntegerType_> (v->fVal);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

VariantValue::UnsignedIntegerType_ VariantValue::AsUnsignedInteger_ () const
{
    if (fVal_.get () == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
                AssertNotNull (v);
                return static_cast<int> (v->fVal);
            }
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eUnsignedInteger: {
                auto    v   =   dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                // not sure this is right for high order bit set for unsigned long long?
                // --LGP 2013-08-25
                return Characters::String2Int<UnsignedIntegerType_> (v->fVal);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

VariantValue::FloatType_ VariantValue::AsFloatType_ () const
{
    if (fVal_.get () == nullptr) {
        return 0.0f;
    }
    switch (fVal_->GetType ()) {
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                return static_cast<float> (v->fVal);
            }
        case    Type::eUnsignedInteger: {
                auto    v   =   dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                return static_cast<float> (v->fVal);
            }
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                // NB. this may return NAN if string not a well-formed number (including empty string case)
                return Characters::String2Float (v->fVal);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
Date VariantValue::As() const
{
    if (fVal_.get () == nullptr) {
        return Date ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eDate: {
                auto    v   =   dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.GetDate ();
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                return Date::Parse (v->fVal, Date::ParseFormat::eISO8601);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
DateTime VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return DateTime ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eDate: {
                auto    v   =   dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
                AssertNotNull (v);
                return DateTime (v->fVal);
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                return DateTime::Parse (v->fVal, DateTime::ParseFormat::eISO8601);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

String  VariantValue::AsString_ () const
{
    if (fVal_.get () == nullptr) {
        return String ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eDate: {
                auto    v   =   dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.Format (Date::PrintFormat::eISO8601);
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.Format (DateTime::PrintFormat::eISO8601);
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<String>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eBoolean: {
                auto    v   =   dynamic_cast<const TIRep_<bool>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal ? L"true" : L"false";
            }
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                Assert (typeid (v->fVal) == typeid (long long));
                return Characters::Format (L"%lld", v->fVal);
            }
        case    Type::eUnsignedInteger: {
                auto    v   =   dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
                AssertNotNull (v);
                Assert (typeid (v->fVal) == typeid (unsigned long long));
                return Characters::Format (L"%llu", v->fVal);
            }
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
                AssertNotNull (v);
                return Characters::Float2String (v->fVal, Characters::Float2StringOptions (numeric_limits<FloatType_>::digits10));
            }
        case    Type::eMap: {
                auto    v   =   dynamic_cast<const TIRep_<map<wstring, VariantValue>>*> (fVal_.get ());
                AssertNotNull (v);
                wstringstream tmp;
                tmp << L"map[";
                for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                    if (i != v->fVal.begin ()) {
                        tmp << L", ";
                    }
                    tmp << i->first << L" -> " << i->second.As<wstring> ();
                }
                tmp << L"]";
                return tmp.str ();
            }
        case    Type::eArray: {
                auto    v   =   dynamic_cast<const TIRep_<vector<VariantValue>>*> (fVal_.get ());
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
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
map<wstring, VariantValue>   VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return map<wstring, VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eMap: {
                auto    v   =   dynamic_cast<const TIRep_<map<wstring, VariantValue>>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
Mapping<String, VariantValue>   VariantValue::As () const
{
#if     qBROKEN_MAPPING_CTOR_OF_STDMAP
    Mapping<String, VariantValue>   tmp;
    tmp.AddAll_pair (As<map<wstring, VariantValue>> ());
    return tmp;
#else
    return Mapping<String, VariantValue> (As <map<wstring, VariantValue>> ());
#endif
}

template    <>
vector<VariantValue> VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return vector<VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eArray: {
                auto    v   =   dynamic_cast<const TIRep_<vector<VariantValue>>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
Sequence<VariantValue> VariantValue::As () const
{
    return Sequence<VariantValue> (As<vector<VariantValue>> ());
}

bool    VariantValue::Equals (const VariantValue& rhs, bool exactTypeMatchOnly) const
{
    VariantValue::Type  lt  =   GetType ();
    VariantValue::Type  rt  =   rhs.GetType ();
    if (lt != rt) {
        if (exactTypeMatchOnly) {
            return false;
        }

        if (
            (lt == VariantValue::Type::eInteger or lt == VariantValue::Type::eUnsignedInteger) and
            (rt == VariantValue::Type::eInteger or rt == VariantValue::Type::eUnsignedInteger)
        ) {
            return As<UnsignedIntegerType_> () == rhs.As<UnsignedIntegerType_> ();
        }
        // special case - comparing a string with a bool or bool with a string
        if (
            (lt == VariantValue::Type::eBoolean and rt == VariantValue::Type::eString) or
            (lt == VariantValue::Type::eString and rt == VariantValue::Type::eBoolean)
        ) {
            // compare as STRING  - in case someone compares true with 'FRED' (we want that to come out as a FALSE compare result)
            return  As<String> () == rhs.As<String> ();
        }
        return false;
    }
    switch (lt) {
        case    VariantValue::Type::eNull:
            return true;
        case    VariantValue::Type::eBoolean:
            return As<bool> () == rhs.As<bool> ();
        case    VariantValue::Type::eInteger:
            return As<IntegerType_> () == rhs.As<IntegerType_> ();
        case    VariantValue::Type::eUnsignedInteger:
            return As<UnsignedIntegerType_> () == rhs.As<UnsignedIntegerType_> ();
        case    VariantValue::Type::eFloat:
            return As<VariantValue::FloatType_> () == rhs.As<VariantValue::FloatType_> ();
        case    VariantValue::Type::eDate:
            return As<Date> () == rhs.As<Date> ();
        case    VariantValue::Type::eDateTime:
            return As<DateTime> () == rhs.As<DateTime> ();
        case    VariantValue::Type::eString:
            return As<String> () == rhs.As<String> ();
        case    VariantValue::Type::eArray: {
                // same iff all elts same
                vector<VariantValue>    lhsV    =   As<vector<VariantValue>> ();
                vector<VariantValue>    rhsV    =   rhs.As<vector<VariantValue>> ();
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
        case    VariantValue::Type::eMap: {
                // same iff all elts same
                map<wstring, VariantValue>   lhsM    =   As<map<wstring, VariantValue>> ();
                map<wstring, VariantValue>   rhsM    =   rhs.As<map<wstring, VariantValue>> ();
                if (lhsM.size () != rhsM.size ()) {
                    return false;
                }
                map<wstring, VariantValue>::const_iterator li = lhsM.begin ();
                map<wstring, VariantValue>::const_iterator ri = rhsM.begin ();
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
