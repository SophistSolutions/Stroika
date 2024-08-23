/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/SortedMapping.h"

#include <map>

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    using Common::IInOrderComparer;

    /**
     *  \brief   SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedMapping<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note Runtime performance/complexity:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *      o   O (log N) adds, removes and lookups
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_stdmap : public SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using KeyInOrderKeyComparerType    = typename inherited::KeyInOrderKeyComparerType;
        using value_type                   = typename inherited::value_type;
        using key_type                     = typename inherited::key_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside SortedMapping_stdmap
         */
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<KEY_TYPE>>
        using STDMAP = map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER,
                           Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on SortedMapping<> constructor
         */
        SortedMapping_stdmap ()
            requires (totally_ordered<KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit SortedMapping_stdmap (STDMAP<KEY_INORDER_COMPARER>&& src);
        SortedMapping_stdmap (SortedMapping_stdmap&& src) noexcept      = default;
        SortedMapping_stdmap (const SortedMapping_stdmap& src) noexcept = default;
        SortedMapping_stdmap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (totally_ordered<KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit SortedMapping_stdmap (ITERABLE_OF_ADDABLE&& src)
            requires (totally_ordered<KEY_TYPE>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedMapping_stdmap{}
        {
            AssertRepValidType_ ();
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedMapping_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (totally_ordered<KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedMapping_stdmap& operator= (SortedMapping_stdmap&& rhs) noexcept = default;
        nonvirtual SortedMapping_stdmap& operator= (const SortedMapping_stdmap& rhs)     = default;

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
#include "SortedMapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_ */
