/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <map>

#include "../Mapping.h"

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
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v  = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside Mapping_stdmap
         */
        template <typename /*Common::IInOrderComparer<KEY_TYPE>*/ KEY_INORDER_COMPARER = less<key_type>>
        using STDMAP = map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER,
                           Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on Mapping<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_INORDER_COMPARER and EqualsComparer is replaced by IInOrderComparer
         *       and added Mapping_stdmap (STDMAP<>&& src)
         */
        Mapping_stdmap ();
        Mapping_stdmap (STDMAP<>&& src);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer);
        Mapping_stdmap (Mapping_stdmap&& src) noexcept      = default;
        Mapping_stdmap (const Mapping_stdmap& src) noexcept = default;
        Mapping_stdmap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit Mapping_stdmap (ITERABLE_OF_ADDABLE&& src)
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
        Mapping_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_stdmap (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Mapping_stdmap& operator= (Mapping_stdmap&& rhs) noexcept = default;
        nonvirtual Mapping_stdmap& operator= (const Mapping_stdmap& rhs)     = default;

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
#include "Mapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
