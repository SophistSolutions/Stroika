/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_HashTable_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_HashTable_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/DataStructures/HashTable.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Private/HashTableSupport.h"
#include "Stroika/Foundation/Cryptography/Digest/HashBase.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief mostly internal concept validator for hash-table that resides inside KeyedCollection_HashTable
     */
    namespace KeyedCollection_HashTable_Support {
        template <typename TRAITS, typename T, typename KEY_TYPE>
        concept IValidHashTableTraits = DataStructures::HashTable_Support::IValidTraits<TRAITS, T, void> and
                                        TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces;
    }

    /**
     *  \brief   KeyedCollection_HashTable<T,KEY_TYPE> is a HashTable based concrete implementation of the KeyedCollection<T,KEY_TYPE> container pattern.
     *
     * \note Runtime performance/complexity:
     *      o   Add (typical O(1), worst case O(N))
     *      o   Find (typical O(1), worst case O(N))
     * 
     *      key is quality of hash; if you hash well - get constant time access, if you have collisions, get poor (array or linked list like) performance.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_HashTable
        : public Private::HashTableBasedContainer<KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>, KeyedCollection<T, KEY_TYPE, TRAITS>> {
    private:
        using inherited = Private::HashTableBasedContainer<KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>, KeyedCollection<T, KEY_TYPE, TRAITS>>;

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
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_TRY_ANYHOW const KeyExtractorType    fKeyExtractor_;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_TRY_ANYHOW const KEY_EQUALS_COMPARER fKeyComparer;
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
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_TRY_ANYHOW const KeyExtractorType fKeyExtractor_;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_TRY_ANYHOW const KEY_HASHER       fKeyHasher;

            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key
        };

    public:
        /**
         * DESIGN CHOICE - could use HashTable<KEY_TYPE,T> or HASH_TABLE<T,void>. The former would be simpler, and the later more compact.
         * For now - try the more compact choice.
         * 
         *  Note this means that KEY_TYPE in the KeyedCollection_HashTable is VERY different from the KEY_TYPE in
         *  the HashTable (which is 'T' aka value_type from the KeyedCollection_HashTable); which is why we use the HashTable TRAITS AlternateFindType to be KEY_TYPE
         */
        template <Cryptography::Digest::IHashFunction<KEY_TYPE> HASHER = hash<KEY_TYPE>, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  typename LAYOUT_OPTIONS = DataStructures::HashTable_Support::SeparateChainingOptions<T, void>>
        using DefaultTraits =
            DataStructures::HashTable_Support::DefaultTraits<T, void, ElementHash<HASHER>, ElementEqualsComparer<KEY_EQUALS_COMPARER>,
                                                             LAYOUT_OPTIONS, AddOrExtendOrReplaceMode::eAddReplaces, KEY_TYPE>;

    public:
        /**
         *  \brief HashTable is DataStructures::HashTable<...> that can be used inside KeyedCollection_HashTable
         */
        template <KeyedCollection_HashTable_Support::IValidHashTableTraits<T, KEY_TYPE> HASH_TABLE_TRAITS = DefaultTraits<>>
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
        template <KeyedCollection_HashTable_Support::IValidHashTableTraits<T, KEY_TYPE> HASH_TABLE_TRAITS>
        KeyedCollection_HashTable (const KeyExtractorType& keyExtractor, HASHTABLE<HASH_TABLE_TRAITS>&& src);
        template <KeyedCollection_HashTable_Support::IValidHashTableTraits<T, KEY_TYPE> HASH_TABLE_TRAITS>
        KeyedCollection_HashTable (HASHTABLE<HASH_TABLE_TRAITS>&& src)
#if !qCompilerAndStdLib_template_ConstraintDiffersInTemplateRedeclaration_Buggy
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
#endif
        ;
        template <Cryptography::Digest::IHashFunction<KEY_TYPE> KEY_HASH = hash<KEY_TYPE>, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_HashTable (const KeyExtractorType& keyExtractor = {}, KEY_HASH&& keyHasher = {},
                                   KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        KeyedCollection_HashTable (KeyedCollection_HashTable&& src) noexcept      = default;
        KeyedCollection_HashTable (const KeyedCollection_HashTable& src) noexcept = default;

    public:
        /**
         */
        nonvirtual KeyedCollection_HashTable& operator= (KeyedCollection_HashTable&& rhs) noexcept = default;
        nonvirtual KeyedCollection_HashTable& operator= (const KeyedCollection_HashTable& rhs)     = default;

    private:
        using IImplRepBase_ = Containers::Private::HashTableBasedContainerIRep<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep>;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (KeyedCollection_HashTable_Support::IValidHashTableTraits<T, KEY_TYPE>) HASH_TABLE_TRAITS>
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
#include "KeyedCollection_HashTable.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_HashTable_h_ */