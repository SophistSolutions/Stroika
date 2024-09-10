/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_SkipList_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/Private/SkipListSupport.h"
#include "Stroika/Foundation/Containers/SortedMultiSet.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IThreeWayComparer;

    /**
     *
     *
     * \note Runtime performance/complexity:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     * 
     *  \note This implementation doesn't store multiple copies of 'T' objects - it just counts those that compare equal.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class SortedMultiSet_SkipList : public Private::SkipListBasedContainer<SortedMultiSet_SkipList<T, TRAITS>, SortedMultiSet<T, TRAITS>, true> {
    private:
        using inherited = Private::SkipListBasedContainer<SortedMultiSet_SkipList<T, TRAITS>, SortedMultiSet<T, TRAITS>, true>;

    public:
        using TraitsType                  = typename inherited::TraitsType;
        using CounterType                 = typename inherited::CounterType;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using ElementInOrderComparerType  = typename inherited::ElementInOrderComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  AddOrExtendOrReplaceMode::eAddReplaces because.....
         */
        template <IThreeWayComparer<T> COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<T, COMPARER, AddOrExtendOrReplaceMode::eAddReplaces>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside SortedMapping_SkipList
         */
        template <IThreeWayComparer<T> COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<T, CounterType, SKIPLISTTRAITS<COMPARER>>;

    public:
        /**
         */
        SortedMultiSet_SkipList ();
        template <IThreeWayComparer<T> COMPARER>
        explicit SortedMultiSet_SkipList (COMPARER&& comparer);
        SortedMultiSet_SkipList (SortedMultiSet_SkipList&&) noexcept      = default;
        SortedMultiSet_SkipList (const SortedMultiSet_SkipList&) noexcept = default;
        SortedMultiSet_SkipList (const initializer_list<T>& src);
        template <IThreeWayComparer<T> COMPARER>
        SortedMultiSet_SkipList (COMPARER&& comparer, const initializer_list<T>& src);
        SortedMultiSet_SkipList (const initializer_list<value_type>& src);
        template <IThreeWayComparer<T> COMPARER>
        SortedMultiSet_SkipList (COMPARER&& comparer, const initializer_list<value_type>& src);
        template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMultiSet_SkipList<T, TRAITS>>)
        explicit SortedMultiSet_SkipList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedMultiSet_SkipList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IThreeWayComparer<T> COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        SortedMultiSet_SkipList (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        SortedMultiSet_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IThreeWayComparer<T> COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        SortedMultiSet_SkipList (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedMultiSet_SkipList& operator= (SortedMultiSet_SkipList&&) noexcept = default;
        nonvirtual SortedMultiSet_SkipList& operator= (const SortedMultiSet_SkipList&)     = default;

    private:
        using IImplRepBase_ = Private::SkipListBasedContainerIRep<typename SortedMultiSet<T, TRAITS>::_IRep>;
        template <qCompilerAndStdLib_UseConceptOrTypename_BWA (IThreeWayComparer<T>) COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;

    private:
        friend inherited;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMultiSet_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_SkipList_h_*/
