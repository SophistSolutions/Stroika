/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

//#include <map>
#include <unordered_map>

#include "../../Common/Compare.h"
#include "../../Cryptography/Digest/Hash.h"
#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdhashmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdhashmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Cryptography::Digest::IsHashFunction;

    template <typename KEY_TYPE>
    concept Mapping_stdhashmap_IsDefaultConstructible =
        IsHashFunction < std::hash<KEY_TYPE>,
    KEY_TYPE >
        and Common::IsEqualsComparer<std::equal_to<KEY_TYPE>, KEY_TYPE> ();

#if defined(__clang__) && defined(__APPLE__)
#define qCOMPILERBUG_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy 1
#else
#define qCOMPILERBUG_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy 0
#endif

    /**
     *  \brief   Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     *  \note   \em Implementation Details
     *          This module is essentially identical to SortedMapping_stdhashmap, but making it dependent on SortedMapping<> creates
     *          problems with circular dependencies - especially give how the default Mapping CTOR calls the factory class
     *          which maps back to the _stdhashmap<> variant.
     *
     *          There maybe another (better) way, but this works.
     *
     * \note Performance Notes:
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_stdhashmap : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v  = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief STDHASHMAP is std::map<> that can be used inside Mapping_stdhashmap
         * 
         *      STK appraoch to blockallocation not working for unordred map cuz allocates many at a time
         */
        template <typename HASH = std::hash<key_type>, typename KEY_EQUALS_COMPARER = std::equal_to<key_type>>
        using STDHASHMAP = unordered_map<KEY_TYPE, MAPPED_VALUE_TYPE, HASH, KEY_EQUALS_COMPARER>;
        //template <typename HASH = std::hash<key_type>, typename KEY_EQUALS_COMPARER = std::equal_to<key_type>>
        //using STDHASHMAP = unordered_map<KEY_TYPE, MAPPED_VALUE_TYPE, HASH, KEY_EQUALS_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
        *   @todo UPDATE THESE DOCS - REVIEW AND COMPARE - BUT THIS IS LITERALLY QUTIE WRONG
         *  \see docs on Mapping<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_INORDER_COMPARER and IsEqualsComparer is replaced by IsStrictInOrderComparer
         *       and added Mapping_stdhashmap (STDHASHMAP<>&& src)
         */
        Mapping_stdhashmap ()
            requires (Mapping_stdhashmap_IsDefaultConstructible<KEY_TYPE>);
        Mapping_stdhashmap (STDHASHMAP<>&& src)
            requires (Mapping_stdhashmap_IsDefaultConstructible<KEY_TYPE>);
        template <typename HASH, typename KEY_EQUALS_COMPARER>
        explicit Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer)
            requires (
                Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE> and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ());
        Mapping_stdhashmap (Mapping_stdhashmap&& src) noexcept      = default;
        Mapping_stdhashmap (const Mapping_stdhashmap& src) noexcept = default;
        Mapping_stdhashmap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (Mapping_stdhashmap_IsDefaultConstructible<KEY_TYPE>);
        template <typename HASH, typename KEY_EQUALS_COMPARER>
        Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (
                Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE> and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ());
        template <typename ITERABLE_OF_ADDABLE>
        explicit Mapping_stdhashmap (ITERABLE_OF_ADDABLE&& src)
            requires (
                Mapping_stdhashmap_IsDefaultConstructible<KEY_TYPE> and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>
#if !qCOMPILERBUG_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
                and not is_base_of_v<Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>
#endif
            );
        template <typename HASH, typename KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE>
        Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (
                Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE> and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>
#if !qCOMPILERBUG_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
                and not is_base_of_v<Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>
#endif
            );
#if 0
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Mapping_stdhashmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Mapping_stdhashmap (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
#endif

    public:
        /**
         */
        nonvirtual Mapping_stdhashmap& operator= (Mapping_stdhashmap&& rhs) noexcept = default;
        nonvirtual Mapping_stdhashmap& operator= (const Mapping_stdhashmap& rhs)     = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _MappingRepSharedPtr  = typename inherited::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename HASH, typename KEY_EQUALS_COMPARER>
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
#include "Mapping_stdhashmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_stdhashmap_h_ */
