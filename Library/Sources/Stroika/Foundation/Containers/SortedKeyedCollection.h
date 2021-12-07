/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_h_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "KeyedCollection.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    /**
     *  \brief  A SortedKeyedCollection is a KeyedCollection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
     *
     *  A SortedKeyedCollection is a KeyedCollection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
     *
     *  Note that even though you cannot remove elements by value, or check "Contains" etc on Collection - in general, you always
     *  can with a SortedKeyedCollection, because the well-ordering required to define a sorted collection also imputes
     *  a notion of equality which is used for Contains etc.
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  @see Collection<T, TRAITS>
     *  @see SortedMapping<Key,T>
     *  @see SortedSet<T, TRAITS>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
     *
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class SortedKeyedCollection : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    protected:
        class _IRep;

    protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
        using _IRepSharedPtr = conditional_t<Stroika::Foundation::Traversal::kIterableUsesStroikaSharedPtr, Stroika::Foundation::Memory::SharedPtr<_IRep>, shared_ptr<_IRep>>;
#else
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;
#endif

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedKeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        /**
         *  Just a short-hand for the 'TRAITS' part of SortedKeyedCollection<T,TRAITS>. This is often handy to use in
         *  building other templates.
         */
        using TraitsType = TRAITS;

    public:
        /**
         *  Just a short-hand for the KeyInOrderKeyComparerType specified through traits. This is often handy to use in
         *  building other templates.
         */
        using KeyInOrderKeyComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

    public:
        using KeyEqualityComparerType = typename inherited::KeyEqualityComparerType;
        using KeyExtractorType        = typename inherited::KeyExtractorType;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;

    public:
        /**
         *  For the CTOR overload with ITERABLE_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements with the IsAddable_v property.
         *
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  If TRAITS (TraitsType) has a valid default extractor, enable certain constructors.
         *
         * \req IsStrictInOrderComparer<KEY_INORDER_COMPARER> () - for constructors with that type parameter
         *
         *  \par Example Usage
         *      \code
         *      \endcode
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        template <typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        SortedKeyedCollection (SortedKeyedCollection&& src) noexcept      = default;
        SortedKeyedCollection (const SortedKeyedCollection& src) noexcept = default;
        template <typename KEY_EXTRACTOR,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <typename ITERABLE_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedKeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERABLE_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedKeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>* = nullptr>
        SortedKeyedCollection (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>* = nullptr>
        SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::is_iterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    protected:
        explicit SortedKeyedCollection (_IRepSharedPtr&& src) noexcept;
        explicit SortedKeyedCollection (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedKeyedCollection& operator= (const SortedKeyedCollection& rhs) = default;

    public:
        /**
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
     *  \brief  Implementation detail for SortedKeyedCollection<T, TRAITS> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedKeyedCollection<T, KEY_TYPE, TRAITS> container API.
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep : public KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {
    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedKeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedKeyedCollection_h_ */
