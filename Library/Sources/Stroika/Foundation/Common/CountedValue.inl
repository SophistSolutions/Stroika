/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_CountedValue_inl_
#define _Stroika_Foundation_Common_CountedValue_inl_ 1

#include "../Configuration/Concepts.h"

#include "Compare.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     *************** Common::CountedValue<VALUE_TYPE, COUNTER_TYPE> *****************
     ********************************************************************************
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename VTEST, enable_if_t<is_default_constructible_v<VTEST>>*>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue ()
        : CountedValue{VALUE_TYPE{}}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (typename Configuration::ArgByValueType<ValueType> value, CounterType count)
        : fValue{value}
        , fCount{count}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename VALUE2_TYPE, typename COUNTER2_TYPE, enable_if_t<is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>>*>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const pair<VALUE2_TYPE, COUNTER2_TYPE>& src)
        : fValue{src.first}
        , fCount{src.second}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename VALUE2_TYPE, typename COUNTER2_TYPE, enable_if_t<is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>>*>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src)
        : fValue{src.fValue}
        , fCount{src.fCount}
    {
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename TEST, enable_if_t<Configuration::has_spaceship_v<TEST>>*>
    constexpr auto CountedValue<VALUE_TYPE, COUNTER_TYPE>::operator<=> (const CountedValue& rhs) const
    {
        auto cmp = fValue <=> rhs.fValue;
        if (cmp != Common::kEqual) {
            return cmp;
        }
        return fCount <=> rhs.fCount;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename TEST, enable_if_t<Configuration::has_eq_v<TEST>>*>
    constexpr bool CountedValue<VALUE_TYPE, COUNTER_TYPE>::operator== (const CountedValue& rhs) const
    {
        return fValue == rhs.fValue and fValue == rhs.fValue;
    }
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ********************** Common::CountedValue operators **************************
     ********************************************************************************
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator< (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        Common::strong_ordering cmp = Common::ThreeWayCompare (lhs.fValue, rhs.fValue);
        if (cmp != Common::kEqual) {
            return cmp < 0;
        }
        return Common::ThreeWayCompare (lhs.fCount, rhs.fCount) < 0;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator<= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return operator< (lhs, rhs) or operator== (lhs, rhs);
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE, enable_if_t<Configuration::has_eq_v<VALUE_TYPE>>*>
    inline bool operator== (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return lhs.fValue == rhs.fValue and lhs.fValue == rhs.fValue;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator!= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return not(lhs == rhs);
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator>= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return not(lhs < rhs);
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator> (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return not(lhs <= rhs);
    }
#endif

}

#endif /*_Stroika_Foundation_Common_CountedValue_inl_*/
