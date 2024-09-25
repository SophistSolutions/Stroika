/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Common/Concepts.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     *************** Common::CountedValue<VALUE_TYPE, COUNTER_TYPE> *****************
     ********************************************************************************
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue ()
        requires (default_initializable<VALUE_TYPE>)
        : CountedValue{VALUE_TYPE{}}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (typename Common::ArgByValueType<ValueType> value, CounterType count)
        : fValue{value}
        , fCount{count}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
    template <convertible_to<VALUE_TYPE> VALUE2_TYPE, convertible_to<COUNTER_TYPE> COUNTER2_TYPE>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const pair<VALUE2_TYPE, COUNTER2_TYPE>& src)
        : fValue{src.first}
        , fCount{src.second}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
    template <convertible_to<VALUE_TYPE> VALUE2_TYPE, convertible_to<COUNTER_TYPE> COUNTER2_TYPE>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src)
        : fValue{src.fValue}
        , fCount{src.fCount}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
    constexpr auto CountedValue<VALUE_TYPE, COUNTER_TYPE>::operator<=> (const CountedValue& rhs) const
        requires (three_way_comparable<VALUE_TYPE>)
    {
        if (auto cmp = fValue <=> rhs.fValue; cmp != strong_ordering::equal) {
            return cmp;
        }
        return fCount <=> rhs.fCount;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
    constexpr bool CountedValue<VALUE_TYPE, COUNTER_TYPE>::operator== (const CountedValue& rhs) const
        requires (equality_comparable<VALUE_TYPE>)
    {
        return fValue == rhs.fValue and fValue == rhs.fValue;
    }

}
