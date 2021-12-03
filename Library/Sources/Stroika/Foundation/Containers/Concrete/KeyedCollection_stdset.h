/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

    /**
     *  \brief   KeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the KeyedCollection<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note Performance Notes:
     *      o   GetLength () is constant
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_stdset : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        using TraitsType              = typename inherited::TraitsType;
        using KeyExtractorType        = typename inherited::KeyExtractorType;
        using KeyEqualityComparerType = typename inherited::KeyEqualityComparerType;
        using KeyType                 = typename inherited::KeyType;
        using key_type                = typename inherited::key_type;
        using value_type              = typename inherited::value_type;

    public:
        /**
         *  \brief SetInOrderComparer is the COMPARER passed to the STL set. It intrinsically uses the provided extractor and key comparer
         */
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER = less<key_type>>
        struct SetInOrderComparer {
            static_assert (not is_reference_v<KEY_EXTRACTOR>);
            static_assert (not is_reference_v<KEY_INORDER_COMPARER>);
            SetInOrderComparer () = delete;
            SetInOrderComparer (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
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
            [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EXTRACTOR        fKeyExtractor_;
            [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_INORDER_COMPARER fKeyComparer_;
            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key
        };

    public:
        /**
         *  \brief STDSET is std::set<> that can be used inside KeyedCollection_stdset
         */
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER = less<key_type>>
        using STDSET = set<value_type, SetInOrderComparer<KEY_EXTRACTOR, KEY_INORDER_COMPARER>, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        template <typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <typename KEY_EXTRACTOR,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        KeyedCollection_stdset (const KeyedCollection_stdset& src) noexcept = default;
        template <typename CONTAINER_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection_stdset<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdset (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection_stdset<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename CONTAINER_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        KeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> ()>* = nullptr>
        KeyedCollection_stdset (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> ()>* = nullptr>
        KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        KeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual KeyedCollection_stdset& operator= (const KeyedCollection_stdset& rhs) = default;

    protected:
        using _IterableRepSharedPtr        = typename inherited::_IterableRepSharedPtr;
        using _KeyedCollectionRepSharedPtr = typename inherited::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
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
