/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include <set>

#include "Stroika/Foundation/Containers/SortedKeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_h_

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   SortedKeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedKeyedCollection<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note Runtime performance/complexity:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class SortedKeyedCollection_stdset : public SortedKeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = SortedKeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        using TraitsType              = typename inherited::TraitsType;
        using KeyExtractorType        = typename inherited::KeyExtractorType;
        using KeyEqualityComparerType = typename inherited::KeyEqualityComparerType;
        using KeyThreeWayComparerType = typename inherited::KeyThreeWayComparerType;
        using KeyType                 = typename inherited::KeyType;
        using key_type                = typename inherited::key_type;
        using value_type              = typename inherited::value_type;

    public:
        /**
         *  \brief SetInOrderComparer is the COMPARER passed to the STL set. It intrinsically uses the provided extractor and key comparer
         */
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<key_type>>
        struct SetInOrderComparer {
            static_assert (not is_reference_v<KEY_INORDER_COMPARER>);
            SetInOrderComparer () = delete;
            SetInOrderComparer (const KeyExtractorType& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
                : fKeyExtractor_{keyExtractor}
                , fKeyComparer_{inorderComparer}
            {
            }
            int operator() (const value_type& lhs, const KEY_TYPE& rhs) const
            {
                return fKeyComparer_ (fKeyExtractor_ (lhs), rhs);
            };
            int operator() (const KEY_TYPE& lhs, const value_type& rhs) const
            {
                return fKeyComparer_ (lhs, fKeyExtractor_ (rhs));
            };
            int operator() (const value_type& lhs, const value_type& rhs) const
            {
                return fKeyComparer_ (fKeyExtractor_ (lhs), fKeyExtractor_ (rhs));
            };
            [[no_unique_address]] const KeyExtractorType     fKeyExtractor_;
            [[no_unique_address]] const KEY_INORDER_COMPARER fKeyComparer_;
            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key
        };

    public:
        /**
         *  \brief STDSET is std::set<> that can be used inside KeyedCollection_stdset
         */
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<key_type>>
        using STDSET =
            set<value_type, SetInOrderComparer<KEY_INORDER_COMPARER>, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{})
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        SortedKeyedCollection_stdset (SortedKeyedCollection_stdset&&) noexcept      = default;
        SortedKeyedCollection_stdset (const SortedKeyedCollection_stdset&) noexcept = default;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = equal_to<KEY_TYPE>>
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>>)
        SortedKeyedCollection_stdset (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedKeyedCollection_stdset{KeyExtractorType{}, KEY_INORDER_COMPARER{}}
        {
            this->AddAll (src);
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedKeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedKeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                      ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedKeyedCollection_stdset& operator= (SortedKeyedCollection_stdset&&) noexcept = default;
        nonvirtual SortedKeyedCollection_stdset& operator= (const SortedKeyedCollection_stdset&)     = default;

    private:
        using IImplRepBase_ = typename SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IInOrderComparer<KEY_TYPE>) KEY_INORDER_COMPARER>
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
#include "SortedKeyedCollection_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_h_ */
