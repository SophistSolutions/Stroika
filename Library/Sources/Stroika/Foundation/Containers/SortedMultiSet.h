/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_h_
#define _Stroika_Foundation_Containers_SortedMultiSet_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "MultiSet.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             */
            template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
            struct   SortedMultiSet_DefaultTraits : MultiSet_DefaultTraits <T, WELL_ORDER_COMPARER> {
                /**
                 */
                using   WellOrderCompareFunctionType    =   WELL_ORDER_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
            };


            /**
             *      A SortedMultiSet is a MultiSet<T, TRAITS> which remains sorted (iterator).
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \req    RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);
             *
             */
            template    <typename T, typename TRAITS = SortedMultiSet_DefaultTraits<T>>
            class   SortedMultiSet : public MultiSet<T, typename TRAITS::MultisetTraitsType> {
            private:
                using   inherited   =   MultiSet<T, typename TRAITS::MultisetTraitsType>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   SortedMultiSet<T, TRAITS>;

            protected:
                class   _IRep;

            protected:
                using   _SortedMultiSetSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _SortedMultiSetSharedPtrIRep;

            public:
                SortedMultiSet ();
                SortedMultiSet (const SortedMultiSet<T, TRAITS>& src);
                SortedMultiSet (const initializer_list<T>& src);
                SortedMultiSet (const initializer_list<MultiSetEntry<T>>& src);
                template <typename CONTAINER_OF_T>
                explicit SortedMultiSet (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedMultiSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedMultiSet (const _SharedPtrIRep& src);
                explicit SortedMultiSet (_SharedPtrIRep&& src);

            public:
                /**
                 */
                nonvirtual  SortedMultiSet<T, TRAITS>& operator= (const SortedMultiSet<T, TRAITS>& rhs) = default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  SortedMultiSet<T, TRAITS>& operator= (SortedMultiSet<T, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  SortedMultiSet<T, TRAITS>& operator= (SortedMultiSet<T, TRAITS> && rhs) = default;
#endif

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Bag<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using   TraitsType  =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   WellOrderCompareFunctionType    =   typename TraitsType::WellOrderCompareFunctionType;

            public:
                RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);

            protected:
                nonvirtual  void    _AssertRepValidType () const;
            };


            /**
             *  \brief  Implementation detail for SortedMultiSet<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedMultiSet<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename T, typename TRAITS>
            class   SortedMultiSet<T, TRAITS>::_IRep : public MultiSet<T, typename TRAITS::MultisetTraitsType>::_IRep {
            };


        }


        namespace Execution {


            // early alpha placeholder test
            template    <typename T, typename TRAITS>
            class LEGACY_Synchronized<Containers::SortedMultiSet<T, TRAITS>> {
            public:
                using   ContainerType =     Containers::SortedMultiSet<T, TRAITS>;
                using   ElementType   =     typename ContainerType::ElementType;
            public:
                LEGACY_Synchronized () : fDelegate_ () {}
                LEGACY_Synchronized (const LEGACY_Synchronized& src) : fDelegate_ (src) {}
                LEGACY_Synchronized (LEGACY_Synchronized&& src) : fDelegate_ (move (src)) {}
                LEGACY_Synchronized (const ContainerType& src) : fDelegate_ (src) {}
                LEGACY_Synchronized (ContainerType&& src) : fDelegate_ (move (src)) {}
                LEGACY_Synchronized (const initializer_list<T>& src) : fDelegate_ (src) {}
                template <typename CONTAINER_OF_T>
                explicit LEGACY_Synchronized (const CONTAINER_OF_T& src) : fDelegate_ (src) {}
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit LEGACY_Synchronized (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end) : fDelegate_ (start, end) {}
                const LEGACY_Synchronized& operator= (const LEGACY_Synchronized& rhs)
                {
                    fDelegate_ = rhs.fDelegate_;
                    return *this;
                }
                typename Traversal::Iterator<ElementType> begin () const                            { return fDelegate_.begin (); }
                typename Traversal::Iterator<ElementType> end () const                              { return fDelegate_.end (); }
                operator ContainerType () const                                                     { return fDelegate_;    }
            private:
                Containers::SortedMultiSet<T, TRAITS>   fDelegate_;
                mutex                                   fLock_;
            };


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedMultiSet.inl"

#endif  /*_Stroika_Foundation_Containers_SortedMultiSet_h_ */
