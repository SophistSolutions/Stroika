/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>

#include "Stroika/Foundation/Containers/SortedAssociation.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_h_

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   SortedAssociation_stdmultimap<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedAssociation<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note Performance Notes:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_stdmultimap : public SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using KeyInOrderKeyComparerType    = typename inherited::KeyInOrderKeyComparerType;
        using value_type                   = typename inherited::value_type;
        using key_type                     = typename inherited::key_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief STDMULTIMAP is std::multimap<> that can be used inside SortedAssociation_stdmultimap
         */
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER = less<key_type>>
        using STDMULTIMAP =
            multimap<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER,
                     Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on SortedAssociation<> constructor
         */
        SortedAssociation_stdmultimap ();
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit SortedAssociation_stdmultimap (STDMULTIMAP<KEY_INORDER_COMPARER>&& src);
        SortedAssociation_stdmultimap (SortedAssociation_stdmultimap&& src) noexcept      = default;
        SortedAssociation_stdmultimap (const SortedAssociation_stdmultimap& src) noexcept = default;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit SortedAssociation_stdmultimap (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedAssociation_stdmultimap{}
        {
            AssertRepValidType_ ();
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedAssociation_stdmultimap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedAssociation_stdmultimap& operator= (SortedAssociation_stdmultimap&& rhs) noexcept = default;
        nonvirtual SortedAssociation_stdmultimap& operator= (const SortedAssociation_stdmultimap& rhs)     = default;

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
#include "SortedAssociation_stdmultimap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_h_ */
