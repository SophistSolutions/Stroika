/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

    /*
     *  NB: we use enable_shared_from_this<> for performance reasons, not for any semantic purpose
     */
    struct VariantValue::IRep_ : public conditional_t<kVariantValueUsesStroikaSharedPtr_, Memory::enable_shared_from_this<VariantValue::IRep_>, enable_shared_from_this<VariantValue::IRep_>> {
        virtual ~IRep_ () {}
        virtual Type GetType () const = 0;
    };

    /*
     ********************************************************************************
     ********************************** VariantValue ********************************
     ********************************************************************************
     */
    template <typename T, typename... ARGS_TYPE>
    inline VariantValue::SharedRepImpl_<T> VariantValue::MakeSharedPtr_ (ARGS_TYPE&&... args)
    {
        if constexpr (kVariantValueUsesStroikaSharedPtr_) {
            return Memory::MakeSharedPtr<T> (forward<ARGS_TYPE> (args)...);
        }
        else {
            return make_shared<T> (forward<ARGS_TYPE> (args)...);
        }
    }
    inline VariantValue::VariantValue (nullptr_t)
        : VariantValue ()
    {
    }
    inline VariantValue::VariantValue (const vector<VariantValue>& val)
        : VariantValue (Sequence<VariantValue> (val))
    {
    }
    inline VariantValue::VariantValue (VariantValue&& src) noexcept
        : fVal_ (move (src.fVal_))
    {
    }
    inline VariantValue& VariantValue::operator= (VariantValue&& rhs) noexcept
    {
        fVal_ = move (rhs.fVal_);
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
        return GetType () != Type::eNull;
    }
    template <>
    inline Memory::BLOB VariantValue::As () const
    {
        return AsBLOB_ ();
    }
    template <>
    inline signed char VariantValue::As () const
    {
        return static_cast<signed char> (AsInteger_ ());
    }
    template <>
    inline short int VariantValue::As () const
    {
        return static_cast<short> (AsInteger_ ());
    }
    template <>
    inline int VariantValue::As () const
    {
        return static_cast<int> (AsInteger_ ());
    }
    template <>
    inline long int VariantValue::As () const
    {
        return static_cast<long int> (AsInteger_ ());
    }
    template <>
    inline long long int VariantValue::As () const
    {
        return static_cast<long long int> (AsInteger_ ());
    }
    template <>
    inline unsigned char VariantValue::As () const
    {
        return static_cast<unsigned char> (AsUnsignedInteger_ ());
    }
    template <>
    inline unsigned short int VariantValue::As () const
    {
        return static_cast<unsigned short int> (AsUnsignedInteger_ ());
    }
    template <>
    inline unsigned int VariantValue::As () const
    {
        return static_cast<unsigned int> (AsUnsignedInteger_ ());
    }
    template <>
    inline unsigned long int VariantValue::As () const
    {
        return static_cast<unsigned long int> (AsUnsignedInteger_ ());
    }
    template <>
    inline unsigned long long int VariantValue::As () const
    {
        return static_cast<unsigned long long int> (AsUnsignedInteger_ ());
    }
    template <>
    inline float VariantValue::As () const
    {
        return static_cast<float> (AsFloatType_ ());
    }
    template <>
    inline double VariantValue::As () const
    {
        return static_cast<double> (AsFloatType_ ());
    }
    template <>
    inline long double VariantValue::As () const
    {
        return static_cast<long double> (AsFloatType_ ());
    }
    template <>
    inline wstring VariantValue::As () const
    {
        return AsString_ ().As<wstring> ();
    }
    template <>
    inline String VariantValue::As () const
    {
        return AsString_ ();
    }
    [[deprecated ("in Stroika v2.1d24 - use Common::ThreeWayCompare () or ThreeWayComparer{} () instead")]] inline int VariantValue::Compare (const VariantValue& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }

    /*
     ********************************************************************************
     ************************* VariantValue operators *******************************
     ********************************************************************************
     */
    inline bool operator< (const VariantValue& lhs, const VariantValue& rhs)
    {
        return VariantValue::ThreeWayComparer{}(lhs, rhs) < 0;
    }
    inline bool operator<= (const VariantValue& lhs, const VariantValue& rhs)
    {
        return VariantValue::ThreeWayComparer{}(lhs, rhs) <= 0;
    }
    inline bool operator== (const VariantValue& lhs, const VariantValue& rhs)
    {
        return equal_to<VariantValue>{}(lhs, rhs);
    }
    inline bool operator!= (const VariantValue& lhs, const VariantValue& rhs)
    {
        return not equal_to<VariantValue>{}(lhs, rhs);
    }
    inline bool operator>= (const VariantValue& lhs, const VariantValue& rhs)
    {
        return VariantValue::ThreeWayComparer{}(lhs, rhs) >= 0;
    }
    inline bool operator> (const VariantValue& lhs, const VariantValue& rhs)
    {
        return VariantValue::ThreeWayComparer{}(lhs, rhs) > 0;
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<DataExchange::VariantValue::Type> : EnumNames<DataExchange::VariantValue::Type> {
        static constexpr EnumNames<DataExchange::VariantValue::Type> k{
            EnumNames<DataExchange::VariantValue::Type>::BasicArrayInitializer{
                {
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
        DefaultNames ()
            : EnumNames<DataExchange::VariantValue::Type> (k)
        {
        }
    };
}
namespace std {
    /*
     ********************************************************************************
     ******** equal_to <Stroika::Foundation::DataExchange::VariantValue> ************
     ********************************************************************************
     */
    constexpr std::equal_to<Stroika::Foundation::DataExchange::VariantValue>::equal_to (bool exactTypeMatchOnly)
        : fExactTypeMatchOnly (exactTypeMatchOnly)
    {
    }
}

#endif /*_Stroika_Foundation_DataExchange_VariantValue_inl_*/
