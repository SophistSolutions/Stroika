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
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             */
            template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
            struct   SortedTally_DefaultTraits : Tally_DefaultTraits <T, WELL_ORDER_COMPARER> {
                /**
                 */
                typedef WELL_ORDER_COMPARER WellOrderCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
            };


            /**
             *      A SortedTally is a Tally<T, TRAITS> which remains sorted (iterator).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \req    RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);
             *
             */
            template    <typename T, typename TRAITS = SortedTally_DefaultTraits<T>>
            class   SortedTally : public Tally<T, TRAITS> {
            private:
                typedef     Tally<T, TRAITS> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedTally ();
                SortedTally (const SortedTally<T, TRAITS>& src);
                template <typename CONTAINER_OF_T>
                explicit SortedTally (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit SortedTally (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                nonvirtual  SortedTally<T, TRAITS>& operator= (const SortedTally<T, TRAITS>& rhs);

            protected:
                explicit SortedTally (const _SharedPtrIRep& rep);

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Bag<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::WellOrderCompareFunctionType  WellOrderCompareFunctionType;
            };


            /**
             *  \brief  Implementation detail for SortedTally<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedTally<T, TRAITS> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename T, typename TRAITS>
            class   SortedTally<T, TRAITS>::_IRep : public Tally<T, TRAITS>::_IRep {
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
