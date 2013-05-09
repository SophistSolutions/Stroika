/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_  1

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

#include    "Set.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedSet is a Mapping<Key,T> which remains sorted (iterator) by the Key.
             *
             *  Note - this class might have been called "Dictionary".
             *
             *  @see    Mapping<Key,T>
             */
            template    <typename   T>
            class   SortedSet : public Set<T> {
            private:
                typedef     Set<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedSet ();

            public:
                nonvirtual  SortedSet<T>& operator= (const SortedSet<T>& src);

            protected:
                explicit SortedSet (const _SharedPtrIRep& rep);
            };


            /**
             *  \brief  Implementation detail for SortedSet<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedSet<T> container API.
             */
            template    <typename   T>
            class   SortedSet<T>::_IRep : public Set<T>::_IRep {
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Containers_SortedSet_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedSet.inl"
