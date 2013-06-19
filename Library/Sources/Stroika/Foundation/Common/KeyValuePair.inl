/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
            KeyValuePair<KEY_TYPE, VALUE_TYPE>::KeyValuePair (const KeyType& key, const ValueType& value)
                : fKey (key)
                , fValue (value)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Common_KeyValuePair_inl_*/
