/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 *      @todo   Support EachWith?
 *
 *      @todo   Add Equals(), Contains, Remove(T) methods (we have the virtuals in rep already)
 *
 *      @todo   Fixup constructors (templated by value and iterator ctors)
 *
 *      @todo   Improve test cases, and notice that sorting doesnt actually work for sorted-linked-list.
 *
 *      @todo   Implement using redblback trees.
 *
 *      @todo   See if there is a good STL backend to use to implement this? std::multimap<T,void> is the closest
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
                using   EqualsCompareFunctionType       =   WELL_ORDER_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass (Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);

                /**
                 */
                using   WellOrderCompareFunctionType    =   WELL_ORDER_COMPARER;

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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = SortedCollection_DefaultTraits<T>>
            class   SortedCollection : public Collection<T> {
            private:
                using   inherited   =   Collection<T>;

            protected:
                class   _IRep;

            protected:
                using   _SortedCollectionSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _SortedCollectionSharedPtrIRep;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   SortedCollection<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of SortedCollection<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using   TraitsType              =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   WellOrderCompareFunctionType    =   typename TraitsType::WellOrderCompareFunctionType;

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
                explicit SortedCollection (const _SharedPtrIRep& src);
                explicit SortedCollection (_SharedPtrIRep&& src);

            public:
                /**
                 */
                nonvirtual  SortedCollection<T, TRAITS>& operator= (const SortedCollection<T, TRAITS>& rhs);
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  SortedCollection<T, TRAITS>& operator= (SortedCollection<T, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  SortedCollection<T, TRAITS>& operator= (SortedCollection<T, TRAITS> && rhs) = default;
#endif

            protected:
                nonvirtual  void    _AssertRepValidType () const;
            };


            /**
             *  \brief  Implementation detail for SortedCollection<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedCollection<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   SortedCollection<T, TRAITS>::_IRep : public Collection<T>::_IRep {
            public:
                virtual bool    Equals (const typename Collection<T>::_IRep& rhs) const     = 0;
                virtual bool    Contains (T item) const                                     = 0;
                using  Collection<T>::_IRep::Remove;
                virtual void    Remove (T item)                                             = 0;
            };


        }


        namespace Execution {


            // early alpha placeholder test
            template    <typename T>
            class Synchronized<Containers::SortedCollection<T>> {
            public:
                using   ContainerType =     Containers::SortedCollection<T>;
                using   ElementType   =     typename ContainerType::ElementType;
            public:
                Synchronized () : fDelegate_ () {}
                Synchronized (const Synchronized& src) : fDelegate_ (src) {}
                Synchronized (Synchronized&& src) : fDelegate_ (move (src)) {}
                Synchronized (const ContainerType& src) : fDelegate_ (src) {}
                Synchronized (ContainerType&& src) : fDelegate_ (move (src)) {}
                Synchronized (const initializer_list<T>& src) : fDelegate_ (src) {}
                template <typename CONTAINER_OF_T>
                explicit Synchronized (const CONTAINER_OF_T& src) : fDelegate_ (src) {}
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Synchronized (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end) : fDelegate_ (start, end) {}
                const Synchronized& operator= (const Synchronized& rhs)
                {
                    fDelegate_ = rhs.fDelegate_;
                    return *this;
                }
                typename Traversal::Iterator<ElementType> begin () const                            { return fDelegate_.begin (); }
                typename Traversal::Iterator<ElementType> end () const                              { return fDelegate_.end (); }
                operator ContainerType () const                                                     { return fDelegate_;    }
            private:
                Containers::SortedCollection<T> fDelegate_;
                mutex                           fLock_;
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
