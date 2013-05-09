/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_h_
#define _Stroika_Foundation_Containers_SortedMapping_h_  1

/*
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 */


#include    "../StroikaPreComp.h"

#include    "Mapping.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedMapping is a Mapping<Key,T> which remains sorted (iterator) by the Key.
             *
             *  Note - this class might have been called "Dictionary".
             *
             *  @see    Mapping<Key,T>
             */
            template    <typename Key, typename T>
            class   SortedMapping : public Mapping<Key, T> {
            private:
                typedef     Mapping<Key, T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedMapping ();

            public:
                nonvirtual  SortedMapping<Key, T>& operator= (const SortedMapping<Key, T>& src);

            protected:
                explicit SortedMapping (const _SharedPtrIRep& rep);
            };


            /**
             *  \brief  Implementation detail for SortedMapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedMapping<T> container API.
             */
            template    <typename Key, typename T>
            class   SortedMapping<Key, T>::_IRep : public Mapping<Key, T>::_IRep {
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Containers_SortedMapping_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedMapping.inl"
