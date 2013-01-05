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
    : fVal_ (DEBUG_NEW TIRep_<bool, Type::eBoolean> (val))
{
}

VariantValue::VariantValue (int val)
    : fVal_ (DEBUG_NEW TIRep_<int, Type::eInteger> (val))
{
}

VariantValue::VariantValue (float val)
    : fVal_ (DEBUG_NEW TIRep_<FloatType, Type::eFloat> (val))
{
}

VariantValue::VariantValue (double val)
    : fVal_ (DEBUG_NEW TIRep_<FloatType, Type::eFloat> (val))
{
}

VariantValue::VariantValue (const Date& val)
    : fVal_ (DEBUG_NEW TIRep_<Date, Type::eDate> (val))
{
}

VariantValue::VariantValue (const DateTime& val)
    : fVal_ (DEBUG_NEW TIRep_<DateTime, Type::eDateTime> (val))
{
}

VariantValue::VariantValue (const wchar_t* val)
    : fVal_ (DEBUG_NEW TIRep_<wstring, Type::eString> (val))
{
}

VariantValue::VariantValue (const wstring& val)
    : fVal_ (DEBUG_NEW TIRep_<wstring, Type::eString> (val))
{
}

VariantValue::VariantValue (const map<wstring, VariantValue>& val)
    : fVal_ (DEBUG_NEW TIRep_<map<wstring, VariantValue>, Type::eMap> (val))
{
}

VariantValue::VariantValue (const vector<VariantValue>& val)
    : fVal_ (DEBUG_NEW TIRep_<vector<VariantValue>, Type::eArray> (val))
{
}

bool    VariantValue::empty () const
{
    if (fVal_.get () == nullptr) {
        return true;
    }
    switch (fVal_->GetType ()) {
        case    Type::eBoolean:
        case    Type::eInteger: {
                return false;       // cannot be empty
            }
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType, Type::eFloat>*> (fVal_.get ());
                AssertNotNull (v);
                return std::isnan (v->fVal) != 0;
            }
        case    Type::eDate: {
                auto    v   =   dynamic_cast<const TIRep_<Date, Type::eDate>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime, Type::eDateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<wstring, Type::eString>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eMap: {
                auto    v   =   dynamic_cast<const TIRep_<map<wstring, VariantValue>, Type::eMap>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        case    Type::eArray: {
                auto    v   =   dynamic_cast<const TIRep_<vector<VariantValue>, Type::eArray>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.empty ();
            }
        default: {
                return As<wstring> ().empty ();
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
                auto    v   =   dynamic_cast<const TIRep_<bool, Type::eBoolean>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                //return tmp != L"false";           // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
                return As<wstring> () == L"true";   // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
            }
        case    Type::eInteger: {
                return As<int> () != 0;
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
int VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType, Type::eFloat>*> (fVal_.get ());
                AssertNotNull (v);
                return static_cast<int> (v->fVal);
            }
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<int, Type::eInteger>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<wstring, Type::eString>*> (fVal_.get ());
                AssertNotNull (v);
                return Characters::String2Int (v->fVal);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
float VariantValue::As () const
{
    return static_cast<float> (As<double> ());
}

template    <>
double VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return 0.0f;
    }
    switch (fVal_->GetType ()) {
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<int, Type::eInteger>*> (fVal_.get ());
                AssertNotNull (v);
                return static_cast<float> (v->fVal);
            }
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType, Type::eFloat>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<wstring, Type::eString>*> (fVal_.get ());
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
                auto    v   =   dynamic_cast<const TIRep_<Date, Type::eDate>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime, Type::eDateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.GetDate ();
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<wstring, Type::eString>*> (fVal_.get ());
                AssertNotNull (v);
                return Date::Parse (v->fVal, Date::PrintFormat::eISO8601);
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
                auto    v   =   dynamic_cast<const TIRep_<Date, Type::eDate>*> (fVal_.get ());
                AssertNotNull (v);
                return DateTime (v->fVal);
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime, Type::eDateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<wstring, Type::eString>*> (fVal_.get ());
                AssertNotNull (v);
                return DateTime::Parse (v->fVal, DateTime::PrintFormat::eISO8601);
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
wstring VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return wstring ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eDate: {
                auto    v   =   dynamic_cast<const TIRep_<Date, Type::eDate>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.Format ().As<wstring> ();
            }
        case    Type::eDateTime: {
                auto    v   =   dynamic_cast<const TIRep_<DateTime, Type::eDateTime>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal.Format ().As<wstring> ();
            }
        case    Type::eString: {
                auto    v   =   dynamic_cast<const TIRep_<wstring, Type::eString>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        case    Type::eBoolean: {
                auto    v   =   dynamic_cast<const TIRep_<bool, Type::eBoolean>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal ? L"true" : L"false";
            }
        case    Type::eInteger: {
                auto    v   =   dynamic_cast<const TIRep_<int, Type::eInteger>*> (fVal_.get ());
                AssertNotNull (v);
                return Characters::Format (L"%d", v->fVal);
            }
        case    Type::eFloat: {
                auto    v   =   dynamic_cast<const TIRep_<FloatType, Type::eFloat>*> (fVal_.get ());
                AssertNotNull (v);
                return Characters::Float2String (v->fVal);
            }
        case    Type::eMap: {
                auto    v   =   dynamic_cast<const TIRep_<map<wstring, VariantValue>, Type::eMap>*> (fVal_.get ());
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
                auto    v   =   dynamic_cast<const TIRep_<vector<VariantValue>, Type::eArray>*> (fVal_.get ());
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
                auto    v   =   dynamic_cast<const TIRep_<map<wstring, VariantValue> , Type::eMap>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

template    <>
vector<VariantValue> VariantValue::As () const
{
    if (fVal_.get () == nullptr) {
        return vector<VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case    Type::eArray: {
                auto    v   =   dynamic_cast<const TIRep_<vector<VariantValue>, Type::eArray>*> (fVal_.get ());
                AssertNotNull (v);
                return v->fVal;
            }
        default: {
                Execution::DoThrow (DataExchangeFormat::BadFormatException ());
            }
    }
}

bool    Memory::Equals (const VariantValue& lhs, const VariantValue& rhs, bool exactTypeMatchOnly)
{
    VariantValue::Type  lt  =   lhs.GetType ();
    VariantValue::Type  rt  =   rhs.GetType ();
    if (lt != rt) {
        if (exactTypeMatchOnly) {
            return false;
        }

        // specail case - comparing a string with a bool or bool with a string
        if (lt == VariantValue::Type::eBoolean and rt == VariantValue::Type::eString or
                lt == VariantValue::Type::eString and rt == VariantValue::Type::eBoolean
           ) {
            // compare as STRING  - in case someone compares true with 'FRED' (we want that to come out as a FALSE compare result)
            return  lhs.As<wstring> () == rhs.As<wstring> ();
        }
        return false;
    }
    switch (lt) {
        case    VariantValue::Type::eNull:
            return true;
        case    VariantValue::Type::eBoolean:
            return lhs.As<bool> () == rhs.As<bool> ();
        case    VariantValue::Type::eInteger:
            return lhs.As<int> () == rhs.As<int> ();
        case    VariantValue::Type::eFloat:
            return lhs.As<VariantValue::FloatType> () == rhs.As<VariantValue::FloatType> ();
        case    VariantValue::Type::eDate:
            return lhs.As<Date> () == rhs.As<Date> ();
        case    VariantValue::Type::eDateTime:
            return lhs.As<DateTime> () == rhs.As<DateTime> ();
        case    VariantValue::Type::eString:
            return lhs.As<wstring> () == rhs.As<wstring> ();
        case    VariantValue::Type::eArray: {
                // same iff all elts same
                vector<VariantValue>    lhsV    =   lhs.As<vector<VariantValue>> ();
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
                map<wstring, VariantValue>   lhsM    =   lhs.As<map<wstring, VariantValue>> ();
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
