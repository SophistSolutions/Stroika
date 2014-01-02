/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_inl_
#define _Stroika_Foundation_Common_KeyValuePair_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


            /*
             ********************************************************************************
             ******************** Common::KeyValuePair<KEY_TYPE,VALUE_TYPE> *****************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            inline  KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair ()
                : fKey ()
                , fValue ()
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            inline  KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyType& key, const ValueType& value)
                : fKey (key)
                , fValue (value)
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            template    <typename KEY2_TYPE, typename VALUE2_TYPE, typename ENABLE_IF_TEST>
            inline  KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const pair<KEY2_TYPE, VALUE2_TYPE>& value)
                : fKey (value.first)
                , fValue (value.second)
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            template    <typename KEY2_TYPE, typename VALUE2_TYPE, typename ENABLE_IF_TEST>
            inline  KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyValuePair<KEY2_TYPE, VALUE2_TYPE>& value)
                : fKey (value.fKey)
                , fValue (value.fValue)
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            inline  bool    KeyValuePair<KEY_TYPE, VALUE_TYPE>::Equals (const KeyValuePair<KeyType, ValueType>& rhs) const
            {
                return fKey == rhs.fKey and fValue == rhs.fValue;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            inline  bool    KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator== (const KeyValuePair<KeyType, ValueType>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            inline  bool    KeyValuePair<KEY_TYPE, VALUE_TYPE>::operator!= (const KeyValuePair<KeyType, ValueType>& rhs) const
            {
                return not Equals (rhs);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Common_KeyValuePair_inl_*/
