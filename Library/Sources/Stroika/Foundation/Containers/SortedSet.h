/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
            struct   SortedSet_DefaultTraits : Set_DefaultTraits <T, WELL_ORDER_COMPARER> {
                /**
                 */
                typedef WELL_ORDER_COMPARER WellOrderCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
            };


            /**
             *      A SortedSet is a Set<T> which remains sorted (iterator).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \req    RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);
             *
             */
            template    <typename T, typename TRAITS = SortedSet_DefaultTraits<T>>
            class   SortedSet : public Set<T, TRAITS> {
            private:
                typedef     Set<T, TRAITS> inherited;

            public:
                /**
                 */
                typedef TRAITS  TraitsType;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedSet ();
                SortedSet (const SortedSet<T, TRAITS>& s);
                template <typename CONTAINER_OF_T>
                explicit SortedSet (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                nonvirtual  SortedSet<T, TRAITS>& operator= (const SortedSet<T, TRAITS>& rhs);

            protected:
                explicit SortedSet (const _SharedPtrIRep& rep);
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
            class   SortedSet<T, TRAITS>::_IRep : public Set<T, TRAITS>::_IRep {
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
