/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_h_
#define _Stroika_Foundation_Common_KeyValuePair_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>
#include <utility>

#include "../Configuration/Common.h"

/**
 * TODO:
 *      @todo   Consider having KeyValuePair have an operator explict pair<Key,Value>() converter.
 *
 *      @todo   Add Common::ThreeWayCompare () method (enableif default creatable) or spaceship operator).
 */

namespace Stroika::Foundation::Common {

    /**
     *  Essentially the same as pair<KEY_TYPE,VALUE_TYPE> but with more clearly named data elements
     *  (when used for mappings, or other key/value pairs).
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
         *  Similar logic for what overloads are available to pair<>, but here we also allow construction from a 'pair<>'.
         */
        template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE,
                  enable_if_t<is_default_constructible_v<K2> and is_default_constructible_v<V2>>* = nullptr>
        constexpr KeyValuePair ();
        KeyValuePair (const KeyValuePair&) = default;
        KeyValuePair (KeyValuePair&&)      = default;
        template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE,
                  enable_if_t<
                      (is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and(is_convertible_v<const K2&, KEY_TYPE>and is_convertible_v<const V2&, VALUE_TYPE>)>* = nullptr>
        constexpr KeyValuePair (const KeyType& key, const ValueType& value);
        template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE,
                  enable_if_t<
                      (is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and not(is_convertible_v<const K2&, KEY_TYPE> and is_convertible_v<const V2&, VALUE_TYPE>)>* = nullptr>
        constexpr explicit KeyValuePair (const KeyType& key, const ValueType& value);
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<
                      (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and(is_convertible_v<const KEY_TYPE2&, KEY_TYPE>and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src);
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<
                      (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr explicit KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src);
        // @todo DEBUG why changing below two is_constructible to is_constructible_v breaks on gcc --LGP 2018-07-10
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<
                      (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and (is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src);
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<
                      (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "KeyValuePair.inl"

#endif /*_Stroika_Foundation_Common_KeyValuePair_h_*/