/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/DefaultTraits/MultiSet.h"
#include "Stroika/Foundation/Containers/MultiSet.h"

#ifndef _Stroika_Foundation_Containers_SortedMultiSet_h_
#define _Stroika_Foundation_Containers_SortedMultiSet_h_ 1

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::IInOrderComparer;

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
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o MultiSet (base class) are already intrinsically equals-comparable.
     *
     *        o Since SortedMultiSet implies an ordering on the elements of the MultiSet, we can use this to define a
     *          compare_three_way ordering for SortedMultiSet<>
     *
     *        o Since this was never supported before (not a regression) - and close to end of C++17 specific development,
     *          only implementing three way compare for C++20 or later.
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class [[nodiscard]] SortedMultiSet : public MultiSet<T, TRAITS> {
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
        using ElementInOrderComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (T, T)>>;

    public:
        /**
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedMultiSet ()
            requires (Common::IInOrderComparer<less<T>, T>);
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        explicit SortedMultiSet (INORDER_COMPARER&& inorderComparer);
        SortedMultiSet (SortedMultiSet&& src) noexcept      = default;
        SortedMultiSet (const SortedMultiSet& src) noexcept = default;
        SortedMultiSet (const initializer_list<T>& src)
            requires (Common::IInOrderComparer<less<T>, T>);
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src);
        SortedMultiSet (const initializer_list<value_type>& src)
            requires (Common::IInOrderComparer<less<T>, T>);
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src);
        template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        explicit SortedMultiSet (ITERABLE_OF_ADDABLE&& src)
            requires (Common::IInOrderComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMultiSet<T, TRAITS>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedMultiSet{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        SortedMultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (Common::IInOrderComparer<less<T>, T>);
        template <IInputIterator<typename TRAITS::CountedValueType> INORDER_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedMultiSet (shared_ptr<_IRep>&& src) noexcept;
        explicit SortedMultiSet (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedMultiSet& operator= (SortedMultiSet&& rhs) noexcept = default;
        nonvirtual SortedMultiSet& operator= (const SortedMultiSet& rhs)     = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual ElementInOrderComparerType GetElementInOrderComparer () const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to SortedMultiSet, and improve that case to clone properties from this rep (such is rep type, ordering, etc).
         */
        template <typename RESULT_CONTAINER = SortedMultiSet<T, TRAITS>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, typename TRAITS::CountedValueType>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, typename TRAITS::CountedValueType>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  See Iterable<T>::Where - except defaults to MultiSet, and handles cloning rep properties for that special case
         */
        template <derived_from<Iterable<typename TRAITS::CountedValueType>> RESULT_CONTAINER = SortedMultiSet<T, TRAITS>, predicate<typename TRAITS::CountedValueType> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

    public:
        /**
         *  Compare sequentially using the associated GetElementInOrderComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedMultiSet& rhs) const;

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
