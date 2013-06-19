/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_KeyValuePair_h_
#define _Stroika_Foundation_Common_KeyValuePair_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"



/**
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


			/**
			 *	Essentially the same as pair<KEY_TYPE,VALUE_TYPE> but with more clearly named data elements (when used for mappings, or other
			 *	key/value pairs).
			 */
			template	<typename KEY_TYPE, typename VALUE_TYPE>
			struct KeyValuePair {
				typedef	KEY_TYPE	KeyType;
				typedef	VALUE_TYPE	ValueType;

				KeyValuePair ();
				KeyValuePair (const KeyType& key, const ValueType& value);

				KeyType		fKey;
				ValueType	fValue;
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
