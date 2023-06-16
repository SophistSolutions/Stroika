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
         * 
         *  Used in STDHASHSET definition, and typically nowhere else
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
         * 
         *  Used in STDHASHSET definition, and typically nowhere else
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

    public:
        /**
         *  \brief STDHASHSET is std::unordered_set<> that can be used inside KeyedCollection_stdhashset (need specific traits on unordered set)
         * 
         * user providers a hasher on KEY_TYPE, but the std::unordered_set holds a T, so it needs a hasher
         * that works on either T or KEY_TYPE. Similarly for the EqualsComparers
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_set
         */
        template <typename KEY_HASH = std::hash<key_type>, typename KEY_EQUALS_COMPARER = std::equal_to<key_type>>
        using STDHASHSET = unordered_set<T, ElementHash<KEY_HASH>, ElementEqualsComparer<KEY_EQUALS_COMPARER>>;

    public:
        /**
         *  @todo consider adding more CTOR overloads... Like with base class KeyedCollection
         */
        template <typename KEY_HASH = std::hash<KEY_TYPE>, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_stdhashset (const KeyExtractorType& keyExtractor = {}, KEY_HASH&& keyHasher = {},
                                    KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{})
            requires (IEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> and Cryptography::Digest::IHashFunction<KEY_HASH, KEY_TYPE>);

        KeyedCollection_stdhashset (KeyedCollection_stdhashset&& src) noexcept      = default;
        KeyedCollection_stdhashset (const KeyedCollection_stdhashset& src) noexcept = default;

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
