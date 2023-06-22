/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <map>

#include "../SortedMultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_ 1

/**
  *  \file
  *
  *  \version    <a href="Code-Status.md#Beta">Beta</a>
  *
  */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *
     *
     * \note Performance Notes:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class SortedMultiSet_stdmap : public SortedMultiSet<T, TRAITS> {
    private:
        using inherited = SortedMultiSet<T, TRAITS>;

    public:
        using TraitsType                  = typename inherited::TraitsType;
        using CounterType                 = typename inherited::CounterType;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using ElementInOrderComparerType  = typename inherited::ElementInOrderComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside MultiSet_stdmap
         */
        template <IInOrderComparer<T> INORDER_COMPARER = less<T>>
        using STDMAP =
            map<T, CounterType, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const T, CounterType>, sizeof (value_type) <= 1024>>;

    public:
        /**
         */
        SortedMultiSet_stdmap ();
        template <IInOrderComparer<T> INORDER_COMPARER>
        explicit SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer);
        SortedMultiSet_stdmap (SortedMultiSet_stdmap&& src) noexcept      = default;
        SortedMultiSet_stdmap (const SortedMultiSet_stdmap& src) noexcept = default;
        SortedMultiSet_stdmap (const initializer_list<T>& src);
        template <IInOrderComparer<T> INORDER_COMPARER>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src);
        SortedMultiSet_stdmap (const initializer_list<value_type>& src);
        template <IInOrderComparer<T> INORDER_COMPARER>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src);
        template <IIterableOf<CountedValue<T>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMultiSet_stdmap<T, TRAITS>>)
        explicit SortedMultiSet_stdmap (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedMultiSet_stdmap{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInOrderComparer<T> INORDER_COMPARER, IIterableOf<CountedValue<T>> ITERABLE_OF_ADDABLE>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
        SortedMultiSet_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<T> INORDER_COMPARER, IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedMultiSet_stdmap& operator= (SortedMultiSet_stdmap&& rhs) noexcept = default;
        nonvirtual SortedMultiSet_stdmap& operator= (const SortedMultiSet_stdmap& rhs)     = default;

    private:
        class IImplRepBase_;
        template <IInOrderComparer<T> INORDER_COMPARER>
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
#include "SortedMultiSet_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_*/
