/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>

#include "Stroika/Foundation/Containers/Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IInOrderComparer;

    /**
     *  \brief   Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     *  \note   \em Implementation Details
     *          This module is essentially identical to SortedMapping_stdmap, but making it dependent on SortedMapping<> creates
     *          problems with circular dependencies - especially give how the default Mapping CTOR calls the factory class
     *          which maps back to the _stdmap<> variant.
     *
     *          There maybe another (better) way, but this works.
     *
     * \note Performance Notes:
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_stdmap : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  Generic std::function-based function used to (strictly) compare two keys
         */
        using key_compare =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (KEY_TYPE, KEY_TYPE)>>;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside Mapping_stdmap
         */
        template <Common::IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        using STDMAP = map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER,
                           Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on Mapping<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_INORDER_COMPARER and EqualsComparer is replaced by IInOrderComparer
         *       and added Mapping_stdmap (STDMAP<>&& src)
         */
        Mapping_stdmap ()
            requires (totally_ordered<KEY_TYPE>);
        template <Common::IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        Mapping_stdmap (const STDMAP<KEY_INORDER_COMPARER>& src);
        template <Common::IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        Mapping_stdmap (STDMAP<KEY_INORDER_COMPARER>&& src);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer);
        Mapping_stdmap (Mapping_stdmap&& src) noexcept      = default;
        Mapping_stdmap (const Mapping_stdmap& src) noexcept = default;
        Mapping_stdmap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (totally_ordered<KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit Mapping_stdmap (ITERABLE_OF_ADDABLE&& src)
            requires (totally_ordered<KEY_TYPE>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Mapping_stdmap{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (totally_ordered<KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Mapping_stdmap& operator= (Mapping_stdmap&& rhs) noexcept = default;
        nonvirtual Mapping_stdmap& operator= (const Mapping_stdmap& rhs)     = default;

    public:
        /**
        * \req KEY_INORDER_COMPARER == key_compare or the type used to construct the original Mapping_stdmap container.
         */
        template <Common::IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = key_compare>
        nonvirtual KEY_INORDER_COMPARER GetInOrderKeyComparer () const;

    public:
        /**
         *  Extend to support STDMAP<key_compare> and STDMAP<KEY_INORDER_COMPARER> used to construct this object (require)
         */
        template <typename CONTAINER_OF_Key_T>
        nonvirtual CONTAINER_OF_Key_T As () const;

    private:
        class IImplRepBase_;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IInOrderComparer<KEY_TYPE>) KEY_INORDER_COMPARER>
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
#include "Mapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
