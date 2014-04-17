/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
            class   SortedMultiSet : public MultiSet<T, TRAITS> {
            private:
                using   inherited   =   MultiSet<T, TRAITS>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   SortedMultiSet<T, TRAITS>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                SortedMultiSet ();
                SortedMultiSet (const SortedMultiSet<T, TRAITS>& src);
                SortedMultiSet (const initializer_list<T>& s);
                SortedMultiSet (const initializer_list<MultiSetEntry<T>>& s);
                template <typename CONTAINER_OF_T>
                explicit SortedMultiSet (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedMultiSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                nonvirtual  SortedMultiSet<T, TRAITS>& operator= (const SortedMultiSet<T, TRAITS>& rhs);

            protected:
                explicit SortedMultiSet (const _SharedPtrIRep& rep);

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
            class   SortedMultiSet<T, TRAITS>::_IRep : public MultiSet<T, TRAITS>::_IRep {
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
