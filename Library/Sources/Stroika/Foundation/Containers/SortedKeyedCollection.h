/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_h_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_h_ 1

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::IInOrderComparer;

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
     *  \em Concrete Implementations:
     *      o   @see Concrete::SortedKeyedCollection_SkipList<>
     *      o   @see Concrete::SortedKeyedCollection_stdset<>
     * 
     *  \em Factory:
     *      @see SortedKeyedCollection_Factory<> to see default implementations.
     * 
     *  @see Collection<T, TRAITS>
     *  @see SortedMapping<Key,T>
     *  @see SortedSet<T, TRAITS>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
     *
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class [[nodiscard]] SortedKeyedCollection : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    protected:
        class _IRep;

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
         */
        using KeyInOrderComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

    public:
        /**
         */
        using KeyThreeWayComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare,
                                                  function<strong_ordering (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

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
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  If TRAITS (TraitsType) has a valid default extractor, enable certain constructors.
         *
         * \req IInOrderComparer<KEY_INORDER_COMPARER,KEY_TYPE> - for constructors with that type parameter
         * 
         *  \note sort order specified/determined at construction time, and cannot be reset (without creating a new SortedKeyCollection)
         *
         *  \par Example Usage
         *      \code
         *      \endcode
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{})
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        SortedKeyedCollection (SortedKeyedCollection&&) noexcept      = default;
        SortedKeyedCollection (const SortedKeyedCollection&) noexcept = default;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = equal_to<KEY_TYPE>>
        SortedKeyedCollection (ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedKeyedCollection<T, KEY_TYPE, TRAITS>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedKeyedCollection{}
        {
            this->AddAll (src);
            _AssertRepValidType ();
        }
#endif
        ;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedKeyedCollection (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedKeyedCollection (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                               ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedKeyedCollection (shared_ptr<_IRep>&& src) noexcept;
        explicit SortedKeyedCollection (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedKeyedCollection& operator= (SortedKeyedCollection&&) noexcept = default;
        nonvirtual SortedKeyedCollection& operator= (const SortedKeyedCollection&)     = default;

    public:
        /**
         */
        nonvirtual KeyInOrderComparerType GetInOrderKeyComparer () const;

    public:
        /**
         */
        nonvirtual KeyThreeWayComparerType GetThreeWayKeyComparer () const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to SortedKeyedCollection, and improve that case to clone properties from this rep (such is rep type, ordering, etc).
         */
        template <typename RESULT_CONTAINER = SortedKeyedCollection<T, KEY_TYPE, TRAITS>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  \brief subset of this SortedKeyedCollection matching filter-function
         * 
         *  Identical to base class code, but for differnt RESULT_CONTAINER default.
         */
        template <derived_from<Iterable<T>> RESULT_CONTAINER = SortedKeyedCollection<T, KEY_TYPE, TRAITS>, predicate<T> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

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
        virtual KeyThreeWayComparerType GetThreeWayKeyComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedKeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedKeyedCollection_h_ */
