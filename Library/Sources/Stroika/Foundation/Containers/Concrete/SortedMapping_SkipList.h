/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_SkipList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/Private/SkipListSupport.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IThreeWayComparer;

    /**
     *  \brief   SortedMapping_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an SkipList-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_SkipList
        : public Private::SkipListBasedContainer<SortedMapping_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>, SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>, true> {
    private:
        using inherited =
            Private::SkipListBasedContainer<SortedMapping_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>, SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>, true>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using KeyInOrderKeyComparerType    = typename inherited::KeyInOrderKeyComparerType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  AddOrExtendOrReplaceMode::eAddReplaces is the most common behavior for Mapping, so default to that in SkipList, and special case
         *  AddOrExtendOrReplaceMode::eAddIfMissing.
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<KEY_TYPE, KEY_COMPARER, AddOrExtendOrReplaceMode::eAddReplaces>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside SortedMapping_SkipList
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<KEY_TYPE, MAPPED_VALUE_TYPE, SKIPLISTTRAITS<KEY_COMPARER>>;

    public:
        /**
         *  \see docs on Mapping<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_THREEWAY_COMPARER and EqualsComparer is replaced by IInOrderComparer
         *       and added SortedMapping_SkipList (SKIPLIST<>&& src)
         */
        SortedMapping_SkipList ()
            requires (three_way_comparable<KEY_TYPE>);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER>
        SortedMapping_SkipList (SKIPLIST<KEY_THREEWAY_COMPARER>&& src);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER>
        explicit SortedMapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer);
        SortedMapping_SkipList (SortedMapping_SkipList&&) noexcept      = default;
        SortedMapping_SkipList (const SortedMapping_SkipList&) noexcept = default;
        SortedMapping_SkipList (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (three_way_comparable<KEY_TYPE>);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER>
        SortedMapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMapping_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit SortedMapping_SkipList (ITERABLE_OF_ADDABLE&& src)
            requires (three_way_comparable<KEY_TYPE>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedMapping_SkipList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        SortedMapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedMapping_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (three_way_comparable<KEY_TYPE>);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedMapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedMapping_SkipList& operator= (SortedMapping_SkipList&&) noexcept = default;
        nonvirtual SortedMapping_SkipList& operator= (const SortedMapping_SkipList&)     = default;

    private:
        using IImplRepBase_ = Private::SkipListBasedContainerIRep<typename SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep>;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IThreeWayComparer<KEY_TYPE>) KEY_THREEWAY_COMPARER>
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
#include "SortedMapping_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMapping_SkipList_h_ */
