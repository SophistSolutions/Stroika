/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_CountedValue_h_
#define _Stroika_Foundation_Common_CountedValue_h_ 1

#include "../StroikaPreComp.h"

#include <utility>

#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"

/**
 * TODO:
 *      @todo   add default CTOR iff VALUE_TYPE has default CTOR, and same for other
 *              ops like assignment.
 *
 *      @todo   Use typename Configuration::ArgByValueType<> more (one missing CTOR and Equals). On VS2k13 generates
 *              compiler errors I don't understand. Low pri...
 */

namespace Stroika::Foundation::Common {

    /**
     *  CountedValue is the same as a value, but with a count as well.
     *
     *  Essentially the same as pair<VALUE_TYPE,COUNTER_TYPE> but with more clearly named data elements
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE = unsigned int>
    struct CountedValue {
    public:
        /**
         */
        using ValueType = VALUE_TYPE;

    public:
        /**
         */
        using CounterType = COUNTER_TYPE;

    public:
        /**
         */
        constexpr CountedValue (typename Configuration::ArgByValueType<ValueType> value, CounterType count = 1);
        template <
            typename VALUE2_TYPE,
            typename COUNTER2_TYPE,
            enable_if_t<is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>>* = nullptr>
        constexpr CountedValue (pair<VALUE2_TYPE, COUNTER2_TYPE> src);
        template <
            typename VALUE2_TYPE,
            typename COUNTER2_TYPE,
            enable_if_t<is_convertible_v<VALUE2_TYPE, VALUE_TYPE> and is_convertible_v<COUNTER2_TYPE, COUNTER_TYPE>>* = nullptr>
        constexpr CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src);

    public:
        ValueType   fValue;
        CounterType fCount;

    public:
        struct EqualsComparer;

    public:
        struct ThreeWayComparer;
    };

    /**
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    struct CountedValue<VALUE_TYPE, COUNTER_TYPE>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr bool operator() (const CountedValue& lhs, const CountedValue& rhs) const;
    };

    /**
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    struct CountedValue<VALUE_TYPE, COUNTER_TYPE>::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr int operator() (const CountedValue& lhs, const CountedValue& rhs) const;
    };

    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @CountedValue<KEY_TYPE, VALUE_TYPE>::ThreeWayComparer (const Version& rhs), and EqualsComparer
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    bool operator< (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    bool operator<= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    bool operator== (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    bool operator!= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    bool operator> (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);
    template <typename VALUE_TYPE, typename COUNTER_TYPE>
    bool operator>= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CountedValue.inl"

#endif /*_Stroika_Foundation_Common_CountedValue_h_*/
