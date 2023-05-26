/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_h_
#define _Stroika_Foundation_Common_KeyValuePair_h_ 1

#include "../StroikaPreComp.h"

#include <compare>

#include <type_traits>
#include <utility>

#include "../Configuration/Common.h"

/**
 * TODO:
 *      @todo   Consider having KeyValuePair have an operator explict pair<Key,Value>() converter.
 */

namespace Stroika::Foundation::Common {

    /**
     *  Essentially the same as pair<KEY_TYPE,VALUE_TYPE> but with more clearly named data elements
     *  (when used for mappings, or other key/value pairs).
     *
     *  \note   moderately interoperable with pair<>
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    template <typename KEY_TYPE, typename VALUE_TYPE>
    struct KeyValuePair {
    public:
        using KeyType = KEY_TYPE;

    public:
        using ValueType = VALUE_TYPE;


        #if 1
        template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE, enable_if_t<is_default_constructible_v<K2> and is_default_constructible_v<V2>>* = nullptr>
        constexpr KeyValuePair ();
        KeyValuePair (const KeyValuePair&) = default;
        KeyValuePair (KeyValuePair&&)      = default;
        template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE,
                  enable_if_t<(is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and(is_convertible_v<const K2&, KEY_TYPE>and is_convertible_v<const V2&, VALUE_TYPE>)>* = nullptr>
        constexpr KeyValuePair (const KeyType& key, const ValueType& value);
        template <typename K2 = KEY_TYPE, typename V2 = VALUE_TYPE,
                  enable_if_t<(is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and not(is_convertible_v<const K2&, KEY_TYPE> and is_convertible_v<const V2&, VALUE_TYPE>)>* = nullptr>
        constexpr explicit KeyValuePair (const KeyType& key, const ValueType& value);
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<(is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and(
                      is_convertible_v<const KEY_TYPE2&, KEY_TYPE>and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src);
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<(is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and not(
                      is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr explicit KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src);
        // @todo DEBUG why changing below two is_constructible to is_constructible_v breaks on gcc --LGP 2018-07-10
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<(is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and
                              (is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src);
        template <typename KEY_TYPE2, typename VALUE_TYPE2,
                  enable_if_t<(is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and
                              not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>* = nullptr>
        constexpr explicit KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src);

        #else
    public:
        /**
         *  Similar logic for what overloads are available to pair<>, but here we also allow construction from a 'pair<>'.
         * 
         *  @todo cleanup - should be BOTH simpler, and handle a bunch of other cases. See if I can find a good example. pair<> code
         *  from visual studio uses a bunch of enable_if_t and is even more complex.
         */
        constexpr KeyValuePair ()
            requires (is_default_constructible_v<KEY_TYPE> and is_default_constructible_v<VALUE_TYPE>)
        = default;
        KeyValuePair (const KeyValuePair& src)
            requires (is_copy_constructible_v<KEY_TYPE> and is_copy_constructible_v<VALUE_TYPE>)
        = default;
        KeyValuePair (KeyValuePair&& src)
            requires (is_move_constructible_v<KEY_TYPE> and is_move_constructible_v<VALUE_TYPE>)
        = default;
        constexpr KeyValuePair (const KeyType& key, const ValueType& value)
            requires (is_copy_constructible_v<KEY_TYPE> and is_copy_constructible_v<VALUE_TYPE>);
        template <typename KEY_TYPE2, typename VALUE_TYPE2>
        constexpr KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
            requires (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>);
        template <typename KEY_TYPE2, typename VALUE_TYPE2>
        constexpr KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
            requires (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>);
        #endif

    public:
        /**
         */
        nonvirtual KeyValuePair& operator= (const KeyValuePair& rhs) = default;
        nonvirtual KeyValuePair& operator= (KeyValuePair&& rhs)      = default;
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
         *  Define operator<=> in the obvious way key <=> key first, and then if equal, compare values.
         *  This method is only defined if BOTH the key and value have a defined spaceship operator
         */
        constexpr auto operator<=> (const KeyValuePair&) const
            requires (Configuration::has_spaceship_v<KEY_TYPE> and Configuration::has_spaceship_v<VALUE_TYPE>);

    public:
        /**
         *  Define operator== in the obvious way key == key and value == value.
         *  This method is only defined if BOTH the key and value have a defined operator==
         */
        constexpr bool operator== (const KeyValuePair&) const
            requires (Configuration::has_eq_v<KEY_TYPE> and Configuration::has_eq_v<VALUE_TYPE>);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "KeyValuePair.inl"

#endif /*_Stroika_Foundation_Common_KeyValuePair_h_*/