/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_SkipList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/Private/SkipListSupport.h"
#include "Stroika/Foundation/Containers/SortedAssociation.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   SortedAssociation_SkipList<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedAssociation<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note Runtime performance/complexity:
     *      @todo
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_SkipList : public SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE> {
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
         *  AddOrExtendOrReplaceMode::eAddExtras is the most common behavior for Association, so default to that in SkipList.
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<KEY_TYPE, KEY_COMPARER, AddOrExtendOrReplaceMode::eAddExtras>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside SortedMapping_SkipList
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<KEY_TYPE, MAPPED_VALUE_TYPE, SKIPLISTTRAITS<KEY_COMPARER>>;

    public:
        /**
         *  \see docs on SortedAssociation<> constructor
         */
        SortedAssociation_SkipList ();
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER>
        explicit SortedAssociation_SkipList (KEY_COMPARER&& inorderComparer);
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER>
        explicit SortedAssociation_SkipList (SKIPLIST<KEY_COMPARER>&&);
        SortedAssociation_SkipList (SortedAssociation_SkipList&& src) noexcept  = default;
        SortedAssociation_SkipList (const SortedAssociation_SkipList&) noexcept = default;
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER>
        SortedAssociation_SkipList (KEY_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit SortedAssociation_SkipList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedAssociation_SkipList{}
        {
            AssertRepValidType_ ();
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        SortedAssociation_SkipList (KEY_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedAssociation_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedAssociation_SkipList (KEY_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedAssociation_SkipList& operator= (SortedAssociation_SkipList&&) noexcept = default;
        nonvirtual SortedAssociation_SkipList& operator= (const SortedAssociation_SkipList&)     = default;

    private:
        using IImplRepBase_ = typename SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IInOrderComparer<KEY_TYPE>) KEY_COMPARER>
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
#include "SortedAssociation_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedAssociation_SkipList_h_ */
