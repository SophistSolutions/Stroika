/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_h_
#define _Stroika_Foundation_Containers_SortedAssociation_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Common.h"
#include "DefaultTraits/SortedAssociation.h"
#include "Mapping.h"

/**
 *
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   Fix constructors to work better/more flexibly like other containers (copy from iterator etc)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             *
             */
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = DefaultTraits::SortedAssociation<KEY_TYPE, VALUE_TYPE>>
            class SortedAssociation : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
            private:
                using inherited = Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>;

            protected:
                class _IRep;

            protected:
                using _SharedPtrIRep = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>;

            public:
                /**
                *  This constructor creates a concrete sorted mapping object, either empty,
                *  or initialized with any argument values.
                *
                *  The underlying data structure of the SortedAssociation is defined by @see Concrete::SortedAssociation_Factory<>
                */
                SortedAssociation ();
                SortedAssociation (const SortedAssociation& src) noexcept = default;
                SortedAssociation (SortedAssociation&& src) noexcept      = default;
                SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src);
                SortedAssociation (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src);
                template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value>::type>
                explicit SortedAssociation (const CONTAINER_OF_PAIR_KEY_T& src);
                template <typename COPY_FROM_ITERATOR_KEY_T>
                SortedAssociation (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit SortedAssociation (const _SharedPtrIRep& src) noexcept;
                explicit SortedAssociation (_SharedPtrIRep&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) = default;
                nonvirtual SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>&& rhs) = default;

            public:
                /**
                 */
                using TraitsType = TRAITS;

            public:
                /**
                 */
                using KeyWellOrderCompareFunctionType = typename TraitsType::KeyWellOrderCompareFunctionType;
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
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedAssociation.inl"

#endif /*_Stroika_Foundation_Containers_SortedAssociation_h_ */
