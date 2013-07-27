/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_h_

#include    "../../StroikaPreComp.h"

#include    "../Set.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  Set_stdset requires its own traits (besides Set_DefaultTraits) because of the neeed for a compare function for std::set<>
                 */
                template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct   Set_stdset_DefaultTraits : Set_DefaultTraits <T, WELL_ORDER_COMPARER> {
                    /**
                     */
                    typedef WELL_ORDER_COMPARER WellOrderCompareFunctionType;

                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
                };


                /**
                 *  \brief   Set_stdset<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::set-based concrete implementation of the Set<T, TRAITS> container pattern.
                 *
                 *  \note   \em Implemenation   Just indirect to SortedSet_stdset<>, which in turn mostly indirects to std::set<T>
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Set_stdset_DefaultTraits<T>>
                class   Set_stdset : public Set<T, TRAITS> {
                private:
                    typedef     Set<T, TRAITS>  inherited;

                public:
                    /**
                     */
                    typedef TRAITS      TraitsType;

                public:
                    /**
                     */
                    typedef typename    TraitsType::WellOrderCompareFunctionType WellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);

                public:
                    /**
                     */
                    Set_stdset ();
                    Set_stdset (const Set_stdset<T, TRAITS>& src);
                    template    <typename CONTAINER_OF_T>
                    explicit Set_stdset (const CONTAINER_OF_T& src);
                    template    <typename COPY_FROM_ITERATOR_T>
                    explicit Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);

                public:
                    /**
                     */
                    nonvirtual  Set_stdset<T, TRAITS>& operator= (const Set_stdset<T, TRAITS>& rhs);
                };


            }
        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Set_stdset.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Set_stdset_h_ */
