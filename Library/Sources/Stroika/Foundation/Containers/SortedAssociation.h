/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_h_
#define _Stroika_Foundation_Containers_SortedAssociation_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Mapping.h"



/**
 *
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
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
            struct   SortedAssociation_DefaultTraits : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER> {
                /**
                 */
                using   KeyWellOrderCompareFunctionType     =   KEY_WELL_ORDER_COMPARER;

                /**
                 */
                RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
            };


            /**
             *      A SortedAssociation is a Mapping<Key,T> which remains sorted (iterator) by the Key.
             *
             *  Note - this class might have been called "Dictionary".
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  @see    Mapping<Key,T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = SortedAssociation_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
            class   SortedAssociation : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
            private:
                using   inherited   =   Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>;

            protected:
                class   _IRep;
                using   _SortedAssociationSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _SortedAssociationSharedPtrIRep;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>;

            public:
                /**
                *  This constructor creates a concrete sorted mapping object, either empty,
                *  or initialized with any argument values.
                *
                *  The underlying data structure of the Mapping is defined by @see Concrete::Mapping_Factory<>
                */
                SortedAssociation ();
                SortedAssociation (const SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src);
                SortedAssociation (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit SortedAssociation (const CONTAINER_OF_PAIR_KEY_T& src);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit SortedAssociation (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit SortedAssociation (const _SharedPtrIRep& src);
                explicit SortedAssociation (_SharedPtrIRep&& src);

            public:
                /**
                 */
                nonvirtual  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>&  operator= (const SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs);
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS> && rhs) = default;
#endif

            public:
                /**
                 */
                using   TraitsType  =   TRAITS;

            public:
                /**
                 */
                using   KeyWellOrderCompareFunctionType     =   typename TraitsType::KeyWellOrderCompareFunctionType;
            };


            /**
             *  \brief  Implementation detail for SortedAssociation<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedAssociation<T> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class   SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedAssociation.inl"

#endif  /*_Stroika_Foundation_Containers_SortedAssociation_h_ */
