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
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue ()
        requires (is_default_constructible_v<VALUE_TYPE>)
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
    template <typename VALUE2_TYPE, typename COUNTER2_TYPE>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const pair<VALUE2_TYPE, COUNTER2_TYPE>& src)
        requires (is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>)
        : fValue{src.first}
        , fCount{src.second}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename VALUE2_TYPE, typename COUNTER2_TYPE>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src)
        requires (is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>)
        : fValue{src.fValue}
        , fCount{src.fCount}
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    constexpr auto CountedValue<VALUE_TYPE, COUNTER_TYPE>::operator<=> (const CountedValue& rhs) const
        requires (Configuration::has_spaceship_v<VALUE_TYPE>)
    {
        if (auto cmp = fValue <=> rhs.fValue; cmp != strong_ordering::equal) {
            return cmp;
        }
        return fCount <=> rhs.fCount;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    constexpr bool CountedValue<VALUE_TYPE, COUNTER_TYPE>::operator== (const CountedValue& rhs) const
        requires (Configuration::has_eq_v<VALUE_TYPE>)
    {
        return fValue == rhs.fValue and fValue == rhs.fValue;
    }

}

#endif /*_Stroika_Foundation_Common_CountedValue_inl_*/
