/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_h_
#define _Stroika_Foundation_Common_KeyValuePair_h_ 1

#include    "../StroikaPreComp.h"

#include    <utility>

#include    "../Configuration/Common.h"



/**
 * TODO:
 *      @todo   Consider haivng KeyValuePair have an operator explict pair<Key,Value>() converter.
 *
 *      @todo   Consider adding Compare () method (enableif exists compare fucntions - operator<).
 *              And then implment operaotr<, etc using Comapre(). Leave independent Comapare and Equals
 *              so equals can be used forhtis whne types dont have operator <
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


            /**
             *  Essentially the same as pair<KEY_TYPE,VALUE_TYPE> but with more clearly named data elements
             *  (when used for mappings, or other key/value pairs).
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            struct  KeyValuePair {
            public:
                typedef KEY_TYPE    KeyType;
                typedef VALUE_TYPE  ValueType;

            public:
                KeyValuePair ();
                KeyValuePair (const pair<KeyType, ValueType>& value);
                KeyValuePair (const KeyType& key, const ValueType& value);

            public:
                KeyType     fKey;
                ValueType   fValue;

            public:
                /**
                 * @brief   Return true iff this object (both the key and value) are operator== to the rhs value.
                 */
                nonvirtual  bool    Equals (const KeyValuePair<KeyType, ValueType>& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Equals (const typename KeyValuePair<KeyType,ValueType>& rhs)
                 */
                nonvirtual  bool operator== (const KeyValuePair<KeyType, ValueType>& rhs) const;
                nonvirtual  bool operator!= (const KeyValuePair<KeyType, ValueType>& rhs) const;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "KeyValuePair.inl"

#endif  /*_Stroika_Foundation_Common_KeyValuePair_h_*/
