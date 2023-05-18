/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <set>
#include <unordered_set>

#include "../../Cryptography/Digest/HashBase.h"
#include "../KeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdhashset_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdhashset_h_

//// NOT YET IMPELEMTNED - DRAFT ----

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   KeyedCollection_stdhashset<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the KeyedCollection<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \see DeclareInOrderComparer
     * 
     * \note Performance Notes:
     *      @todo
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_stdhashset : public KeyedCollection<T, KEY_TYPE, TRAITS> {
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
         *  An ELEMENT is of type T, but the KEY is of a separate type.
         *  For a keyed collection, we auto-extract the key from the type T, but store the type T.
         *  We internally must count on comparing elements of type KEY_TYPE (after extraction).
         *
         *  This helper allows comparing either KEY or T types interchangeably.
         */
        template <typename KEY_EQUALS_COMPARER = equal_to<key_type>>
        struct ElementEqualsComparer {
            static_assert (not is_reference_v<KEY_EQUALS_COMPARER>);
            constexpr ElementEqualsComparer (const KeyExtractorType& keyExtractor = {}, const KEY_EQUALS_COMPARER& keyEqualsComparer = {})
                : fKeyExtractor_{keyExtractor}
                , fKeyComparer{keyEqualsComparer}
            {
            }
            constexpr int operator() (const value_type& lhs, const KEY_TYPE& rhs) const
            {
                return fKeyComparer (fKeyExtractor_ (lhs), rhs);
            };
            constexpr int operator() (const KEY_TYPE& lhs, const value_type& rhs) const
            {
                return fKeyComparer (lhs, fKeyExtractor_ (rhs));
            };
            constexpr int operator() (const value_type& lhs, const value_type& rhs) const
            {
                return fKeyComparer (fKeyExtractor_ (lhs), fKeyExtractor_ (rhs));
            };
            [[no_unique_address]] const KeyExtractorType    fKeyExtractor_;
            [[no_unique_address]] const KEY_EQUALS_COMPARER fKeyComparer;
            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key
        };

    public:
        /**
         *  An ELEMENT is of type T, but the KEY is of a separate type.
         *  For a keyed collection, we auto-extract the key from the type T, but store the type T.
         *  We internally must count on comparing elements of type KEY_TYPE (after extraction).
         *
         *  This helper allows hashing either KEY or T types interchangeably.
         */
        template <typename KEY_HASHER = std::hash<key_type>>
        struct ElementHash {
            constexpr ElementHash (const KeyExtractorType& keyExtractor = {}, const KEY_HASHER& kh = {})
                : fKeyExtractor_{keyExtractor}
                , fKeyHasher{kh}
            {
            }
            auto operator() (const key_type& k) const noexcept { return fKeyHasher (k); }
            auto operator() (const value_type& v) const noexcept { return fKeyHasher (fKeyExtractor_ (v)); }
            [[no_unique_address]] const KeyExtractorType fKeyExtractor_;
            [[no_unique_address]] const KEY_HASHER       fKeyHasher;

            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key
        };

        // user providers a hasher on KEY_TYPE, but the std::unordered_set holds a T, so it needs a hasher
        // that works on either T or KEY_TYPE. Similarly for the EqualsComparers

    public:
        /**
         *  \brief STDHASHSET is std::unordered_set<> that can be used inside KeyedCollection_stdhashset (need specific traits on unordered set)
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_set
         */
        template <typename KEY_HASH = std::hash<key_type>, typename KEY_EQUALS_COMPARER = std::equal_to<key_type>>
        using STDHASHSET = unordered_set<T, ElementHash<KEY_HASH>, ElementEqualsComparer<KEY_EQUALS_COMPARER>>;

    public:
        template <typename KEY_HASH = std::hash<KEY_TYPE>, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<IEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> and Cryptography::Digest::IsHashFunction<KEY_HASH, KEY_TYPE>>* = nullptr>
        KeyedCollection_stdhashset (const KeyExtractorType& keyExtractor = {}, KEY_HASH&& keyHasher = {},
                                    KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});

        KeyedCollection_stdhashset (KeyedCollection_stdhashset&& src) noexcept      = default;
        KeyedCollection_stdhashset (const KeyedCollection_stdhashset& src) noexcept = default;
#if 0
    public:
        /**
         *  \see docs on KeyedCollection<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_INORDER_COMPARER and IsEqualsComparer is replaced by IsStrictInOrderComparer
         */
        template <typename KEY_INORDER_COMPARER = less<KEY_TYPE>, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdhashset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        KeyedCollection_stdhashset (KeyedCollection_stdhashset&& src) noexcept      = default;
        KeyedCollection_stdhashset (const KeyedCollection_stdhashset& src) noexcept = default;
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdhashset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                              Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdhashset (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                              Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_stdhashset (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE,
                  enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        KeyedCollection_stdhashset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <input_iterator ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t< KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>* = nullptr>
        KeyedCollection_stdhashset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <input_iterator ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t< KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>* = nullptr>
        KeyedCollection_stdhashset (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, input_iterator ITERATOR_OF_ADDABLE,
                  enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () >* = nullptr>
        KeyedCollection_stdhashset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                    ITERATOR_OF_ADDABLE&& end);
#endif

    public:
        /**
         */
        nonvirtual KeyedCollection_stdhashset& operator= (KeyedCollection_stdhashset&& rhs) noexcept = default;
        nonvirtual KeyedCollection_stdhashset& operator= (const KeyedCollection_stdhashset& rhs)     = default;

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
#include "KeyedCollection_stdhashset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_stdhashset_h_ */
