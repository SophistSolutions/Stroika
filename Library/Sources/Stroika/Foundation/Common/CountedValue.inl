/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_CountedValue_inl_
#define _Stroika_Foundation_Common_CountedValue_inl_ 1

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
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (typename Configuration::ArgByValueType<ValueType> value, CounterType count)
        : fValue (value)
        , fCount (count)
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename VALUE2_TYPE, typename COUNTER2_TYPE, enable_if_t<is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>>*>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (pair<VALUE2_TYPE, COUNTER2_TYPE> src)
        : fValue (src.first)
        , fCount (src.second)
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    template <typename VALUE2_TYPE, typename COUNTER2_TYPE, enable_if_t<is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>>*>
    constexpr CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src)
        : fValue (src.fValue)
        , fCount (src.fCount)
    {
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool CountedValue<VALUE_TYPE, COUNTER_TYPE>::Equals (const CountedValue<VALUE_TYPE, COUNTER_TYPE>& rhs) const
    {
        return typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::EqualsComparer{}(*this, rhs);
    }

    /*
     ********************************************************************************
     ********** CountedValue<VALUE_TYPE, COUNTER_TYPE>::EqualsComparer **************
     ********************************************************************************
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    constexpr bool CountedValue<VALUE_TYPE, COUNTER_TYPE>::EqualsComparer::operator() (const CountedValue& lhs, const CountedValue& rhs) const
    {
        return lhs.fValue == rhs.fValue and lhs.fValue == rhs.fValue;
    }

    /*
     ********************************************************************************
     ********** CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer ************
     ********************************************************************************
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    constexpr int CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer::operator() (const CountedValue& lhs, const CountedValue& rhs) const
    {
        if (int cmp = Common::ThreeWayCompare (lhs.fValue, rhs.fValue)) {
            return cmp;
        }
        return Common::ThreeWayCompare (lhs.fCount, rhs.fCount);
    }

    /*
     ********************************************************************************
     ********************** Common::CountedValue operators **************************
     ********************************************************************************
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator< (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer{}(lhs, rhs) < 0;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator<= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return not typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer{}(lhs, rhs) <= 0;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator== (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::EqualsComparer{}(lhs, rhs);
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator!= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return not typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::EqualsComparer{}(lhs, rhs);
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator>= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer{}(lhs, rhs) >= 0;
    }
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    inline bool operator> (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
    {
        return not typename CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer{}(lhs, rhs) > 0;
    }

}

#endif /*_Stroika_Foundation_Common_CountedValue_inl_*/
