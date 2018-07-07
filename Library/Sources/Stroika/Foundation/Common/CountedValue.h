/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *      @todo   Add Compare/operator< support
 *
 *      @todo   Use typename Configuration::ArgByValueType<> more (one missing CTOR and Equals). On VS2k13 generates
 *              compiler errors I dont understand. Low pri...
 */

namespace Stroika::Foundation {
    namespace Common {

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
            CountedValue (typename Configuration::ArgByValueType<ValueType> value, CounterType count = 1);
            template <
                typename VALUE2_TYPE,
                typename COUNTER2_TYPE,
                typename ENABLE_IF_TEST = enable_if_t<is_convertible<VALUE2_TYPE, VALUE_TYPE>::value and is_convertible<COUNTER2_TYPE, COUNTER_TYPE>::value>>
            CountedValue (pair<VALUE2_TYPE, COUNTER2_TYPE> src);
            template <
                typename VALUE2_TYPE,
                typename COUNTER2_TYPE,
                typename ENABLE_IF_TEST = enable_if_t<is_convertible<VALUE2_TYPE, VALUE_TYPE>::value and is_convertible<COUNTER2_TYPE, COUNTER_TYPE>::value>>
            CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src);

        public:
            ValueType   fValue;
            CounterType fCount;

        public:
            /**
             * @brief   Return true iff this object (both the key and value) are operator== to the rhs value.
             */
            nonvirtual bool Equals (const CountedValue<VALUE_TYPE, COUNTER_TYPE>& rhs) const;
        };

        /**
         *  operator indirects to CountedValue<>::Equals ()
         */
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        bool operator== (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);

        /**
         *  operator indirects to CountedValue<>::Equals ()
         */
        template <typename VALUE_TYPE, typename COUNTER_TYPE>
        bool operator!= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CountedValue.inl"

#endif /*_Stroika_Foundation_Common_CountedValue_h_*/
