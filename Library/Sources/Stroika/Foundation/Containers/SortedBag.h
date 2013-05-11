/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedBag_h_
#define _Stroika_Foundation_Containers_SortedBag_h_  1

#include    "../StroikaPreComp.h"

#include    "Bag.h"



/**
 *  \file
 *
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *                  (one bad implemnation that doesnt even do sorting yet, and regtests so bad they dont notice)
 *
 *
 *  TODO:
 *      @todo   Fixup construcotrs (templated by value and iterator ctors)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedBag is a Bag<T> which remains sorted (iterator) as you add  and remove entries.
             *
             *  @see Bag<T>
             *  @see SortedMapping<Key,T>
             *  @see SortedSet<T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <class T>
            class   SortedBag : public Bag<T> {
            private:
                typedef Bag<T>  inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                SortedBag ();
                SortedBag (const SortedBag<T>& sb);
                explicit SortedBag (const T* start, const T* end);

            protected:
                explicit SortedBag (const _SharedPtrIRep& rep);

            public:
                /**
                 */
                nonvirtual  SortedBag<T>& operator= (const SortedBag<T>& rhs);
            };


            /**
             *  \brief  Implementation detail for SortedBag<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedBag<T> container API.
             */
            template    <typename T>
            class   SortedBag<T>::_IRep : public Bag<T>::_IRep {
            };


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "SortedBag.inl"

#endif  /*_Stroika_Foundation_Containers_SortedBag_h_ */
