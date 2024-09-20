/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Set.h"

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *      @todo   Support Iterable<>::Where overload?
 *      @todo   Could optimize Equals() test for if both sorted, faster way to compare.
 */

namespace Stroika::Foundation::Containers {

    using Common::ITotallyOrderingComparer;

    /**
     *      A SortedSet is a Set<T> which remains sorted (iteration order).
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::SortedSet_SkipList<>
     *      o   @see Concrete::SortedSet_stdset<>
     *
     *  \em Factory:
     *      @see SortedSet_Factory<> to see default implementations.
     * 
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="ReadMe.md#Container Element comparisons">Container Element comparisons</a>:
     *        See about ElementInOrderComparerType, ElementThreeWayComparerType and GetElementThreeWayComparer etc
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Set (base class) are already intrinsically equals-comparable.
     *
     *        o Since SortedSet implies an ordering on the elements of the Set, we can use this to define a
     *          compare_three_way ordering for SortedSet<>
     *
     *        o Since this was never supported before (not a regression) - and close to end of C++17 specific development,
     *          only implementing three way compare for C++20 or later.
     */
    template <typename T>
    class [[nodiscard]] SortedSet : public Set<T> {
    private:
        using inherited = Set<T>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedSet;

    protected:
        class _IRep;

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
         *  All constructors either copy their source comparer (copy/move CTOR), or use the default INORDER comparer for 'T'.
         * 
         *  The (element) COMPARER must be provided (if not explicitly, then implicitly via defaults) at construction time. This
         *  is key to differentiating SortedSet from Set construction (where you specify an IEqualsComparer). Here the IEqualsComparer
         *  is implicitly defined by the supposed ITotallyOrderingComparer.
         *
         * \req ITotallyOrderingComparer<COMPARER,T> - for constructors with that type parameter
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedSet ()
            requires (ITotallyOrderingComparer<less<T>, T>);
        template <ITotallyOrderingComparer<T> COMPARER>
        explicit SortedSet (COMPARER&& comparer);
        SortedSet (SortedSet&& src) noexcept      = default;
        SortedSet (const SortedSet& src) noexcept = default;
        SortedSet (const initializer_list<T>& src)
            requires (ITotallyOrderingComparer<less<T>, T>);
        template <ITotallyOrderingComparer<T> COMPARER>
        SortedSet (COMPARER&& comparer, const initializer_list<T>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        explicit SortedSet (ITERABLE_OF_ADDABLE&& src)
            requires (ITotallyOrderingComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedSet<T>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedSet{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <ITotallyOrderingComparer<T> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedSet (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        SortedSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
            requires (ITotallyOrderingComparer<less<T>, T>);
        template <ITotallyOrderingComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedSet (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedSet (shared_ptr<_IRep>&& src) noexcept;
        explicit SortedSet (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedSet& operator= (SortedSet&& rhs) noexcept = default;
        nonvirtual SortedSet& operator= (const SortedSet& rhs)     = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         * 
         *  \note - this is a function-object wrapper on the underlying comparer, so use will work, but isn't as optimized
         *          as already directly knowing the function object via 'other means'
         */
        nonvirtual ElementInOrderComparerType GetElementInOrderComparer () const;

    public:
        /**
         *  Return the function used to compare if two elements returning strong_ordering (sorted properly)
         * 
         *  \note - this is a function-object wrapper on the underlying comparer, so use will work, but isn't as optimized
         *          as already directly knowing the function object via 'other means'
         */
        nonvirtual ElementThreeWayComparerType GetElementThreeWayComparer () const;

    public:
        /**
         *  Compare sequentially using the associated GetElementThreeWayComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedSet& rhs) const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to SortedSet, and improve that case to clone properties from this rep (such is rep type, ordering, etc).
         */
        template <typename RESULT_CONTAINER = SortedSet<T>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         */
        template <derived_from<Iterable<T>> RESULT_CONTAINER = SortedSet<T>, predicate<T> INCLUDE_PREDICATE>
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
     *  \brief  Implementation detail for SortedSet<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedSet<T> container API.
     * 
     *  \note for some scenarios, it might be more efficient to have both a GetElementInOrderComparer and GetElementThreeWayComparer
     *        method, but at the cost of modest code bloat and additional complexity. Callers who really care about this performance
     *        difference can count on other application logic to assure an even better compare function is used to compare usages.
     *        On clang++ on macOS, this was about 50 bytes per class/instantiation. Not much, but for practically zero value added.
     */
    template <typename T>
    class SortedSet<T>::_IRep : public Set<T>::_IRep {
    public:
        virtual ElementThreeWayComparerType GetElementThreeWayComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedSet.inl"

#endif /*_Stroika_Foundation_Containers_SortedSet_h_ */
