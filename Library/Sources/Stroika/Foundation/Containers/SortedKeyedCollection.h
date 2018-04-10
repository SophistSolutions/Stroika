/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_h_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "KeyedCollection.h"

/**
 *  \file
 *
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *                  (DRAFT/placeholder - no where near functional)
 *
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /**
             *  Traits to define the well-ordering of elements of the SortedKeyedCollection.
             *
             *  Note - that a well-ordering also imputes a notion of equality (not (a<b or b < a)), so we define
             *  that as well.
             *
             */
            template <typename KEY_TYPE, typename T, typename KEY_EQUALS_COMPARER = Common::equal_to<KEY_TYPE>, typename KEY_WELL_ORDER_COMPARER = less<KEY_TYPE>>
            struct SortedKeyedCollection_DefaultTraits : KeyedCollection_DefaultTraits<KEY_TYPE, T, KEY_EQUALS_COMPARER> {
            };

            /**
             *  \brief  A SortedKeyedCollection is a KeyedCollection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  A SortedKeyedCollection is a KeyedCollection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  Note that even though you cannot remove elements by value, or check "Contains" etc on Collection - in general, you always
             *  can with a SortedKeyedCollection, because the well-ordering required to define a sorted collection also imputes
             *  a notion of equality which is used for Contains etc.
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  @see Collection<T, TRAITS>
             *  @see SortedMapping<Key,T>
             *  @see SortedSet<T, TRAITS>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template <typename KEY_TYPE, typename T, typename TRAITS = SortedKeyedCollection_DefaultTraits<KEY_TYPE, T>>
            class SortedKeyedCollection : public KeyedCollection<KEY_TYPE, T> {
            private:
                using inherited = KeyedCollection<KEY_TYPE, T>;

            protected:
                class _IRep;

            protected:
                using _SharedPtrIRep = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedKeyedCollection<KEY_TYPE, T, TRAITS>;
                ,
                    public :
                    /**
                 *  Just a short-hand for the 'TRAITS' part of SortedKeyedCollection<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                    using TraitsType = TRAITS;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using WellOrderCompareFunctionType = typename TraitsType::WellOrderCompareFunctionType;

            public:
                /**
                 */
                SortedKeyedCollection ();
                SortedKeyedCollection (const SortedKeyedCollection& src)  = default;
                SortedKeyedCollection (const SortedKeyedCollection&& src) = default;
                SortedKeyedCollection (const std::initializer_list<T>& src);
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value && !std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedKeyedCollection<T>*>::value>::type>
                explicit SortedKeyedCollection (const CONTAINER_OF_ADDABLE& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                SortedKeyedCollection (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedKeyedCollection (const _SharedPtrIRep& src);
                explicit SortedKeyedCollection (_SharedPtrIRep&& src);

            public:
                /**
                 */
                nonvirtual SortedKeyedCollection<T, TRAITS>& operator= (const SortedKeyedCollection<T, TRAITS>& rhs);
                nonvirtual SortedKeyedCollection<T, TRAITS>& operator= (SortedKeyedCollection<T, TRAITS>&& rhs) = default;

            protected:
                nonvirtual void _AssertRepValidType () const;
            };

            /**
             *  \brief  Implementation detail for SortedKeyedCollection<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedKeyedCollection<T, TRAITS> container API.
             */
            template <typename T, typename TRAITS>
            class SortedKeyedCollection<T, TRAITS>::_IRep : public Collection<T>::_IRep {
            public:
                virtual bool Equals (const typename Collection<T>::_IRep& rhs) const = 0;
                virtual bool Contains (T item) const                                 = 0;
                using Collection<T>::_IRep::Remove;
                virtual void Remove (T item) = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedKeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedKeyedCollection_h_ */
