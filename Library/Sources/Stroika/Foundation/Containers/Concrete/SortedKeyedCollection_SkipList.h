/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_SkipList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/SortedKeyedCollection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    using Common::IThreeWayComparer;

    /**
     *  \brief   SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS> is an SkipList-based concrete implementation of the SortedKeyedCollection<T, KEY_TYPE, TRAITS> container pattern.
     *
     * \note Runtime performance/complexity:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class SortedKeyedCollection_SkipList : public SortedKeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = SortedKeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        using TraitsType                = typename inherited::TraitsType;
        using KeyExtractorType          = typename inherited::KeyExtractorType;
        using KeyEqualityComparerType   = typename inherited::KeyEqualityComparerType;
        using KeyInOrderKeyComparerType = typename inherited::KeyInOrderKeyComparerType;
        using KeyType                   = typename inherited::KeyType;
        using key_type                  = typename inherited::key_type;
        using value_type                = typename inherited::value_type;

    public:
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        struct SKIPLIST_ELT_COMPARER {
            SKIPLIST_ELT_COMPARER (const KeyExtractorType& keyExtractor, KEY_COMPARER&& keyComparer = KEY_COMPARER{})
                : fKeyExtractor{keyExtractor}
                , fKeyComparer{keyComparer}
            {
            }
            auto operator() (const T& l, const T& r)
            {
                return fKeyComparer (fKeyExtractor (l), fKeyExtractor (r));
            }
            KeyExtractorType fKeyExtractor;
            KEY_COMPARER     fKeyComparer;
        };

        /**
         *  AddOrExtendOrReplaceMode::eAddReplaces makes sense cuz KeyedCollection<>::Add () will REPLACE the value.
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<T, KEY_COMPARER, AddOrExtendOrReplaceMode::eAddReplaces>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside SortedMapping_SkipList
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<T, void, SKIPLISTTRAITS<KEY_COMPARER>>;

    public:
        template <IThreeWayComparer<KEY_TYPE> COMPARER = compare_three_way>
        SortedKeyedCollection_SkipList (COMPARER&& keyComparer = COMPARER{})
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        SortedKeyedCollection_SkipList (SortedKeyedCollection_SkipList&&) noexcept      = default;
        SortedKeyedCollection_SkipList (const SortedKeyedCollection_SkipList&) noexcept = default;
        template <IThreeWayComparer<KEY_TYPE> COMPARER = compare_three_way>
        SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor, COMPARER&& keyComparer = COMPARER{});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IThreeWayComparer<KEY_TYPE> COMPARER = equal_to<KEY_TYPE>>
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>>)
        SortedKeyedCollection_SkipList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedKeyedCollection_SkipList{KeyExtractorType{}, COMPARER{}}
        {
            this->AddAll (src);
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IThreeWayComparer<KEY_TYPE> COMPARER = compare_three_way>
        SortedKeyedCollection_SkipList (COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IThreeWayComparer<KEY_TYPE> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor, COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IThreeWayComparer<KEY_TYPE> COMPARER = compare_three_way>
        SortedKeyedCollection_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IThreeWayComparer<KEY_TYPE> COMPARER = compare_three_way>
        SortedKeyedCollection_SkipList (COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IThreeWayComparer<KEY_TYPE> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor, COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                        ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedKeyedCollection_SkipList& operator= (SortedKeyedCollection_SkipList&&) noexcept = default;
        nonvirtual SortedKeyedCollection_SkipList& operator= (const SortedKeyedCollection_SkipList&)     = default;

    private:
        using IImplRepBase_ = typename SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IThreeWayComparer<KEY_TYPE>) COMPARER>
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
#include "SortedKeyedCollection_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_SkipList_h_ */
