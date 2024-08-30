/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_SkipList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/SortedSet.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IThreeWayComparer;
    ;

    /**
     *  \brief   SortedSet_SkipList<T> is an std::set-based concrete implementation of the SortedSet<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      Very good low overhead implementation
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class SortedSet_SkipList : public SortedSet<T> {
    private:
        using inherited = SortedSet<T>;

    public:
        /**
         */
        using ElementEqualityComparerType = typename Set<T>::ElementEqualityComparerType;
        using ElementInOrderComparerType  = typename inherited::ElementInOrderComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  AddOrExtendOrReplaceMode::eAddReplaces is the most common behavior for Mapping, so default to that in SkipList, and special case
         *  AddOrExtendOrReplaceMode::eAddIfMissing.
         */
        template <IThreeWayComparer<T> KEY_COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<T, KEY_COMPARER, AddOrExtendOrReplaceMode::eAddReplaces>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside SortedMapping_SkipList
         */
        template <IThreeWayComparer<T> KEY_COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<T, void, SKIPLISTTRAITS<KEY_COMPARER>>;

    public:
        /**
         *  \see docs on SortedSet<> constructor
         *  \req IThreeWayComparer<COMPARER,T> ()
         */
        SortedSet_SkipList ();
        template <IThreeWayComparer<T> COMPARER>
        explicit SortedSet_SkipList (COMPARER&& comparer);
        SortedSet_SkipList (SortedSet_SkipList&&) noexcept      = default;
        SortedSet_SkipList (const SortedSet_SkipList&) noexcept = default;
        SortedSet_SkipList (const initializer_list<T>& src);
        template <IThreeWayComparer<T> COMPARER>
        SortedSet_SkipList (COMPARER&& comparer, const initializer_list<T>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedSet_SkipList<T>>)
        explicit SortedSet_SkipList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedSet_SkipList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IThreeWayComparer<T> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedSet_SkipList (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedSet_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IThreeWayComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedSet_SkipList (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedSet_SkipList& operator= (SortedSet_SkipList&&) noexcept = default;
        nonvirtual SortedSet_SkipList& operator= (const SortedSet_SkipList&)     = default;

    private:
        using IImplRepBase_ = typename SortedSet<T>::_IRep;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IThreeWayComparer<T>) COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedSet_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedSet_SkipList_h_ */
