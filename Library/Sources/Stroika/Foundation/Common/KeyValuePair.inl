/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_inl_
#define _Stroika_Foundation_Common_KeyValuePair_inl_ 1

#include "Compare.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ******************** Common::KeyValuePair<KEY_TYPE,VALUE_TYPE> *****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename K2, typename V2,
              enable_if_t<is_default_constructible_v<K2> and is_default_constructible_v<V2>>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair ()
        : fKey{}
        , fValue{}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename K2, typename V2,
              enable_if_t<
                  (is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and(is_convertible_v<const K2&, KEY_TYPE>and is_convertible_v<const V2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyType& key, const ValueType& value)
        : fKey (key)
        , fValue (value)
    {
        // NB: use non-uniform initialization since we allow for conversions of type - is_convertible_v above
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename K2, typename V2,
              enable_if_t<
                  (is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and not(is_convertible_v<const K2&, KEY_TYPE> and is_convertible_v<const V2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyType& key, const ValueType& value)
        : fKey (key)
        , fValue (value)
    {
        // NB: use non-uniform initialization since we allow for conversions of type - is_convertible_v above
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and(is_convertible_v<const KEY_TYPE2&, KEY_TYPE>and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.first)
        , fValue (src.second)
    {
        // NB: use non-uniform initialization since we allow for conversions of type - is_convertible_v above
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.first)
        , fValue (src.second)
    {
        // NB: use non-uniform initialization since we allow for conversions of type - is_convertible_v above
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and (is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.fKey)
        , fValue (src.fValue)
    {
        // NB: use non-uniform initialization since we allow for conversions of type - is_convertible_v above
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.fKey)
        , fValue (src.fValue)
    {
        // NB: use non-uniform initialization since we allow for conversions of type - is_convertible_v above
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2>
    KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (const pair<KEY_TYPE2, VALUE_TYPE2>& rhs)
    {
        fKey   = rhs.first;
        fValue = rhs.second;
        return *this;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2>
    inline KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& rhs)
    {
        rhs = rhs.fKey;
        rhs = rhs.fValue;
        return *this;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2>
    inline KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (pair<KEY_TYPE2, VALUE_TYPE2>&& rhs)
    {
        fKey   = forward<KEY_TYPE2> (rhs.first);
        fValue = forward<VALUE_TYPE2> (rhs.second);
        return *this;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2>
    inline KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (KeyValuePair<KEY_TYPE2, VALUE_TYPE2>&& rhs)
    {
        fKey   = forward<KEY_TYPE2> (rhs.fKey);
        fValue = forward<VALUE_TYPE2> (rhs.fValue);
        return *this;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename T1, typename T2, enable_if_t<Configuration::has_spaceship_v<T1> and Configuration::has_spaceship_v<T2>>*>
    constexpr inline auto KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator<=> (const KeyValuePair& rhs) const
    {
        auto cmp = fKey <=> rhs.fKey;
        if (cmp != strong_ordering::equal) {
            return cmp;
        }
        return fValue <=> rhs.fValue;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename T1, typename T2, enable_if_t<Configuration::has_eq_v<T1> and Configuration::has_eq_v<T2>>*>
    constexpr inline bool KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator== (const KeyValuePair& rhs) const
    {
        return fKey == rhs.fKey and fValue == rhs.fValue;
    }

}

#endif /*_Stroika_Foundation_Common_KeyValuePair_inl_*/