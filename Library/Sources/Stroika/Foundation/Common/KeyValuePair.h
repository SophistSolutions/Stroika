/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_h_
#define _Stroika_Foundation_Common_KeyValuePair_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <concepts>
#include <type_traits>
#include <utility>

#include "Stroika/Foundation/Configuration/Common.h"

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
     *  \note   Supports KeyValuePair<T,void> - so second type maybe void (sometimes useful as degenerate case - e.g. SkipList)
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

    public:
        /**
         *  Similar logic for what overloads are available to pair<>, but here we also allow construction from a 'pair<>'.
         * 
         *  @todo cleanup - should be BOTH simpler, and handle a bunch of other cases. See if I can find a good example. pair<> code
         *  from visual studio uses a bunch of requires and is even more complex.
         */
        constexpr KeyValuePair ()
            requires (constructible_from<KEY_TYPE> and constructible_from<VALUE_TYPE>)
        = default;
        KeyValuePair (const KeyValuePair& src)
            requires (copy_constructible<KEY_TYPE> and copy_constructible<VALUE_TYPE>)
        = default;
        KeyValuePair (KeyValuePair&& src)
            requires (is_move_constructible_v<KEY_TYPE> and is_move_constructible_v<VALUE_TYPE>)
        = default;
        constexpr KeyValuePair (const KeyType& key, const ValueType& value)
            requires (copy_constructible<KEY_TYPE> and copy_constructible<VALUE_TYPE>);
        template <typename KEY_TYPE2, typename VALUE_TYPE2>
        constexpr KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
            requires (constructible_from<KEY_TYPE, const KEY_TYPE2&> and constructible_from<VALUE_TYPE, const VALUE_TYPE2&>);
        template <typename KEY_TYPE2, typename VALUE_TYPE2>
        constexpr KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
            requires (constructible_from<KEY_TYPE, const KEY_TYPE2&> and constructible_from<VALUE_TYPE, const VALUE_TYPE2&>);

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
         *  Define operator== in the obvious way key == key and value == value.
         *  This method is only defined if BOTH the key and value have a defined operator==
         */
        constexpr bool operator== (const KeyValuePair&) const
            requires (equality_comparable<KEY_TYPE> and equality_comparable<VALUE_TYPE>);

    public:
        /**
         *  Define operator<=> in the obvious way key <=> key first, and then if equal, compare values.
         *  This method is only defined if BOTH the key and value have a defined spaceship operator
         */
        constexpr auto operator<=> (const KeyValuePair&) const
            requires (three_way_comparable<KEY_TYPE> and three_way_comparable<VALUE_TYPE>);
    };

    template <typename KEY_TYPE>
    struct KeyValuePair<KEY_TYPE, void> {
    public:
        using KeyType = KEY_TYPE;

    public:
        using ValueType = void;

    public:
        /**
         *  Similar logic for what overloads are available to pair<>, but here we also allow construction from a 'pair<>'.
         * 
         *  @todo cleanup - should be BOTH simpler, and handle a bunch of other cases. See if I can find a good example. pair<> code
         *  from visual studio uses a bunch of require's and is even more complex.
         */
        constexpr KeyValuePair ()
            requires (constructible_from<KEY_TYPE>)
        = default;
        KeyValuePair (const KeyValuePair& src)
            requires (copy_constructible<KEY_TYPE>)
        = default;
        KeyValuePair (KeyValuePair&& src)
            requires (is_move_constructible_v<KEY_TYPE>)
        = default;
        template <typename KEY_TYPE2>
        constexpr KeyValuePair (const pair<KEY_TYPE2, void>& src)
            requires (constructible_from<KEY_TYPE, const KEY_TYPE2&>);
        template <typename KEY_TYPE2>
        constexpr KeyValuePair (const KeyValuePair<KEY_TYPE2, void>& src)
            requires (constructible_from<KEY_TYPE, const KEY_TYPE2&>);

    public:
        /**
         */
        nonvirtual KeyValuePair& operator= (const KeyValuePair& rhs) = default;
        nonvirtual KeyValuePair& operator= (KeyValuePair&& rhs)      = default;
        template <typename KEY_TYPE2>
        nonvirtual KeyValuePair& operator= (const pair<KEY_TYPE2, void>& rhs);
        template <typename KEY_TYPE2>
        nonvirtual KeyValuePair& operator= (const KeyValuePair<KEY_TYPE2, void>& rhs);
        template <typename KEY_TYPE2>
        nonvirtual KeyValuePair& operator= (pair<KEY_TYPE2, void>&& rhs);
        template <typename KEY_TYPE2>
        nonvirtual KeyValuePair& operator= (KeyValuePair<KEY_TYPE2, void>&& rhs);

    public:
        KeyType fKey{};

    public:
        /**
         *  Define operator== in the obvious way key == key and value == value.
         *  This method is only defined if BOTH the key and value have a defined operator==
         */
        constexpr bool operator== (const KeyValuePair&) const
            requires (equality_comparable<KEY_TYPE>);

    public:
        /**
         *  Define operator<=> in the obvious way key <=> key first, and then if equal, compare values.
         *  This method is only defined if BOTH the key and value have a defined spaceship operator
         */
        constexpr auto operator<=> (const KeyValuePair&) const
            requires (three_way_comparable<KEY_TYPE>);
    };

    namespace Private_ {
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        template <typename T1, typename T2 = void>
        struct is_KVP_ : std::false_type {};
        template <typename T1, typename T2>
        struct is_KVP_<KeyValuePair<T1, T2>> : std::true_type {};
#endif
    }

    /**
     */
    template <typename T>
    concept IKeyValuePair =
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        Private_::is_KVP_<T>::value
#else
        requires (T t) {
            {
                []<typename T1, typename T2> (KeyValuePair<T1, T2>) {}(t)
            };
        }
#endif
        ;
    static_assert (not IKeyValuePair<optional<int>>);
    static_assert (IKeyValuePair<KeyValuePair<int, int>>);
    static_assert (IKeyValuePair<KeyValuePair<int, void>>);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "KeyValuePair.inl"

#endif /*_Stroika_Foundation_Common_KeyValuePair_h_*/