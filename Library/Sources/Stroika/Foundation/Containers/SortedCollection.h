/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedCollection_h_
#define _Stroika_Foundation_Containers_SortedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Collection.h"

/**
 *  \file
 *
 *
 *
 *
 *  TODO:
 *      @todo   Support Iterable<>::Where overload?
 *
 *      @todo   Add Equals(), Contains, Remove(T) methods (we have the virtuals in rep already)
 *
 *      @todo   Improve test cases, and notice that sorting doesn't actually work for sorted-linked-list.
 *
 *      @todo   Implement using redblback trees.
 *
 *      @todo   See if there is a good STL backend to use to implement this? std::multimap<T,void> is the closest
 *              I can see.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /**
             *  \brief  A SortedCollection is a Collection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  A SortedCollection is a Collection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
             *
             *  Note that even though you cannot remove elements by value, or check "Contains" etc on Collection - in general, you always
             *  can with a SortedCollection, because the well-ordering required to define a sorted collection also imputes
             *  a notion of equality which is used for Contains etc.
             *
             *  \note   \em Iterators
             *      Note that iterators always run in sorted order, from least
             *      to largest. Items inserted before the current iterator index will not
             *      be encountered, and items inserted after the current index will be encountered.
             *      Items inserted at the current index remain undefined if they will
             *      be encountered or not.
             *
             *  @see Collection<T>
             *  @see SortedMapping<Key,T>
             *  @see SortedSet<T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             *
             */
            template <typename T>
            class SortedCollection : public Collection<T> {
            private:
                using inherited = Collection<T>;

            protected:
                class _IRep;

            protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
                using _SortedCollectionRepSharedPtr = Memory::SharedPtr<_IRep>;
#else
                using _SortedCollectionRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;
#endif

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = SortedCollection<T>;

            public:
                /**
                 *  This CAN be used as the argument to a SortedSet<> as InOrderComparerType, but
                 *  we allow any template in the SortedSet<> CTOR for an inorderComparer that follows Common::IsStrictInOrderComparer () concept (need better name).
                 */
                using InOrderComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(T, T)>>;

            public:
                /**
                 *  All constructors either copy their source comparer (copy/move CTOR), or use the default INORDER_COMPARER for 'T'.
                 *
                 * \req IsStrictInOrderComparer<INORDER_COMPARER> () - for constructors with that type parameter
                 */
                SortedCollection ();
                template <typename INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>>
                explicit SortedCollection (INORDER_COMPARER&& inorderComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                SortedCollection (const SortedCollection& src) noexcept = default;
                SortedCollection (SortedCollection&& src) noexcept      = default;
                SortedCollection (const std::initializer_list<T>& src);
                template <typename INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>>
                SortedCollection (INORDER_COMPARER&& inOrderComparer, const std::initializer_list<T>& src);
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedCollection<T>*>::value>::type>
                SortedCollection (const CONTAINER_OF_ADDABLE& src);
                template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedCollection<T>*>::value>::type>
                SortedCollection (INORDER_COMPARER&& inOrderComparer, const CONTAINER_OF_ADDABLE& src);
                template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
                SortedCollection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
                template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF = enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>>
                SortedCollection (INORDER_COMPARER&& inOrderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

            protected:
                explicit SortedCollection (const _SortedCollectionRepSharedPtr& src) noexcept;
                explicit SortedCollection (_SortedCollectionRepSharedPtr&& src) noexcept;

            public:
                /**
                 */
                nonvirtual SortedCollection& operator= (const SortedCollection& rhs) = default;
                nonvirtual SortedCollection& operator= (SortedCollection&& rhs) = default;

            public:
                /**
                 *  Return the function used to compare if two elements are in-order (sorted properly)
                 */
                nonvirtual InOrderComparerType GetInOrderComparer () const;

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
             *  \brief  Implementation detail for SortedCollection<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedCollection<T> container API.
             */
            template <typename T>
            class SortedCollection<T>::_IRep : public Collection<T>::_IRep {
            public:
                virtual InOrderComparerType GetInOrderComparer () const                             = 0;
                virtual bool                Equals (const typename Collection<T>::_IRep& rhs) const = 0;
                virtual bool                Contains (T item) const                                 = 0;
                using Collection<T>::_IRep::Remove;
                virtual void Remove (T item) = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedCollection_h_ */
