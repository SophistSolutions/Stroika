/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Dictionary_h_
#define _Stroika_Foundation_Containers_Dictionary_h_  1

/*
 *
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Mapping.hh
 *
 *      (o)         Should inherit from Iterable<T>
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A Dictionary is a Mapping<Key,T> which remains sorted (iterator) by the Key.
             *
             *
             *  Note the base class method Keys() will always return an Iterable<Key> in sorted order.
             *
             *  @see    Dictionary<Key,T>
             */
            template    <class Key, class T>
            class   Dictionary : public Mapping<Key, T> {
            public:
                Dictionary ();
                Dictionary (const Mapping<Key, T>& src);

            public:
                nonvirtual  Dictionary<Key, T>& operator= (const Dictionary<Key, T>& src);
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Mapping_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Dictionary.inl"
