/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_CountedValue_h_
#define _Stroika_Foundation_Common_CountedValue_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <utility>

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
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
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *      o   @todo COULD add EqualsComparer/ThreeWayComparer members which take explicit 'T' comparer argument
     */
    template <typename VALUE_TYPE, typename COUNTER_TYPE = unsigned int>
        requires (default_initializable<COUNTER_TYPE> and unsigned_integral<COUNTER_TYPE>)
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
        constexpr CountedValue ()
            requires (default_initializable<VALUE_TYPE>);
        constexpr CountedValue (typename Configuration::ArgByValueType<ValueType> value, CounterType count = 1);
        template <convertible_to<VALUE_TYPE> VALUE2_TYPE, convertible_to<COUNTER_TYPE> COUNTER2_TYPE>
        constexpr CountedValue (const pair<VALUE2_TYPE, COUNTER2_TYPE>& src);
        template <convertible_to<VALUE_TYPE> VALUE2_TYPE, convertible_to<COUNTER_TYPE> COUNTER2_TYPE>
        constexpr CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src);

    public:
        ValueType   fValue;
        CounterType fCount;

    public:
        /**
         */
        constexpr auto operator<=> (const CountedValue&) const
            requires (three_way_comparable<VALUE_TYPE>);

    public:
        /**
         */
        constexpr bool operator== (const CountedValue&) const
            requires (equality_comparable<VALUE_TYPE>);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CountedValue.inl"

#endif /*_Stroika_Foundation_Common_CountedValue_h_*/
