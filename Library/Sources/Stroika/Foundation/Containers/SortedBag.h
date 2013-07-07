/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedBag_h_
#define _Stroika_Foundation_Containers_SortedBag_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

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
 *      @todo   Use TRAITS mechanism - like with Bag<>
 *
 *      @todo   Fixup constructors (templated by value and iterator ctors)
 *
 *      @todo   Improve test cases, and notice that sorting doesnt actually work for sorted-linked-list.
 *
 *      @todo   Implement using redblback trees.
 *
 *      @todo   See if there is a good STL backend to use to implement this? Multimap<T,void> is the closest
 *              I can see.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            // CALL IT WellOrdered_COMPARER (isntead of ordered or less or compare)

            // redo so comparer is Compare_comparer!!!! and then gen == from it (or maybe its base class?)
            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   SortedBag_DefaultTraits : Bag_DefaultTraits <T, EQUALS_COMPARER> {
                /**
                 */
            };


            /**
             *      A SortedBag is a Bag<T, TRAITS> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  @see Bag<T, TRAITS>
             *  @see SortedMapping<Key,T>
             *  @see SortedSet<T, TRAITS>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = SortedBag_DefaultTraits<T>>
            class   SortedBag : public Bag<T, TRAITS> {
            public:
                RequireElementTraitsInClass(RequireOperatorLess, T);

            private:
                typedef Bag<T, TRAITS>  inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                SortedBag ();
                SortedBag (const SortedBag<T, TRAITS>& sb);
                template <typename CONTAINER_OF_T>
                explicit SortedBag (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedBag (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedBag (const _SharedPtrIRep& rep);

            public:
                /**
                 */
                nonvirtual  SortedBag<T, TRAITS>& operator= (const SortedBag<T, TRAITS>& rhs);
            };


            /**
             *  \brief  Implementation detail for SortedBag<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedBag<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename T, typename TRAITS>
            class   SortedBag<T, TRAITS>::_IRep : public Bag<T, TRAITS>::_IRep {
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
