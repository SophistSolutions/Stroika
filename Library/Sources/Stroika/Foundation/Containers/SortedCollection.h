/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Containers/Collection.h"

#ifndef _Stroika_Foundation_Containers_SortedCollection_h_
#define _Stroika_Foundation_Containers_SortedCollection_h_ 1

/**
 *  \file
 *
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Implement using red-black trees (kind of done via STL, but do with explicit RedBlackTree class)
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::ITotallyOrderingComparer;

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
     *  \em Concrete Implementations:
     *      o   @see Concrete::SortedCollection_LinkedList<>
     *      o   @see Concrete::SortedCollection_stdmultiset<>
     *      o   @see Concrete::SortedCollection_SkipList<>
     * 
     *  \em Factory:
     *      @see SortedCollection_Factory<> to see default implementations.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="ReadMe.md#Container Element comparisons">Container Element comparisons</a>:
     *        See about ElementInOrderComparerType, ElementThreeWayComparerType and GetElementThreeWayComparer etc
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Even though the base class Collection<T> provides no intrinsic comparison operators, to be sorted,
     *          does imply a comparison operator, so a SortedCollection<T> fully supports the c++ standard operator<=> strong comparison
     *          feature.
     *      o   Compare sequentially using the associated GetInOrderComparer ()
     */
    template <typename T>
    class [[nodiscard]] SortedCollection : public Collection<T> {
    private:
        using inherited = Collection<T>;

    protected:
        class _IRep;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedCollection;

    public:
        /**
         */
        using ElementInOrderComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (ArgByValueType<T>, ArgByValueType<T>)>>;

    public:
        /**
         */
        using ElementThreeWayComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare, function<strong_ordering (ArgByValueType<T>, ArgByValueType<T>)>>;

    public:
        /**
         *  All constructors either copy their source comparer (copy/move CTOR), or use the default COMPARER for 'T'.
         *
         * \req ITotallyOrderingComparer<COMPARER,T>  - for constructors with inorderComparer parameter
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedCollection ()
            requires (ITotallyOrderingComparer<less<T>, T>);
        template <ITotallyOrderingComparer<T> COMPARER>
        explicit SortedCollection (COMPARER&& inorderComparer);
        SortedCollection (SortedCollection&&) noexcept      = default;
        SortedCollection (const SortedCollection&) noexcept = default;
        SortedCollection (const initializer_list<T>& src)
            requires (ITotallyOrderingComparer<less<T>, T>);
        template <ITotallyOrderingComparer<T> COMPARER>
        SortedCollection (COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        explicit SortedCollection (ITERABLE_OF_ADDABLE&& src)
            requires (ITotallyOrderingComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection<T>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedCollection{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <ITotallyOrderingComparer<T> COMPARER, IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        SortedCollection (COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (ITotallyOrderingComparer<less<T>, T>);
        template <ITotallyOrderingComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedCollection (COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedCollection (shared_ptr<_IRep>&& src) noexcept;
        explicit SortedCollection (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedCollection& operator= (SortedCollection&&) noexcept = default;
        nonvirtual SortedCollection& operator= (const SortedCollection&)     = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual ElementInOrderComparerType GetInOrderComparer () const;

    public:
        /**
         *  Return the function used to compare if two elements returning strong_ordering (sorted properly)
         */
        nonvirtual ElementThreeWayComparerType GetElementThreeWayComparer () const;

    public:
        /**
         * \brief Compares items with the already associated GetInOrderComparer(), and returns true if the item is found.
         */
        nonvirtual bool Contains (ArgByValueType<T> item) const;

    public:
        /**
         * \brief Removes the argument item with the already associated GetInOrderComparer(), and returns true if the item is found.
         *
         *  \note mutates container
         */
        using inherited::Remove;
        nonvirtual void Remove (ArgByValueType<T> item);

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to SortedCollection, and improve that case to clone properties from this rep (such is rep type, ordering, etc).
         */
        template <typename RESULT_CONTAINER = SortedCollection<T>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  \brief subset of this SortedCollection matching filter-function
         * 
         *  Identical to base class code, but for different RESULT_CONTAINER default.
         */
        template <derived_from<Iterable<T>> RESULT_CONTAINER = SortedCollection<T>, predicate<T> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

    public:
        /**
         *  Compare sequentially using the associated GetInOrderComparer ()  
         */
        nonvirtual bool operator== (const SortedCollection& rhs) const;

    public:
        /**
         *  Compare sequentially using the associated GetInOrderComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedCollection& rhs) const;

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
     * 
     *  \note for some scenarios, it might be more efficient to have both a GetElementInOrderComparer and GetElementThreeWayComparer
     *        method, but at the cost of modest code bloat and additional complexity. Callers who really care about this performance
     *        difference can count on other application logic to assure an even better compare function is used to compare usages.
     *        On clang++ on macOS, this was about 50 bytes per class/instantiation. Not much, but for practically zero value added.
     */
    template <typename T>
    class SortedCollection<T>::_IRep : public Collection<T>::_IRep {
    public:
        virtual ElementThreeWayComparerType GetElementThreeWayComparer () const     = 0;
        virtual bool                        Contains (ArgByValueType<T> item) const = 0;
        using Collection<T>::_IRep::Remove;
        virtual void Remove (ArgByValueType<T> item) = 0;
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedCollection_h_ */
