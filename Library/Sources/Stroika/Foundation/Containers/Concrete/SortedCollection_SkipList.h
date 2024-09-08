/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_SkipList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/Private/SkipListSupport.h"
#include "Stroika/Foundation/Containers/SortedCollection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IThreeWayComparer;

    /**
     *  \brief SortedCollection_SkipList<T> is an SkipList-based concrete implementation of the SortedCollection<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      SortedCollection_SkipList<T> is ....
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class SortedCollection_SkipList : public Private::SkipListBasedContainer<SortedCollection_SkipList<T>, SortedCollection<T>, true> {
    private:
        using inherited = Private::SkipListBasedContainer<SortedCollection_SkipList<T>, SortedCollection<T>, true>;

    public:
        using value_type          = typename inherited::value_type;
        using InOrderComparerType = typename SortedCollection<T>::InOrderComparerType;

    public:
        /**
         *  AddOrExtendOrReplaceMode::eAddReplaces is the most common behavior for Mapping, so default to that in SkipList, and special case
         *  AddOrExtendOrReplaceMode::eAddIfMissing.
         */
        template <IThreeWayComparer<T> COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<T, COMPARER, AddOrExtendOrReplaceMode::eAddExtras>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside SortedMapping_SkipList
         */
        template <IThreeWayComparer<T> COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<T, void, SKIPLISTTRAITS<COMPARER>>;

    public:
        /**
         *  \see docs on SortedCollection<> constructor
         */
        SortedCollection_SkipList ();
        template <IThreeWayComparer<T> COMPARER>
        explicit SortedCollection_SkipList (COMPARER&& comparer);
        SortedCollection_SkipList (SortedCollection_SkipList&&) noexcept      = default;
        SortedCollection_SkipList (const SortedCollection_SkipList&) noexcept = default;
        SortedCollection_SkipList (const initializer_list<T>& src);
        template <IThreeWayComparer<T> COMPARER>
        SortedCollection_SkipList (COMPARER&& comparer, const initializer_list<T>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection_SkipList<T>>)
        explicit SortedCollection_SkipList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedCollection_SkipList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IThreeWayComparer<T> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedCollection_SkipList (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedCollection_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IThreeWayComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedCollection_SkipList (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedCollection_SkipList& operator= (SortedCollection_SkipList&&) noexcept = default;
        nonvirtual SortedCollection_SkipList& operator= (const SortedCollection_SkipList&)     = default;

    private:
        using IImplRepBase_ = Private::SkipListBasedContainerIRep<typename SortedCollection<T>::_IRep>;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (Common::IThreeWayComparer<T>) COMPARER>
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
#include "SortedCollection_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_SkipList_h_ */
