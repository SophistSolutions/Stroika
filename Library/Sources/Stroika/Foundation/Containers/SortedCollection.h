/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedCollection_h_
#define _Stroika_Foundation_Containers_SortedCollection_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Collection.h"



/**
 *  \file
 *
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *                  (one bad implemnation that doesnt even do sorting yet, and regtests so bad they dont notice)
 *
 *
 *  TODO:
 *      @todo   Add Equals(), Contains, Remove(T) methods (we have the virtuals in rep already)
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


            /**
             *  Traits to define the well-ordering of elements of the SortedCollection.
             *
             *  Note - that a well-ordering also imputes a notion of equality (not (a<b or b < a)), so we define
             *  that as well.
             *
             */
            template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
            struct   SortedCollection_DefaultTraits {

                /**
                */
                typedef WELL_ORDER_COMPARER EqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass (Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);

                /**
                 */
                typedef WELL_ORDER_COMPARER WellOrderCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
            };


            /**
             *  \brief  A SortedCollection is a Collection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  A SortedCollection is a Collection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  Note that even though you cannot remove elements by value, or check "Contains" etc on Collection - in general, you always
             *  can with a SortedCollection, because the well-ordering required to define a sorted collection also imputes
             *  a notion of equality which is used for Contains etc.
             *
             *  @see Collection<T, TRAITS>
             *  @see SortedMapping<Key,T>
             *  @see SortedSet<T, TRAITS>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = SortedCollection_DefaultTraits<T>>
            class   SortedCollection : public Collection<T> {
            private:
                typedef Collection<T>  inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of SortedCollection<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::WellOrderCompareFunctionType  WellOrderCompareFunctionType;

            public:
                /**
                 */
                SortedCollection ();
                SortedCollection (const SortedCollection<T, TRAITS>& src);
                SortedCollection (const std::initializer_list<T>& src);
                template <typename CONTAINER_OF_T>
                explicit SortedCollection (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedCollection (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedCollection (const _SharedPtrIRep& rep);

            public:
                /**
                 */
                nonvirtual  SortedCollection<T, TRAITS>& operator= (const SortedCollection<T, TRAITS>& rhs);
            };


            /**
             *  \brief  Implementation detail for SortedCollection<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedCollection<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename T, typename TRAITS>
            class   SortedCollection<T, TRAITS>::_IRep : public Collection<T>::_IRep {
            public:
                virtual bool    Equals (const typename Collection<T>::_IRep& rhs) const     = 0;
                virtual bool    Contains (T item) const                                     = 0;
                virtual void    Remove (T item)                                             = 0;
            };


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "SortedCollection.inl"

#endif  /*_Stroika_Foundation_Containers_SortedCollection_h_ */
