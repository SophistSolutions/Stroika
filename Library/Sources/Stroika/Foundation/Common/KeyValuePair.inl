/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_inl_
#define _Stroika_Foundation_Common_KeyValuePair_inl_ 1

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
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename K2, typename V2,
              enable_if_t<
                  (is_copy_constructible_v<K2> and is_copy_constructible_v<V2>)and not(is_convertible_v<const K2&, KEY_TYPE> and is_convertible_v<const V2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyType& key, const ValueType& value)
        : fKey (key)
        , fValue (value)
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and(is_convertible_v<const KEY_TYPE2&, KEY_TYPE>and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.first)
        , fValue (src.second)
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible_v<VALUE_TYPE, const VALUE_TYPE2&>)and not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.first)
        , fValue (src.second)
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and (is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.fKey)
        , fValue (src.fValue)
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename KEY_TYPE2, typename VALUE_TYPE2,
              enable_if_t<
                  (is_constructible_v<KEY_TYPE, const KEY_TYPE2&> and is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value) and not(is_convertible_v<const KEY_TYPE2&, KEY_TYPE> and is_convertible_v<const VALUE_TYPE2&, VALUE_TYPE>)>*>
    constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
        : fKey (src.fKey)
        , fValue (src.fValue)
    {
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
    inline bool KeyValuePair<KEY_TYPE, VALUE_TYPE>::Equals (const KeyValuePair<KeyType, ValueType>& rhs) const
    {
        return fKey == rhs.fKey and fValue == rhs.fValue;
    }

    /*
     ********************************************************************************
     ************* KeyValuePair<KEY_TYPE, VALUE_TYPE>::EqualsComparer ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE>
    constexpr bool KeyValuePair<KEY_TYPE, VALUE_TYPE>::EqualsComparer::operator() (const KeyValuePair& lhs, const KeyValuePair& rhs) const
    {
        return lhs.fKey == rhs.fKey and lhs.fValue == rhs.fValue;
    }

    /*
     ********************************************************************************
     *********** KeyValuePair<KEY_TYPE, VALUE_TYPE>::ThreeWayComparer ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE>
    constexpr int KeyValuePair<KEY_TYPE, VALUE_TYPE>::ThreeWayComparer::operator() (const KeyValuePair& lhs, const KeyValuePair& rhs) const
    {
        if (int cmp = Common::ThreeWayCompare<KEY_TYPE>{}(lhs.fKey, rhs.fKey)) {
            return cmp;
        }
        return Common::ThreeWayCompare<VALUE_TYPE>{}(lhs.fValue, rhs.fValue);
    }

    /*
     ********************************************************************************
     ********************** KeyValuePair comparison operators ***********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool operator< (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
    {
        return Common::ThreeWayCompare<KeyValuePair<KEY_TYPE, VALUE_TYPE>>{}(lhs, rhs) < 0;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool operator<= (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
    {
        return Common::ThreeWayCompare<KeyValuePair<KEY_TYPE, VALUE_TYPE>>{}(lhs, rhs) <= 0;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool operator== (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
    {
        return typename KeyValuePair<KEY_TYPE, VALUE_TYPE>::EqualsComparer{}(lhs, rhs);
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool operator!= (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
    {
        return not typename KeyValuePair<KEY_TYPE, VALUE_TYPE>::EqualsComparer{}(lhs, rhs);
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool operator>= (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
    {
        return Common::ThreeWayCompare<KeyValuePair<KEY_TYPE, VALUE_TYPE>>{}(lhs, rhs) >= 0;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool operator> (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
    {
        return Common::ThreeWayCompare<KeyValuePair<KEY_TYPE, VALUE_TYPE>>{}(lhs, rhs) > 0;
    }

}

#endif /*_Stroika_Foundation_Common_KeyValuePair_inl_*/