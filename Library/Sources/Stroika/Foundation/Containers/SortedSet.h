/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Set.h"



/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a> -- CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika...
 *
 *
 *  TODO:
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Support Iterable<>::Where overload?
 *
 *      @todo   CRITICAL - need version where you can pass in a lambda to compare two things for <, so
 *              easy to construct a sorted set with your own sorter function!!!
 *
 *              This applies equally to other Stroika sorted types
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *  \req    DEFAULT IMPLEMENTATION (with no args) - RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);
             */
            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
            struct   SortedSet_DefaultTraits : Set_DefaultTraits <T, EQUALS_COMPARER> {
                /**
                 */
                using       WellOrderCompareFunctionType        =   WELL_ORDER_COMPARER;

                /**
                */
                RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
            };


            /**
             *      A SortedSet is a Set<T> which remains sorted (iterator).
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename T, typename TRAITS = SortedSet_DefaultTraits<T>>
            class   SortedSet : public Set<T, typename TRAITS::SetTraitsType> {
            private:
                using   inherited   =   Set<T, typename TRAITS::SetTraitsType>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   SortedSet<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of SortedSet<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using   TraitsType                  =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   WellOrderCompareFunctionType    =   typename TraitsType::WellOrderCompareFunctionType;

            protected:
                class   _IRep;

            protected:
                using   _SortedSetSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _SortedSetSharedPtrIRep;

            public:
                SortedSet ();
                SortedSet (const SortedSet<T, TRAITS>& src);
                SortedSet (const initializer_list<T>& src);
                template    < typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const SortedSet<T, TRAITS>*>::value >::type >
                explicit SortedSet (const CONTAINER_OF_T& src);
                template    <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit SortedSet (const _SharedPtrIRep& src);
                explicit SortedSet (_SharedPtrIRep&& src);

            public:
                /**
                 */
                nonvirtual  SortedSet<T, TRAITS>& operator= (const SortedSet<T, TRAITS>& rhs) = default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  SortedSet<T, TRAITS>& operator= (SortedSet<T, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  SortedSet<T, TRAITS>& operator= (SortedSet<T, TRAITS> && rhs) = default;
#endif

            protected:
                nonvirtual  void    _AssertRepValidType () const;
            };


            /**
             *  \brief  Implementation detail for SortedSet<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedSet<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename T, typename TRAITS>
            class   SortedSet<T, TRAITS>::_IRep : public Set<T, typename TRAITS::SetTraitsType>::_IRep {
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
