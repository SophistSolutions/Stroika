/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_SkipList_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_SkipList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IThreeWayComparer;

    /**
     *  \brief   Mapping_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_SkipList : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLISTTRAITS = DataStructures::SkipList_Support::DefaultTraits<KEY_TYPE, KEY_COMPARER, AddOrExtendOrReplaceMode::eAddIfMissing>;

        /**
         *  \brief SKIPLIST is SkipList that can be used inside Mapping_SkipList
         */
        template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way>
        using SKIPLIST = DataStructures::SkipList<KEY_TYPE, MAPPED_VALUE_TYPE, SKIPLISTTRAITS<KEY_COMPARER>>;

    public:
        /**
         *  \see docs on Mapping<> constructor, except that KEY_EQUALS_COMPARER is replaced with KEY_THREEWAY_COMPARER and EqualsComparer is replaced by IInOrderComparer
         *       and added Mapping_SkipList (SKIPLIST<>&& src)
         */
        Mapping_SkipList ()
            requires (three_way_comparable<KEY_TYPE>);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER>
        Mapping_SkipList (SKIPLIST<KEY_THREEWAY_COMPARER>&& src);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER>
        explicit Mapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer);
        Mapping_SkipList (Mapping_SkipList&& src) noexcept      = default;
        Mapping_SkipList (const Mapping_SkipList& src) noexcept = default;
        Mapping_SkipList (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (three_way_comparable<KEY_TYPE>);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER>
        Mapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit Mapping_SkipList (ITERABLE_OF_ADDABLE&& src)
            requires (three_way_comparable<KEY_TYPE>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Mapping_SkipList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Mapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (three_way_comparable<KEY_TYPE>);
        template <IThreeWayComparer<KEY_TYPE> KEY_THREEWAY_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_SkipList (KEY_THREEWAY_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Mapping_SkipList& operator= (Mapping_SkipList&& rhs) noexcept = default;
        nonvirtual Mapping_SkipList& operator= (const Mapping_SkipList& rhs)     = default;

    private:
        class IImplRepBase_;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IThreeWayComparer<KEY_TYPE>) KEY_THREEWAY_COMPARER>
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
#include "Mapping_SkipList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_SkipList_h_ */
