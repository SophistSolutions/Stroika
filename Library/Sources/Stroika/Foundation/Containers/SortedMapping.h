/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_h_
#define _Stroika_Foundation_Containers_SortedMapping_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Mapping.h"

/**
 *
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *      @todo   Add MOVE CTOR (low priority)
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             *
             */
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            class SortedMapping : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
            private:
                using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

            protected:
                class _IRep;

            protected:
                using _SortedMappingRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

            public:
                /**
                 *  This CAN be used as the argument to a SortedMapping<> as InOrderComparerType, but
                 *  we allow any template in the SortedSet<> CTOR for an inorderComparer that follows Common::IsStrictInOrderComparer () concept
                 */
                using KeyInOrderKeyComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(KEY_TYPE, KEY_TYPE)>>;

            public:
                /**
                 *  This constructor creates a concrete sorted mapping object, either empty,
                 *  or initialized with any argument values.
                 *
                 *  The underlying data structure of the Mapping is defined by @see Factory::SortedMapping_Factory<>
                 */
                SortedMapping ();
                template <typename KEY_INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Common::IsPotentiallyComparerRelation<KEY_INORDER_COMPARER> ()>>
                explicit SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                SortedMapping (const SortedMapping& src) noexcept = default;
                SortedMapping (SortedMapping&& src) noexcept      = default;
                SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
                SortedMapping (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<(Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::value or Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>::value) and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>*>::value>::type>
                SortedMapping (const CONTAINER_OF_ADDABLE& src);
                template <typename KEY_INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Common::IsPotentiallyComparerRelation<KEY_INORDER_COMPARER> () and (Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::value or Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>::value) and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>*>::value>::type>
                SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, const CONTAINER_OF_ADDABLE& src);
                template <typename COPY_FROM_ITERATOR_KEYVALUE>
                SortedMapping (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

            protected:
                explicit SortedMapping (const _SortedMappingRepSharedPtr& src) noexcept;
                explicit SortedMapping (_SortedMappingRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedMapping& operator= (const SortedMapping& rhs) = default;
                nonvirtual SortedMapping& operator= (SortedMapping&& rhs) = default;

            public:
                /**
                *  Return the function used to compare if two elements are in-order (sorted properly)
                */
                nonvirtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const;

            protected:
                /**
                 */
                template <typename T2>
                using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

            protected:
                /**
                 */
                template <typename T2>
                using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

            protected:
                nonvirtual void _AssertRepValidType () const;
            };

            /**
             *  \brief  Implementation detail for SortedMapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedMapping<T> container API.
             */
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            class SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
            public:
                virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMapping.inl"

#endif /*_Stroika_Foundation_Containers_SortedMapping_h_ */
