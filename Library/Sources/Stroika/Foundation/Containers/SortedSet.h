/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_  1

#include    "../StroikaPreComp.h"

#include    "ElementTraits.h"
#include    "Set.h"



/*
 *
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a> -- CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika...
 *
 *
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedSet is a Set<T> which remains sorted (iterator).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \req    RequireElementTraitsInClass(RequireOperatorLess, T);
             *
             */
            template    <typename   T>
            class   SortedSet : public Set<T> {
            public:
                RequireElementTraitsInClass(RequireOperatorLess, T, _SORTEDSET_REQUIRES_OPLESS_);

            private:
                typedef     Set<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedSet ();
                SortedSet (const SortedSet<T>& s);
                template <typename CONTAINER_OF_T>
                explicit SortedSet (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR>
                explicit SortedSet (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

            public:
                nonvirtual  SortedSet<T>& operator= (const SortedSet<T>& rhs);

            protected:
                explicit SortedSet (const _SharedPtrIRep& rep);
            };


            /**
             *  \brief  Implementation detail for SortedSet<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedSet<T> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename   T>
            class   SortedSet<T>::_IRep : public Set<T>::_IRep {
            };


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedSet.inl"

#endif  /*_Stroika_Foundation_Containers_SortedSet_h_ */
