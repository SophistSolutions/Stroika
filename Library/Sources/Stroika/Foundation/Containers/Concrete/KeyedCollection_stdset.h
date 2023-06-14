/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <set>

#include "../KeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    using Common::IInOrderComparer;

    /**
     *  \brief   KeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the KeyedCollection<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note The constructors require IInOrderComparer comparison operators, not EqualsComparer operators like ordinary KeyedCollection<>
     * 
     * \see DeclareInOrderComparer
     * 
     * \note Performance Notes:
     *      o   size () is constant
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_stdset : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using TraitsType                  = typename inherited::TraitsType;
        using KeyExtractorType            = typename inherited::KeyExtractorType;
        using KeyEqualityComparerType     = typename inherited::KeyEqualityComparerType;
        using KeyType                     = typename inherited::KeyType;
        using key_type                    = typename inherited::key_type;
        using value_type                  = typename inherited::value_type;

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
            int operator() (const value_type& lhs, const KEY_TYPE& rhs) const { return fKeyComparer_ (fKeyExtractor_ (lhs), rhs); };
            int operator() (const KEY_TYPE& lhs, const value_type& rhs) const { return fKeyComparer_ (lhs, fKeyExtractor_ (rhs)); };
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
        /**
         *  \see docs on KeyedCollection<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_INORDER_COMPARER and EqualsComparer is replaced by IInOrderComparer
         */
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        KeyedCollection_stdset (KeyedCollection_stdset&& src) noexcept      = default;
        KeyedCollection_stdset (const KeyedCollection_stdset& src) noexcept = default;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        KeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        template <IIterableOfT<T> ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<KeyedCollection_stdset<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        KeyedCollection_stdset (ITERABLE_OF_ADDABLE&& src);
#else
        template <IIterableOfT<T> ITERABLE_OF_ADDABLE>
        KeyedCollection_stdset (ITERABLE_OF_ADDABLE&& src)
            requires (not is_base_of_v<KeyedCollection_stdset<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>);
#endif
        template <IIterableOfT<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOfT<T> ITERABLE_OF_ADDABLE>
        KeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIteratorOfT<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        KeyedCollection_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInputIteratorOfT<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIteratorOfT<T> ITERATOR_OF_ADDABLE>
        KeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual KeyedCollection_stdset& operator= (KeyedCollection_stdset&& rhs) noexcept = default;
        nonvirtual KeyedCollection_stdset& operator= (const KeyedCollection_stdset& rhs)     = default;

    private:
        class IImplRepBase_;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
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
#include "KeyedCollection_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_h_ */
