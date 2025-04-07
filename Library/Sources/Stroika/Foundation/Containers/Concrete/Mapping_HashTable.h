/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_HashTable_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_HashTable_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <unordered_map>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/DataStructures/HashTable.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Cryptography/Digest/HashBase.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     *  @aliases Could have been called Mapping_stdunorderedmap - but that name would not be nearly as suggestive.
     *           the name std::unordered_map is something of an move towards the approach taken by Stroika - focusing
     *           on data access patterns, rather than implementation data structure. But the API - truly has hash-table
     *           written all over it (so the name is really misleading in std).
     * 
     *  \note   \em Implementation Details
     *          This module is essentially identical to SortedMapping_HashTable, but making it dependent on SortedMapping<> creates
     *          problems with circular dependencies - especially give how the default Mapping CTOR calls the factory class
     *          which maps back to the _HashTable<> variant.
     *
     *          There maybe another (better) way, but this works.
     *
     * \note Runtime performance/complexity:
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_HashTable : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief HashTable is std::map<> that can be used inside Mapping_HashTable
         * 
         *      @todo - STK appraoch to blockallocation not working for unordred map cuz allocates many at a time
         */
        template <typename HASH = std::hash<key_type>, typename KEY_EQUALS_COMPARER = std::equal_to<key_type>>
        using STDHASHMAP = unordered_map<KEY_TYPE, MAPPED_VALUE_TYPE, HASH, KEY_EQUALS_COMPARER>;

        template <DataStructures::HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_VALUE_TYPE> HASH_TABLE_TRAITS =
                      DataStructures::HashTable_Support::DefaultTraits<KEY_TYPE, MAPPED_VALUE_TYPE>>
        using HASHTABLE = DataStructures::HashTable<KEY_TYPE, MAPPED_VALUE_TYPE, HASH_TABLE_TRAITS>;

    public:
        /**
         *   @todo UPDATE THESE DOCS - REVIEW AND COMPARE - BUT THIS IS LITERALLY QUTIE WRONG
         *  \see docs on Mapping<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_INORDER_COMPARER and EqualsComparer is replaced by IInOrderComparer
         *       and added Mapping_HashTable (STDHASHMAP<>&& src)
         */
        Mapping_HashTable ()
            requires (Cryptography::Digest::IHashFunction<std::hash<KEY_TYPE>, KEY_TYPE> and IEqualsComparer<std::equal_to<KEY_TYPE>, KEY_TYPE>);
        template <DataStructures::HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_VALUE_TYPE> HASH_TABLE_TRAITS = DataStructures::HashTable_Support::DefaultTraits<KEY_TYPE, MAPPED_VALUE_TYPE>>
        Mapping_HashTable (HASHTABLE<HASH_TABLE_TRAITS>&& src)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
            requires (default_initializable<Mapping_HashTable>)
#endif
        ;
        template <Cryptography::Digest::IHashFunction<KEY_TYPE> HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        explicit Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer);
        Mapping_HashTable (Mapping_HashTable&&) noexcept      = default;
        Mapping_HashTable (const Mapping_HashTable&) noexcept = default;
        Mapping_HashTable (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
            requires (default_initializable<Mapping_HashTable>)
#endif
        ;
        template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
            requires (Cryptography::Digest::IHashFunction<HASH, KEY_TYPE>)
        Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOfTo<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (
#if qCompilerAndStdLib_default_initializable_broken_Buggy
                is_default_constructible_v<Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>
#else
                default_initializable<Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>
#endif
                and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit Mapping_HashTable (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Mapping_HashTable{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOfTo<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (Cryptography::Digest::IHashFunction<HASH, KEY_TYPE> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Mapping_HashTable{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_HashTable (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
            requires (default_initializable<Mapping_HashTable>)
#endif
        ;
        template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (Cryptography::Digest::IHashFunction<HASH, KEY_TYPE>);

    public:
        /**
         */
        nonvirtual Mapping_HashTable& operator= (Mapping_HashTable&&) noexcept = default;
        nonvirtual Mapping_HashTable& operator= (const Mapping_HashTable&)     = default;

    private:
        using IImplRepBase_ = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;
        template <DataStructures::HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_VALUE_TYPE> HASH_TABLE_TRAITS>
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
#include "Mapping_HashTable.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_HashTable_h_ */