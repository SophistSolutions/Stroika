/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_h_
#define _Stroika_Foundation_Containers_SortedMultiSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "DefaultTraits/MultiSet.h"
#include "MultiSet.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 */

namespace Stroika::Foundation::Containers {

    /**
     *      A SortedMultiSet is a MultiSet<T, TRAITS> which remains sorted (iterator).
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
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o MultiSet (base class) are already intrinsically equals-comparable.
     *
     *        o Since SortedMultiSet implies an ordering on the elements of the MultiSet, we can use this to define a
     *          three_way_compare ordering for SortedMultiSet<>
     *
     *        o Since this was never supported before (not a regression) - and close to end of C++17 specific development,
     *          only implementing three way compare for C++20 or later.
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class SortedMultiSet : public MultiSet<T, TRAITS> {
    private:
        using inherited = MultiSet<T, TRAITS>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedMultiSet<T, TRAITS>;

    protected:
        class _IRep;

    public:
        using TraitsType = typename inherited::TraitsType;
        using value_type = typename inherited::value_type;

    public:
        /**
         *  Ordering reletion applied to sort a 'SortedMultiSet'. Returned by GetElementInOrderComparer ();
         */
        using ElementInOrderComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (T, T)>>;

    protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
        using _IRepSharedPtr = conditional_t<Stroika::Foundation::Traversal::kIterableUsesStroikaSharedPtr, Stroika::Foundation::Memory::SharedPtr<_IRep>, shared_ptr<_IRep>>;
#else
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;
#endif

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;

    public:
        /**
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedMultiSet ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        explicit SortedMultiSet (INORDER_COMPARER&& inorderComparer);
        SortedMultiSet (SortedMultiSet&& src) noexcept      = default;
        SortedMultiSet (const SortedMultiSet& src) noexcept = default;
        SortedMultiSet (const initializer_list<T>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src);
        SortedMultiSet (const initializer_list<value_type>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedMultiSet<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedMultiSet (ITERABLE_OF_ADDABLE&& src);
        template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedMultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedMultiSet (_IRepSharedPtr&& src) noexcept;
        explicit SortedMultiSet (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedMultiSet& operator= (SortedMultiSet&& rhs) noexcept = default;
        nonvirtual SortedMultiSet& operator= (const SortedMultiSet& rhs) = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual ElementInOrderComparerType GetElementInOrderComparer () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         *  Compare sequentially using the associated GetElementInOrderComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedMultiSet& rhs) const;
#endif

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
     *  \brief  Implementation detail for SortedMultiSet<T, TRAITS> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedMultiSet<T, TRAITS> container API.
     *
     *  Note that this doesn't add any methods, but still serves the purpose of allowing
     *  testing/validation that the subtype information is correct (it is sorted).
     */
    template <typename T, typename TRAITS>
    class SortedMultiSet<T, TRAITS>::_IRep : public MultiSet<T, TRAITS>::_IRep {
    public:
        virtual ElementInOrderComparerType GetElementInOrderComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMultiSet.inl"

#endif /*_Stroika_Foundation_Containers_SortedMultiSet_h_ */
