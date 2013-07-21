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
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<VALUE_TYPE>>
            struct   SortedMapping_DefaultTraits : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_WELL_ORDER_COMPARER, VALUE_EQUALS_COMPARER> {
            public:
                /**
                 */
                typedef KEY_WELL_ORDER_COMPARER KeyWellOrderCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
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
            class   SortedMapping : public Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> {
            private:
                typedef     Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedMapping ();

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

            protected:
                explicit SortedMapping (const _SharedPtrIRep& rep);
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
            class   SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep : public Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep {
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
