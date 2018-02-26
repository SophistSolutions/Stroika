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
             *
             *  \note   moderately interoperable with pair<>
             */
            template <typename KEY_TYPE, typename VALUE_TYPE>
            struct KeyValuePair {
            public:
                using KeyType = KEY_TYPE;

            public:
                using ValueType = VALUE_TYPE;

            public:
                /**
                 */
                constexpr KeyValuePair ()          = default;
                KeyValuePair (const KeyValuePair&) = default;
                KeyValuePair (KeyValuePair&&)      = default;
                template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE,
                          enable_if_t<
                              is_copy_constructible<K2>::value and
                                  is_copy_constructible<V2>::value and
                                  is_convertible<const K2&, K2>::value and
                                  is_convertible<const V2&, V2>::value,
                              int>
                              ENABLE_IF = 0>
                constexpr KeyValuePair (const KeyType& key, const ValueType& value);
                template <typename KEY_TYPE2, typename VALUE_TYPE2,
                          enable_if_t<
                              is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                                  is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                                  is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and
                                  is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value,
                              int>
                              ENABLE_IF_CONVERTIBLE = 0>
                constexpr KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src);
                template <typename KEY_TYPE2, typename VALUE_TYPE2,
                          enable_if_t<
                              is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                                  is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                                  !(is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value),
                              int>
                              ENABLE_IF_NOT_CONVERTIBLE = 0>
                constexpr explicit KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src);
                template <typename KEY_TYPE2, typename VALUE_TYPE2,
                          enable_if_t<
                              is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                                  is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                                  (is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value),
                              int>
                              ENABLE_IF_CONVERTIBLE = 0>
                constexpr KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src);
                template <typename KEY_TYPE2, typename VALUE_TYPE2,
                          enable_if_t<
                              is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                                  is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                                  !(is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value),
                              int>
                              ENABLE_IF_NOT_CONVERTIBLE = 0>
                constexpr explicit KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src);

            public:
                /**
                 */
                nonvirtual KeyValuePair& operator= (const KeyValuePair& rhs) = default;
                nonvirtual KeyValuePair& operator= (KeyValuePair&& rhs) = default;
                template <typename KEY_TYPE2, typename VALUE_TYPE2>
                nonvirtual KeyValuePair& operator= (const pair<KEY_TYPE2, VALUE_TYPE2>& rhs);
                template <typename KEY_TYPE2, typename VALUE_TYPE2>
                nonvirtual KeyValuePair& operator= (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& rhs);
                template <typename KEY_TYPE2, typename VALUE_TYPE2>
                nonvirtual KeyValuePair& operator= (pair<KEY_TYPE2, VALUE_TYPE2>&& rhs);
                template <typename KEY_TYPE2, typename VALUE_TYPE2>
                nonvirtual KeyValuePair& operator= (KeyValuePair<KEY_TYPE2, VALUE_TYPE2>&& rhs);

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
