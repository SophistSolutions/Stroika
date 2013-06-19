/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedTally_h_
#define _Stroika_Foundation_Containers_SortedTally_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Tally.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Use TRAITS mechanism - like with Bag<>
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedTally is a Tally<T> which remains sorted (iterator).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \req    RequireElementTraitsInClass(RequireOperatorLess, T);
             *
             */
            template    <typename   T>
            class   SortedTally : public Tally<T> {
            public:
                RequireElementTraitsInClass(RequireOperatorLess, T);

            private:
                typedef     Tally<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedTally ();
                SortedTally (const SortedTally<T>& st);
                template <typename CONTAINER_OF_T>
                explicit SortedTally (const CONTAINER_OF_T& st);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedTally (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                nonvirtual  SortedTally<T>& operator= (const SortedTally<T>& rhs);

            protected:
                explicit SortedTally (const _SharedPtrIRep& rep);
            };


            /**
             *  \brief  Implementation detail for SortedTally<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedTally<T> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename   T>
            class   SortedTally<T>::_IRep : public Tally<T>::_IRep {
            };


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedTally.inl"

#endif  /*_Stroika_Foundation_Containers_SortedTally_h_ */
