/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_h_
#define _Stroika_Foundation_Containers_SortedMapping_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Mapping.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
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
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
            struct   SortedMapping_DefaultTraits : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> {
                /**
                 */
                using   KeyWellOrderCompareFunctionType     =   KEY_WELL_ORDER_COMPARER;

                /**
                 */
                RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
            };


            /**
             *      A SortedMapping is a Mapping<Key,T> which remains sorted (iterator) by the Key.
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
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = SortedMapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
            class   SortedMapping : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
            private:
                using   inherited   =   Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>;

            protected:
                class   _IRep;

            protected:
                using   _SortedMappingSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _SortedMappingSharedPtrIRep;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>;

            public:
                /**
                *  This constructor creates a concrete sorted mapping object, either empty,
                *  or initialized with any argument values.
                *
                *  The underlying data structure of the Mapping is defined by @see Concrete::Mapping_Factory<>
                */
                SortedMapping ();
                SortedMapping (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src);
                SortedMapping (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit SortedMapping (const CONTAINER_OF_PAIR_KEY_T& src);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit SortedMapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit SortedMapping (const _SharedPtrIRep& src);
                explicit SortedMapping (_SharedPtrIRep&& src);

            public:
                /**
                 */
                nonvirtual  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>&  operator= (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs);
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> && rhs) = default;
#endif

            public:
                /**
                 */
                using   TraitsType  =   TRAITS;

            public:
                /**
                 */
                using   KeyWellOrderCompareFunctionType     =   typename TraitsType::KeyWellOrderCompareFunctionType;

            protected:
                nonvirtual  void    _AssertRepValidType () const;
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


        namespace Execution {


            // early alpha placeholder test
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class LEGACY_Synchronized<Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>> {
            public:
                using   ContainerType =     Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>;
                using   ElementType   =     typename ContainerType::ElementType;
            public:
                LEGACY_Synchronized () : fDelegate_ () {}
                LEGACY_Synchronized (const LEGACY_Synchronized& src) : fDelegate_ (src) {}
                LEGACY_Synchronized (LEGACY_Synchronized&& src) : fDelegate_ (move (src)) {}
                LEGACY_Synchronized (const ContainerType& src) : fDelegate_ (src) {}
                LEGACY_Synchronized (ContainerType&& src) : fDelegate_ (move (src)) {}
                LEGACY_Synchronized (const initializer_list<Common::KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src) : fDelegate_ (src) {}
                LEGACY_Synchronized (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src) : fDelegate_ (src) {}
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
                Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> fDelegate_;
                mutex                                                   fLock_;
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
