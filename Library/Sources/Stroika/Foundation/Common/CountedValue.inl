/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_CountedValue_inl_
#define _Stroika_Foundation_Common_CountedValue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation {
    namespace Common {

        /*
         ********************************************************************************
         *************** Common::CountedValue<VALUE_TYPE, COUNTER_TYPE> *****************
         ********************************************************************************
         */
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        inline CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (typename Configuration::ArgByValueType<ValueType> value, CounterType count)
            : fValue (value)
            , fCount (count)
        {
        }
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        template <typename VALUE2_TYPE, typename COUNTER2_TYPE, typename ENABLE_IF_TEST>
        inline CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (pair<VALUE2_TYPE, COUNTER2_TYPE> src)
            : fValue (src.first)
            , fCount (src.second)
        {
        }
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        template <typename VALUE2_TYPE, typename COUNTER2_TYPE, typename ENABLE_IF_TEST>
        inline CountedValue<VALUE_TYPE, COUNTER_TYPE>::CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src)
            : fValue (src.fValue)
            , fCount (src.fCount)
        {
        }
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        inline bool CountedValue<VALUE_TYPE, COUNTER_TYPE>::Equals (const CountedValue<VALUE_TYPE, COUNTER_TYPE>& rhs) const
        {
            return fValue == rhs.fValue and fCount == rhs.fCount;
        }

        /*
         ********************************************************************************
         ********************** Common::CountedValue operators **************************
         ********************************************************************************
         */
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        inline bool operator== (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
        {
            return lhs.Equals (rhs);
        }
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        inline bool operator!= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs)
        {
            return not lhs.Equals (rhs);
        }
    }
}
#endif /*_Stroika_Foundation_Common_CountedValue_inl_*/
