/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_h_
#define _Stroika_Foundation_Common_KeyValuePair_h_ 1

#include "../StroikaPreComp.h"

#include <utility>

#include "../Configuration/Common.h"

/**
 * TODO:
 *      @todo   Consider haivng KeyValuePair have an operator explict pair<Key,Value>() converter.
 *
 *      @todo   Consider adding Compare () method (enableif exists compare fucntions - operator<).
 *              And then implment operaotr<, etc using Comapre(). Leave independent Comapare and Equals
 *              so equals can be used forhtis whne types dont have operator <
 */

namespace Stroika {
    namespace Foundation {
        namespace Common {

            /**
             *  Essentially the same as pair<KEY_TYPE,VALUE_TYPE> but with more clearly named data elements
             *  (when used for mappings, or other key/value pairs).
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            template <typename KEY_TYPE, typename VALUE_TYPE>
            struct KeyValuePair {
            public:
                using KeyType   = KEY_TYPE;
                using ValueType = VALUE_TYPE;

            public:
                KeyValuePair () = default;
                constexpr KeyValuePair (typename Configuration::ArgByValueType<KeyType> key, typename Configuration::ArgByValueType<ValueType> value);
                template <
                    typename KEY2_TYPE,
                    typename VALUE2_TYPE,
                    typename ENABLE_IF_TEST = typename enable_if<is_convertible<KEY2_TYPE, KEY_TYPE>::value && is_convertible<VALUE2_TYPE, VALUE_TYPE>::value>::type>
                constexpr KeyValuePair (const pair<KEY2_TYPE, VALUE2_TYPE>& value);
                template <
                    typename KEY2_TYPE,
                    typename VALUE2_TYPE,
                    typename ENABLE_IF_TEST = typename enable_if<is_convertible<KEY2_TYPE, KEY_TYPE>::value && is_convertible<VALUE2_TYPE, VALUE_TYPE>::value>::type>
                constexpr KeyValuePair (const KeyValuePair<KEY2_TYPE, VALUE2_TYPE>& value);

            public:
                KeyType   fKey{};
                ValueType fValue{};

            public:
                /**
                 * @brief   Return true iff this object (both the key and value) are operator== to the rhs value.
                 */
                nonvirtual bool Equals (const KeyValuePair<KeyType, ValueType>& rhs) const;
            };

            /**
             *  operator indirects to KeyValuePair<>::Equals ()
             */
            template <typename KEY_TYPE, typename VALUE_TYPE>
            bool operator== (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs);

            /**
             *  operator indirects to KeyValuePair<>::Equals ()
             */
            template <typename KEY_TYPE, typename VALUE_TYPE>
            bool operator!= (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "KeyValuePair.inl"

#endif /*_Stroika_Foundation_Common_KeyValuePair_h_*/
