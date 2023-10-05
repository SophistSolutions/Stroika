/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Set.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *      @todo   Support Iterable<>::Where overload?
 */

namespace Stroika::Foundation::Containers {

    using Common::IInOrderComparer;

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
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
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
        using ArchetypeContainerType = SortedSet<T>;

    protected:
        class _IRep;

    protected:
        using _IRepSharedPtr = shared_ptr<_IRep>;

    public:
        /**
         *  Ordering reletion applied to sort a 'SortedSet'. Returned by GetInOrderComparer ();
         */
        using ElementInOrderComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (T, T)>>;

    public:
        /**
         *  All constructors either copy their source comparer (copy/move CTOR), or use the default INORDER comparer for 'T'.
         * 
         *  The INORDER_COMPARER must be provided (if not explicitly, then implicitly via defaults) at construction time. This
         *  is key to differentiating SortedSet from Set construction (where you specify an IEqualsComparer). Here the IEqualsComparer
         *  is implicitly defined by the supposed IInOrderComparer.
         *
         * \req IInOrderComparer<INORDER_COMPARER,T> - for constructors with that type parameter
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedSet ();
        template <IInOrderComparer<T> INORDER_COMPARER>
        explicit SortedSet (INORDER_COMPARER&& inorderComparer);
        SortedSet (SortedSet&& src) noexcept      = default;
        SortedSet (const SortedSet& src) noexcept = default;
        SortedSet (const initializer_list<T>& src);
        template <IInOrderComparer<T> INORDER_COMPARER>
        SortedSet (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        explicit SortedSet (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedSet<T>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedSet{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedSet (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedSet (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedSet (_IRepSharedPtr&& src) noexcept;
        explicit SortedSet (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedSet& operator= (SortedSet&& rhs) noexcept = default;
        nonvirtual SortedSet& operator= (const SortedSet& rhs)     = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual ElementInOrderComparerType GetInOrderComparer () const;

    public:
        /**
         *  Compare sequentially using the associated GetInOrderComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedSet& rhs) const;

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
     */
    template <typename T>
    class SortedSet<T>::_IRep : public Set<T>::_IRep {
    public:
        virtual ElementInOrderComparerType GetInOrderComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedSet.inl"

#endif /*_Stroika_Foundation_Containers_SortedSet_h_ */
