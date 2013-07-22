/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

#include    "../../StroikaPreComp.h"

#include    "../Mapping.h"

//#include    "SortedMapping_stdmap.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 *      @todo   Note in docs why (deadly #include embrance) why this is cloned, and just just .....

                // I THINK best todo would be to have PUBLIC (HEADER) file act like new class, but backend INL file
                // just use SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::REP!!!!
 *
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  Mapping_stdmap requiers its own traits (besides Mapping_DefaultTraits) because of the neeed for a compare function, in std::map<>
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<VALUE_TYPE>>
                struct   Mapping_stdmap_DefaultTraits : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_WELL_ORDER_COMPARER, VALUE_EQUALS_COMPARER> {
                public:
                    /**
                     */
                    typedef KEY_WELL_ORDER_COMPARER KeyWellOrderCompareFunctionType;

                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
                };


                /**
                 *  \brief   Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Mapping_stdmap_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
                class   Mapping_stdmap : public Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> {
                private:
                    typedef     Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>  inherited;

                public:
                    /**
                     */
                    typedef TRAITS      TraitsType;

                public:
                    /**
                     */
                    typedef typename    TraitsType::KeyWellOrderCompareFunctionType KeyWellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);

                public:
                    Mapping_stdmap ();
                    Mapping_stdmap (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs);
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
#include    "Mapping_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
