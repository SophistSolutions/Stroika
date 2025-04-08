/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_HashTable_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_HashTable_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/DataStructures/HashTable.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Cryptography/Digest/HashBase.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   KeyedCollection_HashTable<T,KEY_TYPE> is a HashTable based concrete implementation of the KeyedCollection<T,KEY_TYPE> container pattern.
     *
     * \note Runtime performance/complexity:
     *      @todo
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_HashTable : public KeyedCollection<T, KEY_TYPE, TRAITS> {
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
         *  An ELEMENT is of type T, but the KEY is of a separate type.
         *  For a keyed collection, we auto-extract the key from the type T, but store the type T.
         *  We internally must count on comparing elements of type KEY_TYPE (after extraction).
         *
         *  This helper allows comparing either KEY or T types interchangeably.
         * 
         *  Used in STDHASHSET definition, and typically nowhere else
         */
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<key_type>>
        struct ElementEqualsComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
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

    //        static_assert (IEqualsComparer<ElementEqualsComparer, value_type>); // we promise this
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
        template <Cryptography::Digest::IHashFunction<KEY_TYPE> KEY_HASHER = hash<key_type>>
        struct ElementHash {
            constexpr ElementHash (const KeyExtractorType& keyExtractor = {}, const KEY_HASHER& kh = {})
                : fKeyExtractor_{keyExtractor}
                , fKeyHasher{kh}
            {
            }
            auto operator() (const key_type& k) const noexcept
            {
                return fKeyHasher (k);
            }
            auto operator() (const value_type& v) const noexcept
            {
                return fKeyHasher (fKeyExtractor_ (v));
            }
            [[no_unique_address]] const KeyExtractorType fKeyExtractor_;
            [[no_unique_address]] const KEY_HASHER       fKeyHasher;

            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key

//            static_assert (Cryptography::Digest::IHashFunction<ElementHash, value_type>);   // we promise this
        };

    public:
        /**
        * DESIGN CHOICE - could use HashTable<KEY_TYPE,T> or HASH_TABLE<T,void>. The former would be simpler, and the later more compact.
        * For now - try the more compact choice.
         */
        template <Cryptography::Digest::IHashFunction<KEY_TYPE> HASHER = hash<KEY_TYPE>, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  typename LAYOUT_OPTIONS = DataStructures::HashTable_Support::SeparateChainingOptions<T, void>>
        using DefaultTraits =
            DataStructures::HashTable_Support::DefaultTraits<T, void, ElementHash<HASHER>, ElementEqualsComparer<KEY_EQUALS_COMPARER>, LAYOUT_OPTIONS, AddOrExtendOrReplaceMode::eAddReplaces>;

    public:
        /**
         *  \brief HashTable is DataStructures::HashTable<...> that can be used inside Mapping_HashTable
         */
        template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
        using HASHTABLE = DataStructures::HashTable<T, void, HASH_TABLE_TRAITS>;

    public:
        /**
         * Convenient shorthand - not 100% sure why I couldn't just do this with default template arg to HASHTABLE, but didn't compile on gcc/clang?
         */
        template <typename K = KEY_TYPE>
            requires (Cryptography::Digest::IHashFunction<hash<K>, K> and IEqualsComparer<equal_to<K>, K>)
        using DEFAULT_HASHTABLE = DataStructures::HashTable<T, void, DefaultTraits<hash<K>, equal_to<K>>>;

    public:
        /**
         *  @todo consider adding more CTOR overloads... Like with base class KeyedCollection
         */
        KeyedCollection_HashTable ()
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      Cryptography::Digest::IHashFunction<hash<KEY_TYPE>, KEY_TYPE> and IEqualsComparer<std::equal_to<KEY_TYPE>, KEY_TYPE>);
        template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
            requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
        KeyedCollection_HashTable (const KeyExtractorType& keyExtractor, HASHTABLE<HASH_TABLE_TRAITS>&& src);
        template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
            requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
        KeyedCollection_HashTable (HASHTABLE<HASH_TABLE_TRAITS>&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <typename KEY_HASH = hash<KEY_TYPE>, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_HashTable (const KeyExtractorType& keyExtractor = {}, KEY_HASH&& keyHasher = {},
                                   KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{})
            requires (IEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> and Cryptography::Digest::IHashFunction<KEY_HASH, KEY_TYPE>);
        KeyedCollection_HashTable (KeyedCollection_HashTable&& src) noexcept      = default;
        KeyedCollection_HashTable (const KeyedCollection_HashTable& src) noexcept = default;

    public:
        /**
         */
        nonvirtual KeyedCollection_HashTable& operator= (KeyedCollection_HashTable&& rhs) noexcept = default;
        nonvirtual KeyedCollection_HashTable& operator= (const KeyedCollection_HashTable& rhs)     = default;

    private:
        struct IImplRepBase_ {};
        template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
            requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
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
#include "KeyedCollection_HashTable.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_HashTable_h_ */