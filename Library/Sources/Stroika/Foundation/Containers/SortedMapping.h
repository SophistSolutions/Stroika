/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_h_
#define _Stroika_Foundation_Containers_SortedMapping_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Mapping.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Beta">Alpha-Beta</a>
 *
 *
 *  TODO:
 *      @todo   Fix constructors to work better/more flexibly like other containers (copy from iterator etc)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<VALUE_TYPE>, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
            struct   SortedMapping_DefaultTraits : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER> {
                /**
                 */
                typedef KEY_WELL_ORDER_COMPARER KeyWellOrderCompareFunctionType;

                /**
                 */
                RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
            };


            /**
             *      A SortedMapping is a Mapping<Key,T> which remains sorted (iterator) by the Key.
             *
             *  Note - this class might have been called "Dictionary".
             *
             *  @see    Mapping<Key,T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = SortedMapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
            class   SortedMapping : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
            private:
                typedef     Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                *  This constructor creates a concrete sorted mapping object, either empty,
                *  or initialized with any argument values.
                *
                *  The underlying data structure of the Mapping is defined by @see Concrete::Mapping_Factory<>
                */
                SortedMapping ();
                SortedMapping (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& m);
                SortedMapping (const std::initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& m);
                SortedMapping (const std::initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& m);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit SortedMapping (const CONTAINER_OF_PAIR_KEY_T& cp);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit SortedMapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit SortedMapping (const _SharedPtrIRep& rep);

            public:
                nonvirtual  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>&  operator= (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src);

            public:
                /**
                 */
                typedef TRAITS      TraitsType;

            public:
                /**
                 */
                typedef typename    TraitsType::KeyWellOrderCompareFunctionType KeyWellOrderCompareFunctionType;
            };


            /**
             *  \brief  Implementation detail for SortedMapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedMapping<T> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class   SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedMapping.inl"

#endif  /*_Stroika_Foundation_Containers_SortedMapping_h_ */
