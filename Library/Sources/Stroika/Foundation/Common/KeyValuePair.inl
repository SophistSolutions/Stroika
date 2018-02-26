/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_inl_
#define _Stroika_Foundation_Common_KeyValuePair_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Common {

            /*
             ********************************************************************************
             ******************** Common::KeyValuePair<KEY_TYPE,VALUE_TYPE> *****************
             ********************************************************************************
             */
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename K2, typename V2,
                      enable_if_t<
                          is_copy_constructible<K2>::value and
                              is_copy_constructible<V2>::value and
                              is_convertible<const K2&, K2>::value and
                              is_convertible<const V2&, V2>::value,
                          int>
                          ENABLE_IF>
            inline constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyType& key, const ValueType& value)
                : fKey (key)
                , fValue (value)
            {
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename KEY_TYPE2, typename VALUE_TYPE2,
                      enable_if_t<
                          is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                              is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                              is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and
                              is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value,
                          int>
                          ENABLE_IF_CONVERTIBLE>
            inline constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
                : fKey (src.first)
                , fValue (src.second)
            {
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename KEY_TYPE2, typename VALUE_TYPE2,
                      enable_if_t<
                          is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                              is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                              !(is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value),
                          int>
                          ENABLE_IF_NOT_CONVERTIBLE>
            inline constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY_TYPE2, VALUE_TYPE2>& src)
                : fKey (src.first)
                , fValue (src.second)
            {
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename KEY_TYPE2, typename VALUE_TYPE2,
                      enable_if_t<
                          is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                              is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                              (is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value),
                          int>
                          ENABLE_IF_CONVERTIBLE>
            inline constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
                : fKey (src.fKey)
                , fValue (src.fValue)
            {
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename KEY_TYPE2, typename VALUE_TYPE2,
                      enable_if_t<
                          is_constructible<KEY_TYPE, const KEY_TYPE2&>::value and
                              is_constructible<VALUE_TYPE, const VALUE_TYPE2&>::value and
                              !(is_convertible<const KEY_TYPE2&, KEY_TYPE>::value and is_convertible<const VALUE_TYPE2&, VALUE_TYPE>::value),
                          int>
                          ENABLE_IF_NOT_CONVERTIBLE>
            inline constexpr KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& src)
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
            KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (const KeyValuePair<KEY_TYPE2, VALUE_TYPE2>& rhs)
            {
                rhs = _Right.fKey;
                rhs = _Right.fValue;
                return *this;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename KEY_TYPE2, typename VALUE_TYPE2>
            KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (pair<KEY_TYPE2, VALUE_TYPE2>&& rhs)
            {
                fKey   = std::forward<KEY_TYPE2> (rhs.first);
                fValue = std::forward<VALUE_TYPE2> (rhs.second);
                return *this;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            template <typename KEY_TYPE2, typename VALUE_TYPE2>
            KeyValuePair<KEY_TYPE, VALUE_TYPE>& KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator= (KeyValuePair<KEY_TYPE2, VALUE_TYPE2>&& rhs)
            {
                fKey   = std::forward<KEY_TYPE2> (rhs.fKey);
                fValue = std::forward<VALUE_TYPE2> (rhs.fValue);
                return *this;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            inline bool KeyValuePair<KEY_TYPE, VALUE_TYPE>::Equals (const KeyValuePair<KeyType, ValueType>& rhs) const
            {
                return fKey == rhs.fKey and fValue == rhs.fValue;
            }

            /*
             ********************************************************************************
             ************************** KeyValuePair operators ******************************
             ********************************************************************************
             */
            template <typename KEY_TYPE, typename VALUE_TYPE>
            inline bool operator== (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE>
            inline bool operator!= (const KeyValuePair<KEY_TYPE, VALUE_TYPE>& lhs, const KeyValuePair<KEY_TYPE, VALUE_TYPE>& rhs)
            {
                return not lhs.Equals (rhs);
            }
        }
    }
}
#endif /*_Stroika_Foundation_Common_KeyValuePair_inl_*/
