/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_VariantValue_inl_
#define _Stroika_Foundation_DataExchange_VariantValue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/BlockAllocated.h"

namespace Stroika::Foundation::DataExchange {

    /**
     */
    struct VariantValue::IRep_ {
        virtual ~IRep_ ()             = default;
        virtual Type GetType () const = 0;
    };

    /*
     ********************************************************************************
     ********************************** VariantValue ********************************
     ********************************************************************************
     */
    inline VariantValue::VariantValue (nullptr_t)
        : VariantValue{}
    {
    }
    inline VariantValue::VariantValue (const vector<VariantValue>& val)
        : VariantValue{Sequence<VariantValue> (val)}
    {
    }
    template <Characters::IConvertibleToString STRINGISH_T>
    inline VariantValue::VariantValue (STRINGISH_T&& val)
        requires (not is_same_v<remove_cvref_t<STRINGISH_T>, String>)
        : VariantValue{String{forward<STRINGISH_T> (val)}}
    {
    }
    template <typename T>
    inline VariantValue::VariantValue (const optional<T>& val)
        requires (is_convertible_v<T, VariantValue>)
        : VariantValue{val.has_value () ? VariantValue{*val} : VariantValue{}}
    {
    }
    template <typename T>
    inline VariantValue& VariantValue::operator= (T&& val)
        requires (requires (T x) { VariantValue{x}; })
    {
        *this = VariantValue{forward<T> (val)};
        return *this;
    }
    inline VariantValue::Type VariantValue::GetType () const
    {
        if (fVal_ == nullptr) {
            return Type::eNull;
        }
        return fVal_->GetType ();
    }
    inline VariantValue::operator bool () const
    {
        return fVal_ != nullptr;
    }
    template <typename RETURNTYPE>
    inline RETURNTYPE VariantValue::As () const
        requires (same_as<RETURNTYPE, bool> or same_as<RETURNTYPE, BLOB> or integral<RETURNTYPE> or floating_point<RETURNTYPE> or
                  same_as<RETURNTYPE, Date> or same_as<RETURNTYPE, DateTime> or same_as<RETURNTYPE, wstring> or same_as<RETURNTYPE, String> or
                  same_as<RETURNTYPE, Mapping<String, VariantValue>> or same_as<RETURNTYPE, Sequence<VariantValue>> or
                  same_as<RETURNTYPE, map<wstring, VariantValue>> or same_as<RETURNTYPE, vector<VariantValue>>
#if qHasFeature_boost
                  or same_as<RETURNTYPE, boost::json::value>
#endif
        )
    {
        if constexpr (same_as<RETURNTYPE, bool>) {
            return this->AsBool_ ();
        }
        else if constexpr (same_as<RETURNTYPE, Memory::BLOB>) {
            return this->AsBLOB_ ();
        }
        else if constexpr (signed_integral<RETURNTYPE>) {
            return static_cast<RETURNTYPE> (this->AsInteger_ ());
        }
        else if constexpr (unsigned_integral<RETURNTYPE>) {
            return static_cast<RETURNTYPE> (this->AsUnsignedInteger_ ());
        }
        else if constexpr (floating_point<RETURNTYPE>) {
            return static_cast<RETURNTYPE> (this->AsFloatType_ ());
        }
        else if constexpr (same_as<RETURNTYPE, Time::Date>) {
            return this->AsDate_ ();
        }
        else if constexpr (same_as<RETURNTYPE, Time::DateTime>) {
            return this->AsDateTime_ ();
        }
        else if constexpr (same_as<RETURNTYPE, wstring>) {
            return this->AsString_ ().As<wstring> ();
        }
        else if constexpr (same_as<RETURNTYPE, String>) {
            return this->AsString_ ();
        }
        else if constexpr (same_as<RETURNTYPE, Mapping<String, VariantValue>>) {
            return this->AsMapping_ ();
        }
        else if constexpr (same_as<RETURNTYPE, Sequence<VariantValue>>) {
            return this->AsSequence_ ();
        }
        else if constexpr (same_as<RETURNTYPE, map<wstring, VariantValue>>) {
            return this->AsMapping_ ().Map<map<wstring, VariantValue>> ([] (auto v) -> pair<wstring, VariantValue> {
                return {v.fKey.As<wstring> (), v.fValue};
            });
        }
        else if constexpr (same_as<RETURNTYPE, vector<VariantValue>>) {
            return this->AsSequence_ ().As<vector<VariantValue>> ();
        }
#if qHasFeature_boost
        else if constexpr (same_as<RETURNTYPE, boost::json::value>) {
            return this->AsBoostJSONValue_ ();
        }
#endif
    }
    inline strong_ordering VariantValue::operator<=> (const VariantValue& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline bool VariantValue::operator== (const VariantValue& rhs) const
    {
        Ensure (EqualsComparer{}(*this, rhs) == (ThreeWayComparer{}(*this, rhs) == 0)); // These must return the same answer
        return EqualsComparer{}(*this, rhs);
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<DataExchange::VariantValue::Type> DefaultNames<DataExchange::VariantValue::Type>::k{{{
        {DataExchange::VariantValue::eNull, L"Null"},
        {DataExchange::VariantValue::eBLOB, L"BLOB"},
        {DataExchange::VariantValue::eBoolean, L"Boolean"},
        {DataExchange::VariantValue::eInteger, L"Integer"},
        {DataExchange::VariantValue::eUnsignedInteger, L"Unsigned-Integer"},
        {DataExchange::VariantValue::eFloat, L"Float"},
        {DataExchange::VariantValue::eDate, L"Date"},
        {DataExchange::VariantValue::eDateTime, L"DateTime"},
        {DataExchange::VariantValue::eString, L"String"},
        {DataExchange::VariantValue::eArray, L"Array"},
        {DataExchange::VariantValue::eMap, L"Map"},
    }}};
}

#endif /*_Stroika_Foundation_DataExchange_VariantValue_inl_*/
