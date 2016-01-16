/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_CountedValue_h_
#define _Stroika_Foundation_Common_CountedValue_h_ 1

#include    "../StroikaPreComp.h"

#include    <utility>

#include    "../Configuration/Common.h"
#include    "../Configuration/TypeHints.h"



/**
 * TODO:
 *      @todo   add default CTOR iff VALUE_TYPE has default CTOR, and same for other
 *              ops like assignment.
 *
 *      @todo   Add Compare/operator< support
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


            /**
             *  CountedValue is the same as a value, but with a count as well.
             *
             *  Essentially the same as pair<VALUE_TYPE,COUNTER_TYPE> but with more clearly named data elements
             */
            template    <typename VALUE_TYPE, typename COUNTER_TYPE = unsigned int>
            struct  CountedValue {
            public:
                /**
                 */
                using   ValueType       =   VALUE_TYPE;

            public:
                /**
                 */
                using   CounterType     =   COUNTER_TYPE;

            public:
                /**
                 */
                CountedValue (typename Configuration::ArgByValueType<ValueType> value, typename Configuration::ArgByValueType<CounterType> count);
                template    <
                    typename VALUE2_TYPE,
                    typename COUNTER2_TYPE,
                    typename ENABLE_IF_TEST = typename enable_if < is_convertible<VALUE2_TYPE, VALUE_TYPE>::value && is_convertible<COUNTER2_TYPE, COUNTER_TYPE>::value >::type
                    >
                CountedValue (typename Configuration::ArgByValueType<pair<VALUE2_TYPE, COUNTER2_TYPE>> src);
                template    <
                    typename VALUE2_TYPE,
                    typename COUNTER2_TYPE,
                    typename ENABLE_IF_TEST = typename enable_if < is_convertible<VALUE2_TYPE, VALUE_TYPE>::value && is_convertible<COUNTER2_TYPE, COUNTER_TYPE>::value >::type
                    >
                CountedValue (const CountedValue<VALUE2_TYPE, COUNTER2_TYPE>& src);

            public:
                ValueType       fValue;
                CounterType     fCount;

            public:
                /**
                 * @brief   Return true iff this object (both the key and value) are operator== to the rhs value.
                 */
                nonvirtual  bool    Equals (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs) const;
            };


            /**
             *  operator indirects to CountedValue<>::Equals ()
             */
            template    <typename VALUE_TYPE, typename COUNTER_TYPE>
            bool    operator== (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);

            /**
             *  operator indirects to CountedValue<>::Equals ()
             */
            template    <typename VALUE_TYPE, typename COUNTER_TYPE>
            bool    operator!= (typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> lhs, typename Configuration::ArgByValueType<CountedValue<VALUE_TYPE, COUNTER_TYPE>> rhs);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CountedValue.inl"

#endif  /*_Stroika_Foundation_Common_CountedValue_h_*/
